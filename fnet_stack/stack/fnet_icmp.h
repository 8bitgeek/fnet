/**************************************************************************
* 
* Copyright 2011-2015 by Andrey Butok. FNET Community.
* Copyright 2008-2010 by Andrey Butok. Freescale Semiconductor, Inc.
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
* @file fnet_icmp.h
*
* @author Andrey Butok
*
* @brief Private. ICMP protocol function definitions, data structures, etc.
*
***************************************************************************/

#ifndef _FNET_ICMP_H_

#define _FNET_ICMP_H_

#include "fnet.h"
#include "fnet_ip_prv.h"
#include "fnet_prot.h"

/************************************************************************
*     Definition of type and code field values.
*************************************************************************/
#define FNET_ICMP_ECHOREPLY                (0u)  /* Echo reply message.*/
#define FNET_ICMP_UNREACHABLE              (3u)  /* Destination Unreachable Message:*/
#define FNET_ICMP_UNREACHABLE_NET          (0u)  /*    -net unreachable*/
#define FNET_ICMP_UNREACHABLE_HOST         (1u)  /*    -host unreachable*/
#define FNET_ICMP_UNREACHABLE_PROTOCOL     (2u)  /*    -protocol unreachable*/
#define FNET_ICMP_UNREACHABLE_PORT         (3u)  /*    -port unreachable*/
#define FNET_ICMP_UNREACHABLE_NEEDFRAG     (4u)  /*    -fragmentation needed and DF set*/
#define FNET_ICMP_UNREACHABLE_SRCFAIL      (5u)  /*    -source route failed*/
#define FNET_ICMP_UNREACHABLE_NET_UNKNOWN  (6u)  /*    -unknown net*/
#define FNET_ICMP_UNREACHABLE_HOST_UNKNOWN (7u)  /*    -unknown host*/
#define FNET_ICMP_UNREACHABLE_ISOLATED     (8u)  /*    -src host isolated*/
#define FNET_ICMP_UNREACHABLE_NET_PROHIB   (9u)  /*    -prohibited access*/
#define FNET_ICMP_UNREACHABLE_HOST_PROHIB  (10u) /*    -ditto*/
#define FNET_ICMP_UNREACHABLE_TOSNET       (11u) /*    -bad tos for net*/
#define FNET_ICMP_UNREACHABLE_TOSHOST      (12u) /*    -bad tos for host*/
#define FNET_ICMP_SOURCEQUENCH             (4u)  /* Source Quench Message, packet lost, slow down.*/
#define FNET_ICMP_REDIRECT                 (5u)  /* Redirect Message:*/
#define FNET_ICMP_REDIRECT_NET             (0u)  /*    -redirect datagrams for the Network*/
#define FNET_ICMP_REDIRECT_HOST            (1u)  /*    -redirect datagrams for the Host*/
#define FNET_ICMP_REDIRECT_TOSNET          (2u)  /*    -redirect datagrams for the Type of Service and Network*/
#define FNET_ICMP_REDIRECT_TOSHOST         (3u)  /*    -redirect datagrams for the Type of Service and Host*/
#define FNET_ICMP_ECHO                     (8u)  /* Echo message.*/
#define FNET_ICMP_ROUTERADVERT             (9u)  /* Router advertisement.*/
#define FNET_ICMP_ROUTERSOLICIT            (10u) /* Router solicitation.*/
#define FNET_ICMP_TIMXCEED                 (11u) /* Time Exceeded Message:*/
#define FNET_ICMP_TIMXCEED_INTRANS         (0u)  /*    -time to live exceeded in transit (ttl==0).*/
#define FNET_ICMP_TIMXCEED_REASS           (1u)  /*    -fragment reassembly time exceeded (ttl==0).*/
#define FNET_ICMP_PARAMPROB                (12u) /* Parameter Problem Message: */
#define FNET_ICMP_PARAMPROB_IPHEDER        (0u)  /*    -IP header bad.*/
#define FNET_ICMP_PARAMPROB_OPTABSENT      (1u)  /*    -required option missing.*/
#define FNET_ICMP_TSTAMP                   (13u) /* Timestamp message (request)*/
#define FNET_ICMP_TSTAMPREPLY              (14u) /* Timestamp reply message*/
#define FNET_ICMP_IREQ                     (15u) /* Information request message*/
#define FNET_ICMP_IREQREPLY                (16u) /* Information reply message*/
#define FNET_ICMP_MASKREQ                  (17u) /* Address mask request.*/
#define FNET_ICMP_MASKREPLY                (18u) /* Address mask reply.*/

