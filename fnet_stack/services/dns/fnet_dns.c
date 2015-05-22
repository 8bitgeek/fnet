/**************************************************************************
* 
* Copyright 2011-2015 by Andrey Butok. FNET Community.
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
* @file fnet_dns.c
*
* @author Andrey Butok
*
* @brief DNS Resolver implementation.
*
***************************************************************************/


#include "fnet_config.h"

#if FNET_CFG_DNS_RESOLVER

#include "fnet_dns.h"
#include "fnet_dns_prv.h"

#if FNET_CFG_DEBUG_DNS    
    #define FNET_DEBUG_DNS   FNET_DEBUG
#else
    #define FNET_DEBUG_DNS(...)
#endif

/************************************************************************
*     Definitions
*************************************************************************/

#define FNET_DNS_ERR_PARAMS            "ERROR: Wrong input parameters."
#define FNET_DNS_ERR_SOCKET_CREATION   "ERROR: Socket creation error."
#define FNET_DNS_ERR_SOCKET_CONNECT    "ERROR: Socket Error during connect."
#define FNET_DNS_ERR_SERVICE           "ERROR: Service registration is failed."
#define FNET_DNS_ERR_IS_INITIALIZED    "ERROR: DNS is already initialized."


static void fnet_dns_state_machine(void *);
static unsigned int fnet_dns_add_question( char *message, unsigned short type, char *host_name);

/************************************************************************
*    DNS-client interface structure.
*************************************************************************/
typedef struct
{
    SOCKET                      socket_cln;
    fnet_poll_desc_t            service_descriptor;
    fnet_dns_state_t            state;                          /* Current state. */
    fnet_dns_handler_resolved_t handler;                        /* Callback function. */
    long                        handler_cookie;                 /* Callback-handler specific parameter. */
    unsigned long               last_time;                      /* Last receive time, used for timeout detection. */
    unsigned int                iteration;                      /* Current iteration number.*/
    /* Internal buffer used for Message buffer and Resolved addresses.*/
    union{
    char                        message[FNET_DNS_MESSAGE_SIZE]; /* Message buffer and Resolved addresses.*/
    fnet_ip4_addr_t        	    resolved_ip4_addr[FNET_DNS_MESSAGE_SIZE/sizeof(fnet_ip4_addr_t)]; /* Resolved IPv4 addresses.*/
    fnet_ip6_addr_t        	    resolved_ip6_addr[FNET_DNS_MESSAGE_SIZE/sizeof(fnet_ip6_addr_t)]; /* Resolved IPv6 addresses.*/
    };
    int                         addr_number;
    unsigned long               message_size;                   /* Size of the message.*/
    unsigned short              id;
    unsigned short              dns_type;                       /* DNS Resource Record type that is queried.*/
    fnet_address_family_t       addr_family;
} 
fnet_dns_if_t;


/* DNS-client interface */
static fnet_dns_if_t fnet_dns_if;

/************************************************************************
* NAME: fnet_dns_init
*
* DESCRIPTION: Initializes DNS client service and starts the host 
*              name reolving.
************************************************************************/
static unsigned int fnet_dns_add_question( char *message, unsigned short type, char *host_name)
{
    unsigned int        total_length = 0U;
    unsigned int        label_length;
    fnet_dns_q_tail_t   *q_tail;
    char                *strtok_pos = FNET_NULL;
    

    /* Set Question section :    
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                     QNAME                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QTYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     QCLASS                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    */ 
    /* QNAME */
    /* a domain name represented as a sequence of labels, where
    * each label consists of a length octet followed by that
    * number of octets. The domain name terminates with the
    * zero length octet for the null label of the root. Note
    * that this field may be an odd number of octets; no
    * padding is used.
    */
 
    /* Copy host_name string.*/
    fnet_strcpy(&message[1], host_name); 
 
    /* TBD PFI, use strtok_pos as pointer.*/
    
    /* Replace '.' by zero.*/
    fnet_strtok_r(&message[1], ".", &strtok_pos);

    while((label_length = fnet_strlen(&message[total_length]+1U)) > 0U)
    {
        message[total_length] = (char)label_length; /* Set length before (previous) label.*/
        total_length += label_length + 1U;
       
        fnet_strtok_r(FNET_NULL,".", &strtok_pos);
    }
    
    q_tail = (fnet_dns_q_tail_t *)&message[total_length];
    
    /* Skip 1 byte (zero). End of string. */

    /* QTYPE */
    q_tail->qtype =  type;
    
    /* QCLASS */
    q_tail->qclass = FNET_HTONS(FNET_DNS_HEADER_CLASS_IN);

    return (total_length+ sizeof(fnet_dns_q_tail_t)); 
}


