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
* @file fnet_mk_config.h
*
* @author Andrey Butok
*
* @brief Kinetis specific default configuration.
*
***************************************************************************/

/************************************************************************
 * !!!DO NOT MODIFY THIS FILE!!!
 ************************************************************************/

#ifndef _FNET_MK_CONFIG_H_

#define _FNET_MK_CONFIG_H_

#include "fnet_user_config.h"


#ifndef FNET_MK
  #define FNET_MK   (0)
#endif

#if FNET_MK

/**************************************************************************
 *  Reduced Media Independent Interface (RMII) support.
 ******************************************************************************/
#ifndef FNET_CFG_CPU_ETH_RMII
    #define FNET_CFG_CPU_ETH_RMII        			(1)
#endif 

/**************************************************************************
 *  Default serial port number.
 ******************************************************************************/
#ifndef FNET_CFG_CPU_SERIAL_PORT_DEFAULT
    #define FNET_CFG_CPU_SERIAL_PORT_DEFAULT        (3U) /* TWR board uses the default port number 3.*/
#endif

/**************************************************************************
 *  Maximum Timer number that is avaiable on the used platform.
 ******************************************************************************/
#define  FNET_CFG_CPU_TIMER_NUMBER_MAX              (3u)


/******************************************************************************
 *  Vector number of the timer interrupt.
 *  NOTE: User application should not change this parameter. 
 ******************************************************************************/
#ifndef FNET_CFG_CPU_TIMER_VECTOR_NUMBER
    #if FNET_CFG_CPU_MK64FN1
        #define FNET_CFG_CPU_TIMER_VECTOR_NUMBER        (64U  + FNET_CFG_CPU_TIMER_NUMBER)
    #else
        #define FNET_CFG_CPU_TIMER_VECTOR_NUMBER        (84U  + FNET_CFG_CPU_TIMER_NUMBER)
    #endif
#endif

/******************************************************************************
 *  Vector number of the Ethernet Receive Frame vector number.
 *  NOTE: User application should not change this parameter. 
 ******************************************************************************/
#ifndef FNET_CFG_CPU_ETH0_VECTOR_NUMBER
    #if FNET_CFG_CPU_MK64FN1
        #define FNET_CFG_CPU_ETH0_VECTOR_NUMBER        (100U)
    #else
        #define FNET_CFG_CPU_ETH0_VECTOR_NUMBER        (93U)
    #endif
#endif

/*****************************************************************************
 *  Byte order is little endian. 
 ******************************************************************************/ 
#define FNET_CFG_CPU_LITTLE_ENDIAN                      (1)

/*****************************************************************************
 *  On-chip Flash memory start address. 
 ******************************************************************************/ 
#ifndef FNET_CFG_CPU_FLASH_ADDRESS 
    #define FNET_CFG_CPU_FLASH_ADDRESS                  (0x0U)
#endif 

/*****************************************************************************
 *   On-chip SRAM memory start address. 
 ******************************************************************************/ 
#if FNET_CFG_CPU_SRAM_SIZE
#ifndef FNET_CFG_CPU_SRAM_ADDRESS 
    #define FNET_CFG_CPU_SRAM_ADDRESS   ((fnet_uint32_t)(0x20000000U - (FNET_CFG_CPU_SRAM_SIZE/2U))) /* SRAM_L = [0x2000_0000–(SRAM_size/2)]*/
#endif
#endif


#ifndef FNET_CFG_CPU_FLASH_PROGRAM_SIZE
    #define FNET_CFG_CPU_FLASH_PROGRAM_SIZE             (4U)
#endif 

#endif /* FNET_MK */

#endif /* _FNET_MK_CONFIG_H_ */
