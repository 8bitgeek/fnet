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
* @file fnet_eth_prv.h
*
* @author Andrey Butok
*
* @brief Private. Ethernet platform independent API functions.
*
***************************************************************************/

#ifndef _FNET_ETHERNET_PRV_H_

#define _FNET_ETHERNET_PRV_H_

#include "fnet_config.h"

#if (FNET_CFG_CPU_ETH0 ||FNET_CFG_CPU_ETH1)

#include "fnet_eth.h"
#include "fnet_arp.h"
#include "fnet_netif_prv.h"

/************************************************************************
*     Definitions
*************************************************************************/
/* Ethernet Frame Types */
#define FNET_ETH_TYPE_IP4       (0x0800U)
#define FNET_ETH_TYPE_ARP       (0x0806U)
#define FNET_ETH_TYPE_IP6       (0x86DDU)

#define FNET_ETH_HDR_SIZE       (14U)    /* Size of Ethernet header.*/
#define FNET_ETH_CRC_SIZE       (4U)     /* Size of Ethernet CRC.*/

/************************************************************************
*    Network Layer Protocol interface control structure.
*************************************************************************/
typedef struct fnet_eth_prot_if
{
    fnet_uint16_t protocol;                                    /* Protocol number */
    void (*input)( fnet_netif_t *netif, fnet_netbuf_t *nb );    /* Protocol input function.*/
} fnet_eth_prot_if_t;

/*****************************************************************************
*     Ethernet Frame header.
******************************************************************************/
FNET_COMP_PACKED_BEGIN
typedef struct
{
    fnet_mac_addr_t destination_addr    FNET_COMP_PACKED;   /**< 48-bit destination address.*/
    fnet_mac_addr_t source_addr         FNET_COMP_PACKED;   /**< 48-bit source address.*/
    fnet_uint16_t   type                FNET_COMP_PACKED;   /**< 16-bit type field.*/
} fnet_eth_header_t;
FNET_COMP_PACKED_END

/*****************************************************************************
*     Ethernet Control data structure 
******************************************************************************/
typedef struct fnet_eth_if
{
    void                *if_cpu_ptr;  /* Points to CPU-specific control data structure of the interface. */
    fnet_index_t        mac_number;   /* MAC module number [0-1]. */
    void                ( *output)(fnet_netif_t *netif, fnet_uint16_t type, const fnet_mac_addr_t dest_addr, fnet_netbuf_t* nb);
#if FNET_CFG_MULTICAST      
    void                ( *multicast_join)(fnet_netif_t *netif, fnet_mac_addr_t multicast_addr);
    void                ( *multicast_leave)(fnet_netif_t *netif, fnet_mac_addr_t multicast_addr);
#endif /* FNET_CFG_MULTICAST */
    /* Internal parameters.*/
    fnet_bool_t         connection_flag;
    fnet_timer_desc_t   eth_timer;    /* Optional ETH timer.*/
#if FNET_CFG_IP4    
    fnet_arp_if_t       arp_if;
#endif   
#if FNET_CFG_IP6   
    fnet_nd6_if_t       nd6_if;
#endif 
#if !FNET_CFG_CPU_ETH_MIB     
    struct fnet_netif_statistics statistics;
#endif    
} fnet_eth_if_t;

/************************************************************************
*     Global Data Structures
*************************************************************************/
extern const fnet_mac_addr_t fnet_eth_null_addr;
extern const fnet_mac_addr_t fnet_eth_broadcast;

#if FNET_CFG_CPU_ETH0 
    extern fnet_netif_t fnet_eth0_if;
    #define FNET_ETH0_IF ((fnet_netif_desc_t)(&fnet_eth0_if))
#endif
#if FNET_CFG_CPU_ETH1 
    extern fnet_netif_t fnet_eth1_if;
    #define FNET_ETH1_IF ((fnet_netif_desc_t)(&fnet_eth1_if))
#endif    

/************************************************************************
*     Function Prototypes
*************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

fnet_return_t fnet_eth_init(fnet_netif_t *netif);
void fnet_eth_release( fnet_netif_t *netif);
void fnet_eth_drain(fnet_netif_t *netif);
void fnet_eth_change_addr_notify(fnet_netif_t *netif);

#if FNET_CFG_IP4
    void fnet_eth_output_ip4(fnet_netif_t *netif, fnet_ip4_addr_t dest_ip_addr, fnet_netbuf_t* nb);
#endif 

void fnet_eth_output_low( fnet_netif_t *netif, fnet_uint16_t type, const fnet_mac_addr_t dest_addr,
                          fnet_netbuf_t *nb );
void fnet_eth_prot_input( fnet_netif_t *netif, fnet_netbuf_t *nb, fnet_uint16_t protocol ); 

#if FNET_CFG_MULTICAST
    #if FNET_CFG_IP4 
        void fnet_eth_multicast_leave_ip4(fnet_netif_t *netif, fnet_ip4_addr_t multicast_addr );
        void fnet_eth_multicast_join_ip4(fnet_netif_t *netif, fnet_ip4_addr_t  multicast_addr );
    #endif
    #if FNET_CFG_IP6
        void fnet_eth_multicast_leave_ip6(fnet_netif_t *netif, fnet_ip6_addr_t *multicast_addr );
        void fnet_eth_multicast_join_ip6(fnet_netif_t *netif, const fnet_ip6_addr_t  *multicast_addr );
    #endif        
#endif /* FNET_CFG_MULTICAST */

#if FNET_CFG_IP6
    void fnet_eth_output_ip6(fnet_netif_t *netif, const fnet_ip6_addr_t *src_ip_addr,  const fnet_ip6_addr_t *dest_ip_addr, fnet_netbuf_t* nb);
#endif

#if FNET_CFG_DEBUG_TRACE_ETH && FNET_CFG_DEBUG_TRACE
    void fnet_eth_trace(fnet_uint8_t *str, fnet_eth_header_t *eth_hdr);
#else
    #define fnet_eth_trace(str, eth_hdr)    do{}while(0)
#endif            

#if defined(__cplusplus)
}
#endif              

#endif /* FNET_CFG_ETH */

#endif /* _FNET_ETHERNET_PRV_H_ */
