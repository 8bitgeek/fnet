/**************************************************************************
* 
* Copyright 2014 by Andrey Butok. FNET Community.
*
***************************************************************************
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License Version 3 
* or later (the "LGPL").
*
* As a special exception, the copyright holders of the FNET project give you
* permission to link the FNET sources with independent modules to produce an
* executable, regardless of the license terms of these independent modules,
* and to copy and distribute the resulting executable under terms of your 
* choice, provided that you also meet, for each linked independent module,
* the terms and conditions of the license of that module.
* An independent module is a module which is not derived from or based 
* on this library. 
* If you modify the FNET sources, you may extend this exception 
* to your version of the FNET sources, but you are not obligated 
* to do so. If you do not wish to do so, delete this
* exception statement from your version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* You should have received a copy of the GNU General Public License
* and the GNU Lesser General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*
**********************************************************************/ /*!
*
* @file fnet_llmnr.c
*
* @author Andrey Butok
*
* @brief LLMNR Server/Responder implementation (RFC4795).
*
***************************************************************************/


#include "fnet.h"

#if FNET_CFG_LLMNR

#include "fnet_llmnr.h"
#include "services/dns/fnet_dns_prv.h"

#if FNET_CFG_DEBUG_LLMNR    
    #define FNET_DEBUG_LLMNR   FNET_DEBUG
#else
    #define FNET_DEBUG_LLMNR(...)
#endif


/************************************************************************
*     Definitions
*************************************************************************/

/* LLMNR-server states. */
typedef enum
{
    FNET_LLMNR_STATE_DISABLED = 0,       /**< @brief The LLMNR server is not 
                                             * initialized or released.  */
    FNET_LLMNR_STATE_WAITING_REQUEST,    /**< @brief LLMNR server is waiting 
                                             * for a request from a LLMNR client. */
} fnet_llmnr_state_t;


/* RFC 4795: The IPv4 link-scope multicast address a given responder listens to, and to which a
 * sender sends queries, is 224.0.0.252.*/
#define FNET_LLMNR_IP4_LINK_LOCAL_MULTICAST_ADDR   FNET_IP4_ADDR_INIT(224, 0, 0, 252)

/* RFC 4795: The IPv6 link-scope multicast address a given responder listens to,
 * and to which a sender sends all queries, is FF02:0:0:0:0:0:1:3.*/
const fnet_ip6_addr_t fnet_llmnr_ip6_link_local_multicast_addr = {FNET_IP6_ADDR_INIT(0xFF, 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0x00, 0x03)};

/* Error strings.*/
#define FNET_LLMNR_ERR_PARAMS            "LLMNR: Wrong input parameters."
#define FNET_LLMNR_ERR_SOCKET_CREATION   "LLMNR: Socket creation error."
#define FNET_LLMNR_ERR_SOCKET_BIND       "LLMNR: Socket Error during bind."
#define FNET_LLMNR_ERR_SERVICE           "LLMNR: Service registration is failed."
#define FNET_LLMNR_ERR_IS_INITIALIZED    "LLMNR: DNS is already initialized."
#define FNET_LLMNR_ERR_JOIN_MULTICAST    "LLMNR: Joining to multicast group is failed."

#define FNET_LLMNR_MESSAGE_SIZE     (FNET_DNS_MESSAGE_SIZE) /* Messages carried by UDP are restricted to 512 bytes (not counting the IP
                                                            * or UDP headers).  
                                                            * Longer messages (not supported) are truncated and the TC bit is set in
                                                            * the header.*/    

/* For UDP queries and responses, the Hop Limit field in the IPv6 header
 * and the TTL field in the IPV4 header MAY be set to any value.
 * However, it is RECOMMENDED that the value 255 be used for
 * compatibility with early implementations of [RFC3927]. */
#define FNET_LLMNR_TTL              (255)            