/************************************************************************
* NAME: fnet_dns_init
*
* DESCRIPTION: Initializes DNS client service and starts the host 
*              name resolving.
************************************************************************/
int fnet_dns_init( struct fnet_dns_params *params )
{
    const unsigned long bufsize_option = FNET_DNS_MESSAGE_SIZE;
    unsigned int        total_length;
    unsigned long       host_name_length;
    struct sockaddr     remote_addr;
    fnet_dns_header_t   *header;
  
    /* Check input parameters. */
    if((params == 0) 
        || (params->dns_server_addr.sa_family == AF_UNSPEC) 
        || fnet_socket_addr_is_unspecified(&params->dns_server_addr)
        || (params->handler == 0)
        /* Check length of host_name.*/
        || ((host_name_length = fnet_strlen(params->host_name)) == 0U) || (host_name_length >= FNET_DNS_MAME_SIZE))
    {
        FNET_DEBUG_DNS(FNET_DNS_ERR_PARAMS);
        goto ERROR;
    }

    /* Check if DNS service is free.*/
    if(fnet_dns_if.state != FNET_DNS_STATE_DISABLED)
    {
        FNET_DEBUG_DNS(FNET_DNS_ERR_IS_INITIALIZED);
        goto ERROR;
    }
    
    /* Save input parmeters.*/
    fnet_dns_if.handler = params->handler;
    fnet_dns_if.handler_cookie = params->cookie;
    fnet_dns_if.addr_family = params->addr_family;
    fnet_dns_if.addr_number = 0;

    if(params->addr_family == AF_INET)
    {
        fnet_dns_if.dns_type = FNET_HTONS(FNET_DNS_TYPE_A);
    }
    else
    if(params->addr_family == AF_INET6)
    {
        fnet_dns_if.dns_type = FNET_HTONS(FNET_DNS_TYPE_AAAA);
    }
    else
    {
        FNET_DEBUG_DNS(FNET_DNS_ERR_PARAMS);
        goto ERROR;
    }
    
    fnet_dns_if.iteration = 0U;  /* Reset iteration counter.*/
    fnet_dns_if.id++;           /* Change query ID.*/
   
    /* Create socket */
    if((fnet_dns_if.socket_cln = socket(params->dns_server_addr.sa_family, SOCK_DGRAM, 0)) == SOCKET_INVALID)
    {
        FNET_DEBUG_DNS(FNET_DNS_ERR_SOCKET_CREATION);
        goto ERROR;
    }
    
    /* Set socket options */
    setsockopt(fnet_dns_if.socket_cln, SOL_SOCKET, SO_RCVBUF, (char *) &bufsize_option, sizeof(bufsize_option));
    setsockopt(fnet_dns_if.socket_cln, SOL_SOCKET, SO_SNDBUF, (char *) &bufsize_option, sizeof(bufsize_option));
    
    /* Bind/connect to the server.*/
    FNET_DEBUG_DNS("Connecting to DNS Server.");
    fnet_memset_zero(&remote_addr, sizeof(remote_addr));
    remote_addr = params->dns_server_addr;
    if(remote_addr.sa_port == 0U)
    {
        remote_addr.sa_port = FNET_CFG_DNS_PORT;
    }
   
    if(connect(fnet_dns_if.socket_cln, &remote_addr, sizeof(remote_addr))== FNET_ERR)
    {
        FNET_DEBUG_DNS(FNET_DNS_ERR_SOCKET_CONNECT);
        goto ERROR_1;
    }
    
    /* ==== Build message. ==== */
    fnet_memset_zero(fnet_dns_if.message, sizeof(fnet_dns_if.message)); /* Clear buffer.*/
     
    /* Set header fields:
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
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
    
    header = (fnet_dns_header_t *)fnet_dns_if.message;
    
    header->id = fnet_dns_if.id;            /* Set ID. */
    
    header->flags = FNET_HTONS(FNET_DNS_HEADER_FLAGS_RD); /* Recursion Desired.*/
   
    header->qdcount = FNET_HTONS(1U);        /* One Question. */
    
    /* No Answer (ANCOUNT).*/ /* No Authority (NSCOUNT). */ /* No Additional (ARCOUNT). */


    total_length = sizeof(fnet_dns_header_t);
    total_length += fnet_dns_add_question( &fnet_dns_if.message[total_length], fnet_dns_if.dns_type, params->host_name);
    fnet_dns_if.message_size = (unsigned long)(total_length);
  
    /* Register DNS service. */
    fnet_dns_if.service_descriptor = fnet_poll_service_register(fnet_dns_state_machine, (void *) &fnet_dns_if);
    if(fnet_dns_if.service_descriptor == (fnet_poll_desc_t)FNET_ERR)
    {
        FNET_DEBUG_DNS(FNET_DNS_ERR_SERVICE);
        goto ERROR_1;
    }
    
    fnet_dns_if.state = FNET_DNS_STATE_TX; /* => Send request. */    
   
    return FNET_OK;
ERROR_1:
    closesocket(fnet_dns_if.socket_cln);

ERROR:
    return FNET_ERR;
}