/* The macros returns TRUE if "type" is an ICMP request or response type and FALSE if it is an error type. */     
#define FNET_ICMP_IS_QUERY_TYPE(type)  (((type) == FNET_ICMP_ECHO) || ((type) == FNET_ICMP_ECHOREPLY) ||     \
                                        ((type) == FNET_ICMP_TSTAMP) || ((type) == FNET_ICMP_TSTAMPREPLY) || \
                                        ((type) == FNET_ICMP_IREQ) || ((type) == FNET_ICMP_IREQREPLY) ||     \
                                        ((type) == FNET_ICMP_MASKREQ) || ((type) == FNET_ICMP_MASKREPLY) ||  \
                                        ((type) == FNET_ICMP_ROUTERADVERT) || ((type) == FNET_ICMP_ROUTERSOLICIT) )

#define FNET_ICMP_TTL                   (FNET_IP_TTL_DEFAULT)   /* The TTL of ICMP messages.*/
#define FNET_ICMP_TOS                   (IP_TOS_NORMAL)         /* The TOS of ICMP messages.*/


/**************************************************************************/ /*!
 * @internal
 * @brief    ICMP message header.
 ******************************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct
{
    fnet_uint8_t   type FNET_COMP_PACKED;      /* The type of the message.*/
    fnet_uint8_t   code FNET_COMP_PACKED;      /* The code of the message.*/
    fnet_uint16_t  checksum FNET_COMP_PACKED;  /* The checksum of the message.*/
} fnet_icmp_header_t;
FNET_COMP_PACKED_END

/**************************************************************************/ /*!
 * @internal
 * @brief    ICMP Echo message header.
 ******************************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct
{
    fnet_icmp_header_t  header FNET_COMP_PACKED;            /**< The ICMP standard header.*/
    fnet_uint16_t       identifier FNET_COMP_PACKED;        /**< Identifier.*/
    fnet_uint16_t       sequence_number FNET_COMP_PACKED;   /**< Sequence Number.*/
} fnet_icmp_echo_header_t;
FNET_COMP_PACKED_END

/**************************************************************************/ /*!
 * @internal
 * @brief    ICMP Error message header.
 ******************************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct
{
    fnet_icmp_header_t header FNET_COMP_PACKED;      /**< The ICMP standard header.*/

    union
    {
        fnet_uint32_t unused;       /**< Unused.*/
        fnet_uint16_t mtu;         /**< MTU.*/
        fnet_uint16_t ptr;         /**< Pointer indicates the error.*/
    } fields FNET_COMP_PACKED;

    fnet_ip_header_t ip FNET_COMP_PACKED;            /**< IP header.*/
} fnet_icmp_err_header_t;
FNET_COMP_PACKED_END

/**************************************************************************/ /*!
 * @internal
 * @brief    ICMP Timestamp message header.
 ******************************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct
{
    fnet_icmp_header_t  header FNET_COMP_PACKED;                /**< The ICMP standard header.*/
    fnet_uint16_t       identifier FNET_COMP_PACKED;            /**< Identifier.*/
    fnet_uint16_t       sequence_number FNET_COMP_PACKED;       /**< Sequence Number.*/
    fnet_uint32_t       originate_timestamp FNET_COMP_PACKED;   /**< Originate timestamp.*/
    fnet_uint32_t       receive_timestamp FNET_COMP_PACKED;     /**< Receive timestamp.*/
    fnet_uint32_t       transmit_timestamp FNET_COMP_PACKED;    /**< Transmit timestamp.*/
} fnet_icmp_timestamp_header_t;
FNET_COMP_PACKED_END

/**************************************************************************/ /*!
 * @internal
 * @brief    ICMP Address mask message header.
 ******************************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct
{
    fnet_icmp_header_t header FNET_COMP_PACKED;          /**< The ICMP standard header.*/
    fnet_uint16_t identifier FNET_COMP_PACKED;          /**< Identifier.*/
    fnet_uint16_t sequence_number FNET_COMP_PACKED;     /**< Sequence Number.*/
    fnet_uint32_t mask FNET_COMP_PACKED;                /**< Subnet mask.*/
} fnet_icmp_mask_header_t;
FNET_COMP_PACKED_END

#if FNET_CFG_IP4

extern fnet_prot_if_t fnet_icmp_prot_if;

/************************************************************************
*     Function Prototypes
*************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

void fnet_icmp_error( fnet_netif_t *netif, fnet_uint8_t type, fnet_uint8_t code, fnet_netbuf_t *nb );

#if defined(__cplusplus)
}
#endif

#endif /* FNET_CFG_IP4 */

#endif /* _FNET_ICMP_H_ */