/************************************************************************
*    [RFC 4795 2.1.1.]  LLMNR Header Format
*************************************************************************
   LLMNR queries and responses utilize the DNS header format defined in
   [RFC1035] with exceptions noted below:
        0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                      ID                       |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |QR|   Opcode  | C|TC| T| Z| Z| Z| Z|   RCODE   |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                    QDCOUNT                    |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                    ANCOUNT                    |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                    NSCOUNT                    |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      |                    ARCOUNT                    |
      +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/
    
FNET_COMP_PACKED_BEGIN
typedef struct
{
    unsigned short id       FNET_COMP_PACKED;      /* A 16-bit identifier assigned by the program that generates
                                                    * any kind of query.  This identifier is copied from the query
                                                    * to the response and can be used by the sender to match
                                                    * responses to outstanding queries.  The ID field in a query
                                                    * SHOULD be set to a pseudo-random value. */
    unsigned short flags    FNET_COMP_PACKED;       /* Flags.*/
    unsigned short qdcount  FNET_COMP_PACKED;       /* An unsigned 16-bit integer specifying the number of entries
                                                     * in the question section.  A sender MUST place only one
                                                     * question into the question section of an LLMNR query.  LLMNR
                                                     * responders MUST silently discard LLMNR queries with QDCOUNT
                                                     * not equal to one.  LLMNR senders MUST silently discard LLMNR
                                                     * responses with QDCOUNT not equal to one.*/
    unsigned short ancount  FNET_COMP_PACKED;       /* An unsigned 16-bit integer specifying the number of resource
                                                     * records in the answer section.  LLMNR responders MUST
                                                     * silently discard LLMNR queries with ANCOUNT not equal to
                                                     * zero.*/
    unsigned short nscount  FNET_COMP_PACKED;       /* An unsigned 16-bit integer specifying the number of name
                                                     * server resource records in the authority records section.
                                                     * Authority record section processing is described in Section
                                                     * 2.9.  LLMNR responders MUST silently discard LLMNR queries
                                                     * with NSCOUNT not equal to zero.*/
    unsigned short arcount  FNET_COMP_PACKED;       /* An unsigned 16-bit integer specifying the number of resource
                                                     * records in the additional records section.*/

} fnet_llmnr_header_t;
FNET_COMP_PACKED_END

/* LLMNR Header Flags.*/
#define FNET_LLMNR_HEADER_FLAGS_QR      (0x8000)    /* Query/Response.  A 1-bit field, which, if set, indicates that
                                                    * the message is an LLMNR response; if clear, then the message
                                                    * is an LLMNR query.*/
#define FNET_LLMNR_HEADER_FLAGS_OPCODE  (0x7800)    /* A 4-bit field that specifies the kind of query in this
                                                    * message.  This value is set by the originator of a query and
                                                    * copied into the response.  This specification defines the
                                                    * behavior of standard queries and responses (opcode value of
                                                    * zero).  Future specifications may define the use of other
                                                    * opcodes with LLMNR.  LLMNR senders and responders MUST
                                                    * support standard queries (opcode value of zero).  LLMNR
                                                    * queries with unsupported OPCODE values MUST be silently
                                                    * discarded by responders.*/
#define FNET_LLMNR_HEADER_FLAGS_C       (0x0400)    /* Conflict.  When set within a query, the 'C'onflict bit
                                                    * indicates that a sender has received multiple LLMNR responses
                                                    * to this query.  In an LLMNR response, if the name is
                                                    * considered UNIQUE, then the 'C' bit is clear; otherwise, it
                                                    * is set.  LLMNR senders do not retransmit queries with the 'C'
                                                    * bit set.  Responders MUST NOT respond to LLMNR queries with
                                                    * the 'C' bit set, but may start the uniqueness verification
                                                    * process, as described in Section 4.2. */
#define FNET_LLMNR_HEADER_FLAGS_TC      (0x0200)    /* TrunCation.  The 'TC' bit specifies that this message was
                                                    * truncated due to length greater than that permitted on the
                                                    * transmission channel.  The 'TC' bit MUST NOT be set in an
                                                    * LLMNR query and, if set, is ignored by an LLMNR responder.
                                                    * If the 'TC' bit is set in an LLMNR response, then the sender
                                                    * SHOULD resend the LLMNR query over TCP using the unicast
                                                    * address of the responder as the destination address.  */