/************************************************************************
* NAME: fnet_dns_state_machine
*
* DESCRIPTION: DNS-client state machine.
************************************************************************/
static void fnet_dns_state_machine( void *fnet_dns_if_p )
{
    int                     sent_size;
    int                     received;    
    unsigned int            i;
    fnet_dns_header_t       *header;
    fnet_dns_rr_header_t    *rr_header;
    fnet_dns_if_t           *dns_if = (fnet_dns_if_t *)fnet_dns_if_p;

    switch(dns_if->state)
    {
        /*---- TX --------------------------------------------*/
        case FNET_DNS_STATE_TX:

            FNET_DEBUG_DNS("Sending query...");
            sent_size = send(dns_if->socket_cln, dns_if->message, dns_if->message_size, 0U);
            
            if (sent_size != (int)dns_if->message_size)
        	{
        		dns_if->state = FNET_DNS_STATE_RELEASE; /* ERROR */
        	}	
            else
            {
                dns_if->last_time = fnet_timer_ticks();
                dns_if->state = FNET_DNS_STATE_RX;
            }		
            break; 
        /*---- RX -----------------------------------------------*/
        case  FNET_DNS_STATE_RX:
            /* Receive data */
            
            received = recv(dns_if->socket_cln, dns_if->message, sizeof(dns_if->message), 0U);
            
            if(received > 0 )
            {
                header = (fnet_dns_header_t *)fnet_dns_if.message;
                
                if((header->id == dns_if->id) && /* Check the ID.*/
                   (header->flags & FNET_DNS_HEADER_FLAGS_QR)) /* Is response.*/
                {
                    for (i=(sizeof(fnet_dns_header_t)-1U); i < (unsigned int)received; i++)
                    {
                        /* [RFC1035 4.1.4.] In order to reduce the size of messages, the domain system utilizes a
                        * compression scheme which eliminates the repetition of domain names in a
                        * message. In this scheme, an entire domain name or a list of labels at
                        * the end of a domain name is replaced with a pointer to a prior occurance
                        * of the same name.
                        * The pointer takes the form of a two octet sequence:
                        * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
                        * | 1  1|                OFFSET                   |
                        * +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
                        */
                        /* => Check for 0xC0. */
                        if ((unsigned char)dns_if->message[i] == FNET_DNS_NAME_COMPRESSED_MASK) /* look for the beginnig of the response (Question Name == 192 (label compression))*/
                        {
                            rr_header = (fnet_dns_rr_header_t *)&dns_if->message[i]; 


                            /* Check Question Type, Class and Resource Data Length. */
                            if ( (rr_header->type ==  dns_if->dns_type) && 
                                 (rr_header->rr_class == FNET_HTONS(FNET_DNS_HEADER_CLASS_IN))) 
                            {
                                /* Resolved.*/
                                if(rr_header->type == FNET_HTONS(FNET_DNS_TYPE_A))
                                {
                                	dns_if->resolved_ip4_addr[dns_if->addr_number] = *((fnet_ip4_addr_t*)(&rr_header->rdata));
                                }
                                else /* AF_INET6 */
                                {
                                    FNET_IP6_ADDR_COPY( (fnet_ip6_addr_t*)(&rr_header->rdata), &dns_if->resolved_ip6_addr[dns_if->addr_number] );
                                }
                                dns_if->addr_number++;
                                  
                            }
                            i+=(unsigned int)(sizeof(fnet_dns_rr_header_t)+fnet_ntohs(rr_header->rdlength)-4U-1U);
                        }
                    }
                }
                /* else = wrong message.*/
                
                dns_if->state = FNET_DNS_STATE_RELEASE;
            }
            else if(received == SOCKET_ERROR) /* Check error.*/
            {
                dns_if->state = FNET_DNS_STATE_RELEASE; /* ERROR */
            }
            else /* No data. Check timeout */
            if(fnet_timer_get_interval(dns_if->last_time, fnet_timer_ticks()) > ((FNET_CFG_DNS_RETRANSMISSION_TIMEOUT*1000U)/FNET_TIMER_PERIOD_MS))
            {
                dns_if->iteration++;
                
                if(dns_if->iteration > FNET_CFG_DNS_RETRANSMISSION_MAX)
                {
                    dns_if->state = FNET_DNS_STATE_RELEASE; /* ERROR */
                }
                else
                {
                    dns_if->state = FNET_DNS_STATE_TX;
                }
            }
            break;
         /*---- RELEASE -------------------------------------------------*/    
        case FNET_DNS_STATE_RELEASE:
            fnet_dns_release(); 
            dns_if->handler( dns_if->addr_family, dns_if->addr_number ? dns_if->message : FNET_NULL, dns_if->addr_number, dns_if->handler_cookie); /* User Callback.*/
            break;
        default:
            break;            
    }

}

/************************************************************************
* NAME: fnet_dns_release
*
* DESCRIPTION: This function aborts the resolving and releases 
* the DNS-client service.
************************************************************************/ 
void fnet_dns_release( void )
{
    if(fnet_dns_if.state != FNET_DNS_STATE_DISABLED)
    {
        /* Close socket. */
        closesocket(fnet_dns_if.socket_cln);
    
        /* Unregister the tftp service. */
        fnet_poll_service_unregister( fnet_dns_if.service_descriptor );
    
        fnet_dns_if.state = FNET_DNS_STATE_DISABLED; 
    }
}

/************************************************************************
* NAME: fnet_dns_state
*
* DESCRIPTION: This function returns a current state of the DNS client.
************************************************************************/
fnet_dns_state_t fnet_dns_state( void )
{
    return fnet_dns_if.state;
}


#endif /* FNET_CFG_DNS_RESOLVER */
