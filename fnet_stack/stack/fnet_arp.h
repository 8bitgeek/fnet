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
* @file fnet_arp.h
*
* @author Andrey Butok
*
* @brief Private. ARP protocol function definitions, data structures, etc
*
***************************************************************************/

#ifndef _FNET_ARP_H_

#define _FNET_ARP_H_

#include "fnet_config.h"

#if (FNET_CFG_CPU_ETH0 ||FNET_CFG_CPU_ETH1) && FNET_CFG_IP4

#include "fnet.h"
#include "fnet_netif_prv.h"
#include "fnet_eth.h"
#include "fnet_timer_prv.h"

/************************************************************************
*    ARP definitions
*************************************************************************/
#define FNET_ARP_HARD_TYPE      (1U)         /* for Ethernet.*/

#define FNET_ARP_HARD_SIZE      (6U)         /* for Ethernet.*/
#define FNET_ARP_PROT_SIZE      (4U)         /* for IP.*/

#define FNET_ARP_OP_REQUEST     (1U)         /* ARP request.*/
#define FNET_ARP_OP_REPLY       (2U)         /* ARP reply.*/

#define FNET_ARP_TIMER_PERIOD   ((FNET_CFG_ARP_EXPIRE_TIMEOUT*1000U)/4U)     /* in ms (20/4=5min).*/

/**************************************************************************/ /*!
 * @internal
 * @brief    ARP header structure.
 ******************************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct
{
    fnet_uint16_t   hard_type FNET_COMP_PACKED;         /**< The type of hardware address (=1 for Ethernet).*/
    fnet_uint16_t   prot_type FNET_COMP_PACKED;         /**< The type of protocol address (=0x0800 for IP).*/
    fnet_uint8_t    hard_size FNET_COMP_PACKED;         /**< The size in bytes of the hardware address (=6).*/
    fnet_uint8_t    prot_size FNET_COMP_PACKED;         /**< The size in bytes of the protocol address (=4).*/
    fnet_uint16_t   op FNET_COMP_PACKED;                /**< Opcode.*/
    fnet_mac_addr_t sender_hard_addr FNET_COMP_PACKED;  /**< Hardware address of sender of this packet.*/
    fnet_ip4_addr_t sender_prot_addr FNET_COMP_PACKED;  /**< Protocol address of sender of this packet.*/
    fnet_mac_addr_t target_hard_addr FNET_COMP_PACKED;  /**< Hardware address of target of this packet.*/
    fnet_ip4_addr_t targer_prot_addr FNET_COMP_PACKED;  /**< Protocol address of target of this packet.*/
} fnet_arp_header_t;
FNET_COMP_PACKED_END

/**************************************************************************/ /*!
 * @internal
 * @brief    ARP table entry structure.
 ******************************************************************************/
typedef struct
{
    fnet_mac_addr_t hard_addr;  /**< Hardware address.*/
    fnet_ip4_addr_t prot_addr;   /**< Protocol address.*/
    fnet_time_t     cr_time;      /**< Time of entry creation.*/
    fnet_netbuf_t   *hold;        /**< Last packet until resolved/timeout.*/
    fnet_time_t     hold_time;    /**< Time of the last request.*/
} fnet_arp_entry_t;

typedef struct
{
    fnet_arp_entry_t arp_table[FNET_CFG_ARP_TABLE_SIZE]; /* ARP cach.*/
    fnet_timer_desc_t arp_tmr;                       /* ARP timer.*/
    fnet_event_desc_t arp_event;                     /* ARP event - duplicate address event.*/
} fnet_arp_if_t;

/************************************************************************
*     Function Prototypes
*************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

fnet_return_t fnet_arp_init( fnet_netif_t *netif );
void fnet_arp_release( fnet_netif_t *netif );
void fnet_arp_request( fnet_netif_t *netif, fnet_ip4_addr_t ipaddr );
fnet_mac_addr_t *fnet_arp_lookup( fnet_netif_t *netif, fnet_ip4_addr_t ipaddr );
void fnet_arp_resolve( fnet_netif_t *netif, fnet_ip4_addr_t ipaddr, fnet_netbuf_t *nb );
void fnet_arp_input( fnet_netif_t *netif, fnet_netbuf_t *nb );
void fnet_arp_drain( fnet_netif_t *netif );

#if defined(__cplusplus)
}
#endif

#endif

#endif