#define FNET_LLMNR_HEADER_FLAGS_T       (0x0100)    /* The 'T'entative bit is set in a response if the
                                                    * responder is authoritative for the name, but has not yet
                                                    * verified the uniqueness of the name.  A responder MUST ignore
                                                    * the 'T' bit in a query, if set.  A response with the 'T' bit
                                                    * set is silently discarded by the sender, except if it is a
                                                    * uniqueness query, in which case, a conflict has been detected
                                                    * and a responder MUST resolve the conflict as described in
                                                    * Section 4.1.*/
#define FNET_LLMNR_HEADER_FLAGS_RCODE   (0x000F)    /* Response code.  This 4-bit field is set as part of LLMNR
                                                    * responses.  In an LLMNR query, the sender MUST set RCODE to
                                                    * zero; the responder ignores the RCODE and assumes it to be
                                                    * zero.  The response to a multicast LLMNR query MUST have
                                                    * RCODE set to zero.  A sender MUST silently discard an LLMNR
                                                    * response with a non-zero RCODE sent in response to a
                                                    * multicast query. */



static void fnet_llmnr_state_machine(void *);

/************************************************************************
*    LLMNR server interface structure
*************************************************************************/
struct fnet_llmnr_if
{
    fnet_llmnr_state_t      state;                  /* Current state.*/
    SOCKET                  socket_listen;          /* Listening socket.*/
    fnet_poll_desc_t        service_descriptor;     /* Network interface descriptor. */
    fnet_netif_desc_t       netif;                  /* Service descriptor. */
    const char              *host_name;             /* Link-local host name. */
    fnet_uint32             host_name_ttl;          /* TTL value that indicates for how many seconds link-local host name 
                                                     * is valid for LLMNR querier, in seconds (it is optional).@n
                                                     * Default value is defined by @ref FNET_CFG_LLMNR_HOSTNAME_TTL. */
    char                    message[FNET_LLMNR_MESSAGE_SIZE]; /* Message buffer.*/
};

/* The LLMNR Server interface */ 
static struct fnet_llmnr_if llmnr_if_list[FNET_CFG_LLMNR_MAX];


/************************************************************************
* NAME: fnet_llmnr_is_host_name
*
* DESCRIPTION: Checks if this our host name.
************************************************************************/
static int fnet_llmnr_hostname_cmp(const unsigned char *req_hostname, const unsigned char *hostname)
{
    int             req_hostname_index=0;
    int             hostname_index=0;
    int             i;
    unsigned char   req_hostname_len;
    unsigned char   hostname_c;

    req_hostname_len = req_hostname[req_hostname_index++];
    hostname_c = hostname[hostname_index];

    while(req_hostname_len 
            && ((req_hostname_len & FNET_DNS_NAME_COMPRESSED_MASK) == 0) /* No compression is allowed in query.*/
            && hostname_c )
    {
        for (i=0; i<req_hostname_len; i++)
        {
            if (hostname[hostname_index++] != fnet_tolower(req_hostname[req_hostname_index++]))
            {
                return FNET_FALSE;
            }
        }
        req_hostname_len = req_hostname[req_hostname_index++];
        hostname_c = hostname[hostname_index++];

        if(hostname_c != '.')
        {
            break;
        }
    } 

    return (req_hostname_len == 0) && (hostname_c == 0);
}

