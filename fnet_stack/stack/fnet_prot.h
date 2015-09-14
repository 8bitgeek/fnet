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
* @file fnet_prot.h
*
* @author Andrey Butok
*
* @brief Private. Transport protocol interface definitions.
*
***************************************************************************/

#ifndef _FNET_PROT_H_

#define _FNET_PROT_H_

#include "fnet_netbuf.h"
#include "fnet_netif.h"
#include "fnet_netif_prv.h"
#include "fnet_tcp.h"
#include "fnet_ip_prv.h"
#include "fnet_ip6_prv.h"
#include "fnet_socket.h"
#include "fnet_socket_prv.h"

/************************************************************************
*    Protocol notify commands.
*************************************************************************/
typedef enum
{
    FNET_PROT_NOTIFY_QUENCH,           /* Some one said to slow down.*/
    FNET_PROT_NOTIFY_MSGSIZE,          /* Message size forced drop.*/
    FNET_PROT_NOTIFY_UNREACH_HOST,     /* No route to host.*/
    FNET_PROT_NOTIFY_UNREACH_PROTOCOL, /* Dst says bad protocol.*/
    FNET_PROT_NOTIFY_UNREACH_PORT,     /* Bad port #.*/
    FNET_PROT_NOTIFY_UNREACH_SRCFAIL,  /* Source route failed.*/
    FNET_PROT_NOTIFY_UNREACH_NET,      /* No route to network.*/
    FNET_PROT_NOTIFY_TIMXCEED_INTRANS, /* Packet time-to-live expired in transit.*/
    FNET_PROT_NOTIFY_TIMXCEED_REASS,   /* Reassembly time-to-leave expired.*/
    FNET_PROT_NOTIFY_PARAMPROB         /* Header incorrect.*/
} fnet_prot_notify_t;

struct fnet_netif; /* Forward declaration.*/

/************************************************************************
*    Transport Layer Protocol interface control structure.
*************************************************************************/
typedef struct fnet_prot_if
{
    fnet_socket_if_t           *head;      /* Pointer to the head of the protocol's socket list.*/
    fnet_address_family_t   family;     /* Address domain family.*/
    fnet_socket_type_t      type;       /* Socket type used for.*/
    fnet_uint32_t           protocol; 
    fnet_return_t           (*prot_init)( void );      /* (Optional) Protocol initialization function.*/
    void                    (*prot_release)( void );   /* (Optional) Protocol release function.*/
    void                    (*prot_input)(fnet_netif_t *netif, struct sockaddr *src_addr,  struct sockaddr *dest_addr, fnet_netbuf_t *nb, fnet_netbuf_t *ip_nb); /* Protocol input function.*/       
    void                    (*prot_control_input)(fnet_prot_notify_t command, struct sockaddr *src_addr,  struct sockaddr *dest_addr, fnet_netbuf_t *nb);  /* (Optional) Protocol input control function.*/ 
    void                    (*prot_drain)( void );      /* Protocol drain function. */
    const fnet_socket_prot_if_t *socket_api;            /* Pointer to Transport Protocol API structure.*/
} fnet_prot_if_t;

/************************************************************************
*     Function Prototypes.
*************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

fnet_return_t fnet_prot_init( void );
void fnet_prot_release( void );
fnet_prot_if_t *fnet_prot_find( fnet_address_family_t family, fnet_socket_type_t type, fnet_uint32_t protocol );
void fnet_prot_drain( void );

#if defined(__cplusplus)
}
#endif

#endif
