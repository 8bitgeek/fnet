/**************************************************************************
* 
* Copyright 2012-2015 by Andrey Butok. FNET Community.
* Copyright 2011 by Andrey Butok and Gordon Jahn. Freescale Semiconductor, Inc.
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
* @file fnet_mpc_isr_inst.c
*
* @author Andrey Butok
*
* @date Dec-17-2012
*
* @version 0.1.1.0
*
* @brief Interrupt service dispatcher implementation.
*
***************************************************************************/
#include "fnet.h" 
#if FNET_MPC
#if !FNET_OS

#include "fnet.h"
#include "fnet_isr.h"
#include "fnet_timer.h"
#include "fnet_netbuf.h"
#include "fnet_mpc.h"

/* Vector table address. Defined in linker file.*/
extern fnet_uint32_t FNET_CFG_CPU_VECTOR_TABLE [];


/************************************************************************
* NAME: fnet_cpu_isr_install
*
* DESCRIPTION: 
*************************************************************************/
fnet_return_t fnet_cpu_isr_install(fnet_uint32_t vector_number, fnet_uint32_t priority)
{
    fnet_return_t result;
    fnet_uint32_t *irq_vec;

	irq_vec = (unsigned long *) (FNET_CFG_CPU_VECTOR_TABLE)+vector_number;
	
	if(*irq_vec != (unsigned long)fnet_cpu_isr)
    { /* It's not installed yet.*/
        *irq_vec = (unsigned long)fnet_cpu_isr;
    }
	
    if(*irq_vec == (unsigned long)fnet_cpu_isr)
    {    
    #if FNET_CFG_CPU_INDEX==0
       #if FNET_CFG_CPU_MPC5744P
            FNET_MPC_INTC_PSR(vector_number) = (unsigned short int)(0x8000 | (0x1F & priority));
      #else
            FNET_MPC_INTC_PSR(vector_number) = (unsigned char)(0xF & priority);
      #endif
    #else
            FNET_MPC_INTC_PSR(vector_number) = (unsigned char)(0xC0 | (0xF & priority));
    #endif

        result = FNET_OK;
    }
    else
        result = FNET_ERR;
        
   return result;     
}

#endif

#endif /*FNET_MPC*/