/************************************************************************
* NAME: fnet_llmnr_init
*
* DESCRIPTION: Initializes Link-Local Multicast Name Resolution (LLMNR)
*              server/responder service.
************************************************************************/
fnet_llmnr_desc_t fnet_llmnr_init( struct fnet_llmnr_params *params )
{
    struct fnet_llmnr_if    *llmnr_if = 0;
    int                     i;
    struct sockaddr         local_addr;
    int                     option = FNET_LLMNR_TTL;
    unsigned long           host_name_length;

    /* Check input paramters. */
    if((params == 0) || (params->netif_desc == 0) || (params->host_name == 0)
    || ((host_name_length = fnet_strlen(params->host_name)) == 0) || (host_name_length >= FNET_DNS_MAME_SIZE))
    {
        FNET_DEBUG_LLMNR(FNET_LLMNR_ERR_PARAMS);
        goto ERROR_1;
    }

    /* Try to find free LLMNR server descriptor. */
    for(i=0; i < FNET_CFG_LLMNR_MAX; i++)
    {
        if(llmnr_if_list[i].state == FNET_LLMNR_STATE_DISABLED)
        {
            llmnr_if = &llmnr_if_list[i]; 
        }
    }
    
    if(llmnr_if == 0)
    {
        /* No free LLMNR descriptor. */
        FNET_DEBUG_LLMNR(FNET_LLMNR_ERR_IS_INITIALIZED);
        goto ERROR_1;
    }
    
    /* Reset interface structure. */
    fnet_memset_zero(llmnr_if, sizeof(struct fnet_llmnr_if)); 

    /* Set parameters.*/
    llmnr_if->netif = params->netif_desc;
    llmnr_if->host_name = params->host_name;
    llmnr_if->host_name_ttl = params->host_name_ttl;
    if(llmnr_if->host_name_ttl == 0)
    {
        llmnr_if->host_name_ttl = FNET_CFG_LLMNR_HOSTNAME_TTL;
    }

    /* Init local socket address.*/
    fnet_memset_zero(&local_addr, sizeof(local_addr)); 
    local_addr.sa_family = params->addr_family;
    if(local_addr.sa_family == 0)
    {
        local_addr.sa_family = AF_SUPPORTED;
    }
    local_addr.sa_port = FNET_CFG_LLMNR_PORT;

    /* Create listen socket */
    if((llmnr_if->socket_listen = socket(local_addr.sa_family, SOCK_DGRAM, 0)) == SOCKET_INVALID)
    {
        FNET_DEBUG_LLMNR(FNET_LLMNR_ERR_SOCKET_CREATION);
        goto ERROR_1;
    }

    /* Bind socket. */
    if(bind(llmnr_if->socket_listen, &local_addr, sizeof(local_addr)) == SOCKET_ERROR)
    {
        FNET_DEBUG_LLMNR(FNET_LLMNR_ERR_SOCKET_BIND);
        goto ERROR_2;
    }

    /* Join Multicast Group.*/
    #if FNET_CFG_IP4
        if(local_addr.sa_family & AF_INET)
        {
            struct ip_mreq mreq; /* Multicast group information.*/
            
            mreq.imr_multiaddr.s_addr = FNET_LLMNR_IP4_LINK_LOCAL_MULTICAST_ADDR;
            mreq.imr_interface.s_addr = fnet_netif_get_ip4_addr( params->netif_desc );
            
            /* Join multicast group. */
            if(setsockopt(llmnr_if->socket_listen, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) == SOCKET_ERROR) 
    	    {
            	FNET_DEBUG_LLMNR(FNET_LLMNR_ERR_JOIN_MULTICAST);
                goto ERROR_2;		
        	}
            /*
             * For UDP queries and responses, the Hop Limit field in the IPv6 header
             * and the TTL field in the IPV4 header MAY be set to any value.
             * However, it is RECOMMENDED that the value 255 be used for
             * compatibility with early implementations of [RFC3927]. */
            setsockopt(llmnr_if->socket_listen, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &option, sizeof(option));
    	}
    #endif
    #if FNET_CFG_IP6
        if(local_addr.sa_family & AF_INET6)
        {
            struct ipv6_mreq mreq6; /* Multicast group information.*/
            
            FNET_IP6_ADDR_COPY(&fnet_llmnr_ip6_link_local_multicast_addr, &mreq6.ipv6imr_multiaddr.s6_addr);
            mreq6.ipv6imr_interface = fnet_netif_get_scope_id(params->netif_desc); 
            
            /* Join multicast group. */
            if(setsockopt(llmnr_if->socket_listen, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char *)&mreq6, sizeof(mreq6)) == SOCKET_ERROR) 
    	    {
            	FNET_DEBUG_LLMNR(FNET_LLMNR_ERR_JOIN_MULTICAST);
                goto ERROR_2;		
        	}

            /* For UDP queries and responses, the Hop Limit field in the IPv6 header
             * and the TTL field in the IPV4 header MAY be set to any value.
             * However, it is RECOMMENDED that the value 255 be used for
             * compatibility with early implementations of [RFC3927]. */
            setsockopt(llmnr_if->socket_listen, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char *) &option, sizeof(option));
        }
    #endif    	

    /* Register service. */
    llmnr_if->service_descriptor = fnet_poll_service_register(fnet_llmnr_state_machine, (void *) llmnr_if);
    
    if(llmnr_if->service_descriptor == (fnet_poll_desc_t)FNET_ERR)
    {
        FNET_DEBUG_LLMNR(FNET_LLMNR_ERR_SERVICE);
        goto ERROR_2;
    }
    
    llmnr_if->state = FNET_LLMNR_STATE_WAITING_REQUEST; 

    return (fnet_llmnr_desc_t)llmnr_if;

