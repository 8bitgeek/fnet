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
* and the GNU Lesser General Public Licensalong with this program.
* If not, see <http://www.gnu.org/licenses/>.
*
**********************************************************************/ /*!
*
* @file fnet_udp.h
*
* @author Andrey Butok
*
* @brief Private. UDP protocol definitions.
*
***************************************************************************/

#ifndef _FNET_UDP_H_

#define _FNET_UDP_H_

#if FNET_CFG_UDP

#include "fnet.h"
#include "fnet_socket.h"
#include "fnet_socket_prv.h"
#include "fnet_prot.h"

/************************************************************************
*     UDP definitions
*************************************************************************/
#define FNET_UDP_TTL            (64u)                       /* Default TTL.*/
#define FNET_UDP_TTL_MULTICAST  (1u)                        /* Default TTL for Multicast datagrams.
                                                             * RFC112 6.1: If the upper-layer protocol
                                                             * chooses not to specify a time-to-live, it should
                                                             * default to 1 for all multicast IP datagrams, so that an explicit
                                                             * choice is required to multicast beyond a single network.
                                                             */
#define FNET_UDP_DF             (FNET_FALSE)                       /* DF flag.*/
#define FNET_UDP_TX_BUF_MAX     (FNET_CFG_SOCKET_UDP_TX_BUF_SIZE) /* Default maximum size for send socket buffer.*/
#define FNET_UDP_RX_BUF_MAX     (FNET_CFG_SOCKET_UDP_RX_BUF_SIZE) /* Default maximum size for receive socket buffer.*/

/************************************************************************
*     Global Data Structures
*************************************************************************/
extern fnet_prot_if_t fnet_udp_prot_if;

/* Structure of UDP header.*/
FNET_COMP_PACKED_BEGIN
typedef struct
{
    fnet_uint16_t source_port FNET_COMP_PACKED;      /* Source port number.*/
    fnet_uint16_t destination_port FNET_COMP_PACKED; /* Destination port number.*/
    fnet_uint16_t length FNET_COMP_PACKED;           /* Length.*/
    fnet_uint16_t checksum FNET_COMP_PACKED;         /* Checksum.*/
} fnet_udp_header_t;
FNET_COMP_PACKED_END


#endif  /* FNET_CFG_UDP */

#endif /* _FNET_UDP_H_ */
