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
* @file fapp_bench.h
*
* @author Andrey Butok
*
* @brief FNET Shell Demo API.
*
***************************************************************************/

#ifndef _FAPP_BENCH_H_

#define _FAPP_BENCH_H_

#include "fapp.h"

#if FAPP_CFG_BENCH_CMD

/************************************************************************
* Benchmark definitions
************************************************************************/
#define FAPP_BENCH_PORT                         (FNET_HTONS(7007))      /* Port used by the benchmark application (in network byte order).*/
#define FAPP_BENCH_PACKET_SIZE_MAX              (8*1024)    /* Defines size of Applacation and Socket TX&RX buffers.*/
#define FAPP_BENCH_TX_PACKET_SIZE_DEFAULT       (1472)
#define FAPP_BENCH_TX_PACKET_NUMBER_DEFAULT     (10000)
#define FAPP_BENCH_TX_ITERATION_NUMBER_DEFAULT  (1)
#define FAPP_BENCH_TX_ITERATION_NUMBER_MAX      (10000)

#if defined(__cplusplus)
extern "C" {
#endif

void fapp_benchrx_cmd( fnet_shell_desc_t desc, fnet_index_t argc, fnet_char_t ** argv );
void fapp_benchtx_cmd( fnet_shell_desc_t desc, fnet_index_t argc, fnet_char_t ** argv );

#if defined(__cplusplus)
}
#endif

#endif


#endif