ERROR_2:
    closesocket(llmnr_if->socket_listen);    
ERROR_1:
    return FNET_ERR;
}

/************************************************************************
* NAME: fnet_llmnr_state_machine
*
* DESCRIPTION: LLMNR server state machine.
************************************************************************/
static void fnet_llmnr_state_machine( void *fnet_llmnr_if_p )
{
    struct sockaddr         addr;
    int                     addr_len;      
    int                     received;    
    struct fnet_llmnr_if    *llmnr_if = (struct fnet_llmnr_if *)fnet_llmnr_if_p;
    fnet_llmnr_header_t     *llmnr_header;

    switch(llmnr_if->state)
    {
        /*---- WAITING_REQUEST --------------------------------------------*/
        case FNET_LLMNR_STATE_WAITING_REQUEST:
		    addr_len = sizeof(addr);
            
            received = recvfrom(llmnr_if->socket_listen, llmnr_if->message, sizeof(llmnr_if->message), 0, &addr, &addr_len );

            if(received >= sizeof(fnet_llmnr_header_t) )
            {
                llmnr_header = (fnet_llmnr_header_t *)llmnr_if->message;
               
                if( ((llmnr_header->flags & FNET_HTONS(FNET_LLMNR_HEADER_FLAGS_QR)) == 0) /* Request.*/
                    /* LLMNR senders and responders MUST
                    * support standard queries (opcode value of zero).  LLMNR
                    * queries with unsupported OPCODE values MUST be silently
                    * discarded by responders.*/
                    && ((llmnr_header->flags & FNET_HTONS(FNET_LLMNR_HEADER_FLAGS_OPCODE)) == 0)
                    /* LLMNR responders MUST silently discard LLMNR queries with QDCOUNT
                    * not equal to one.*/
                    && (llmnr_header->qdcount == FNET_HTONS(1))
                    /* LLMNR responders MUST
                    * silently discard LLMNR queries with ANCOUNT not equal to zero.*/
                    && (llmnr_header->ancount == 0)
                    /* LLMNR responders MUST silently discard LLMNR queries
                    * with NSCOUNT not equal to zero.*/
                    && (llmnr_header->nscount == 0)
                )
                {
                    const char          *req_hostname = &llmnr_if->message[sizeof(fnet_llmnr_header_t)];
                    fnet_dns_q_tail_t   *q_tail;
                    unsigned long       req_hostname_len = fnet_strlen(req_hostname);

                    /* Check size */
                    if(received >= (sizeof(fnet_llmnr_header_t) + sizeof(fnet_dns_q_tail_t) + req_hostname_len))
                    {
                        FNET_DEBUG_LLMNR("LLMNR: Req name = %s", req_hostname);
                          
                        /* Responders MUST NOT respond to LLMNR queries for names for which
                           they are not authoritative.*/ 
                        if(fnet_llmnr_hostname_cmp((const unsigned char *)req_hostname, (const unsigned char *)llmnr_if->host_name))
                        {
                            q_tail = (fnet_dns_q_tail_t *)(req_hostname + req_hostname_len);
                            
                            /* Check Question Class. */
                            if (q_tail->qclass == FNET_HTONS(FNET_DNS_HEADER_CLASS_IN) ) 
                            {
                                fnet_dns_rr_header_t    *rr_header = (fnet_dns_rr_header_t*)(q_tail+1);
                                int send_size = (char*)rr_header - (char *)llmnr_header;

                                /* Prepare query response.*/
                            #if FNET_CFG_IP4
                                if(q_tail->qtype == FNET_HTONS(FNET_DNS_TYPE_A))
                                {
                                    FNET_DEBUG_LLMNR("LLMNR: IPv4");

                                    *((fnet_ip4_addr_t*)(&rr_header->rdata)) = fnet_netif_get_ip4_addr(llmnr_if->netif);
                                    rr_header->rdlength = fnet_htons(sizeof(fnet_ip4_addr_t));
                                    rr_header->type = FNET_HTONS(FNET_DNS_TYPE_A);
                                    
                                    send_size += sizeof(fnet_dns_rr_header_t);
                                }else
                            #endif
                            #if FNET_CFG_IP6
                                if(q_tail->qtype == FNET_HTONS(FNET_DNS_TYPE_AAAA))
                                {
                                    fnet_netif_ip6_addr_info_t  addr_info;

                                    FNET_DEBUG_LLMNR("LLMNR: IPv6");
                                    
                                    if(fnet_netif_get_ip6_addr (llmnr_if->netif, 0, &addr_info) == FNET_TRUE)
                                    {
                                        FNET_IP6_ADDR_COPY( &addr_info.address, (fnet_ip6_addr_t*)(&rr_header->rdata));
                                        rr_header->rdlength = fnet_htons(sizeof(fnet_ip6_addr_t));
                                        rr_header->type = FNET_HTONS(FNET_DNS_TYPE_AAAA);

                                        send_size += sizeof(fnet_dns_rr_header_t) - sizeof(unsigned long) + sizeof(fnet_ip6_addr_t);
                                    }
                                    else
                                    {
                                         break; /* No IPv6 address.*/
                                    }
                                }else
                            #endif
                                {
                                    break; /* Not supported query type.*/
                                }
                            
                                /* Init the rest of answer parameters.*/
                                rr_header->ttl = fnet_htonl(llmnr_if->host_name_ttl);
                                rr_header->rr_class = FNET_HTONS(FNET_DNS_HEADER_CLASS_IN);
                                rr_header->name_ptr = fnet_htons((FNET_DNS_NAME_COMPRESSED_MASK<<8) | (FNET_DNS_NAME_COMPRESSED_INDEX_MASK & sizeof(fnet_llmnr_header_t)));

                                /* Updtae LLMNR header response.*/
                                llmnr_header->ancount = FNET_HTONS(1);                          /* One answer.*/
                                llmnr_header->flags |= FNET_HTONS(FNET_LLMNR_HEADER_FLAGS_QR    /* Query response.*/
                                                        |FNET_LLMNR_HEADER_FLAGS_C);            /* The name is not considered unique.*/ 

                                /* A responder responds to a multicast query by sending a unicast UDP response to the sender.*/ 
                                sendto(llmnr_if->socket_listen, llmnr_if->message, send_size, 0, &addr, addr_len);
                            }
                        }
                    }
                }
                /* else = wrong message.*/
            }
            break;
    }
}

