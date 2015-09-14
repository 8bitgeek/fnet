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
* and the GNU Lesser General Public Licensalong with this program.
* If not, see <http://www.gnu.org/licenses/>.
*
**********************************************************************/ /*!
*
* @file fnet_raw.h
*
* @author Andrey Butok
*
* @brief RAW socket definitions.
*
***************************************************************************/

#ifndef _FNET_RAW_H_

#define _FNET_RAW_H_

#if FNET_CFG_RAW

#include "fnet.h"
#include "fnet_socket.h"
#include "fnet_socket_prv.h"
#include "fnet_prot.h"

/************************************************************************
*     RAW definitions
*************************************************************************/
#define FNET_RAW_TTL            (64u)                             /* Default TTL for RAW socket.*/
#define FNET_RAW_TX_BUF_MAX     (FNET_CFG_SOCKET_RAW_TX_BUF_SIZE) /* Default maximum size for send socket buffer.*/
#define FNET_RAW_RX_BUF_MAX     (FNET_CFG_SOCKET_RAW_RX_BUF_SIZE) /* Default maximum size for receive socket buffer.*/

/************************************************************************
*     Global Data Structures
*************************************************************************/
extern fnet_prot_if_t fnet_raw_prot_if;

/************************************************************************
*     Function Prototypes
*************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif
void fnet_raw_input(fnet_netif_t *netif, struct sockaddr *foreign_addr,  struct sockaddr *local_addr, fnet_netbuf_t *nb, fnet_netbuf_t *ip_nb);
#if defined(__cplusplus)
}
#endif

#endif  /* FNET_CFG_RAW */

#endif /* _FNET_RAW_H_ */