/************************************************************************
* NAME: fnet_llmnr_release
*
* DESCRIPTION: eleases the Link-Local Multicast Name Resolution (LLMNR)
* server/responder service.
************************************************************************/ 
void fnet_llmnr_release(fnet_llmnr_desc_t desc)
{
    struct fnet_llmnr_if *llmnr_if = (struct fnet_llmnr_if *)desc;
    
    if(llmnr_if && (llmnr_if->state != FNET_LLMNR_STATE_DISABLED))
    {
        closesocket(llmnr_if->socket_listen);
        
        fnet_poll_service_unregister(llmnr_if->service_descriptor); /* Delete service.*/
        llmnr_if->state = FNET_LLMNR_STATE_DISABLED;
    }
}

/************************************************************************
* NAME: fnet_llmnr_enabled
*
* DESCRIPTION: This function returns FNET_TRUE if the LLMNR server 
*              is enabled/initialised.
************************************************************************/
int fnet_llmnr_enabled(fnet_llmnr_desc_t desc)
{
    struct fnet_llmnr_if    *llmnr_if = (struct fnet_llmnr_if *) desc;
    int                     result;
    
    if(llmnr_if)
        result = (llmnr_if->state == FNET_LLMNR_STATE_DISABLED) ? FNET_FALSE : FNET_TRUE;
    else
        result = FNET_FALSE;    
    
    return result;
}


#endif /* FNET_CFG_LLMNR*/
