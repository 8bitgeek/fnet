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
* @file fnet_mk_timer.c
*
* @author Andrey Butok
*
* @brief Kinetis specific SW timers implementation.
*
***************************************************************************/

#include "fnet_config.h"

#if FNET_MK 
#include "fnet.h"
#include "stack/fnet_timer_prv.h"

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

static void fnet_cpu_timer_handler_top(fnet_uint32_t cookie);

/************************************************************************
* NAME: fnet_timer_handler_top
*
* DESCRIPTION: Top interrupt handler. Increment fnet_current_time 
*              and interrupt flag. 
*************************************************************************/
static void fnet_cpu_timer_handler_top(fnet_uint32_t cookie )
{
    /* Clear the PIT timer flag. */
    FNET_MK_PIT_TFLG(FNET_CFG_CPU_TIMER_NUMBER) |= FNET_MK_PIT_TFLG_TIF_MASK;

    /* Read the load value to restart the timer. */
    (void)FNET_MK_PIT_LDVAL(FNET_CFG_CPU_TIMER_NUMBER);  
    
    /* Update RTC counter. 
     */
    fnet_timer_ticks_inc(); 
}

/************************************************************************
* NAME: fnet_cpu_timer_init
*
* DESCRIPTION: Starts TCP/IP hardware timer. delay_ms - period of timer (ms)
*         e.g. Time-out period = (1/FNET_CFG_SYSTEM_CLOCK_KHZ)x(1)x(124+1)x528x100 = 100 ms
*************************************************************************/
fnet_return_t fnet_cpu_timer_init( fnet_time_t period_ms )
{
    fnet_return_t result;
    fnet_uint32_t timeout;
    
    /* Imstall interrupt handler and enable interrupt in NVIC.
    */
    result = fnet_isr_vector_init(FNET_CFG_CPU_TIMER_VECTOR_NUMBER, fnet_cpu_timer_handler_top,
                                              fnet_timer_handler_bottom, FNET_CFG_CPU_TIMER_VECTOR_PRIORITY, 0u);
    if(result == FNET_OK)
    {  
        /* Initialize the PIT timer to generate an interrupt every period_ms */
    
        /* Enable the clock to the PIT module. Clock for PIT Timers to be enabled */
        FNET_MK_SIM_SCGC6 |= FNET_MK_SIM_SCGC6_PIT_MASK;
    
        /* Enable the PIT timer module. */
        FNET_MK_PIT_MCR &= ~FNET_MK_PIT_MCR_MDIS_MASK;
            
        /* Calculate the timeout value. */
        timeout = period_ms * FNET_MK_PERIPH_CLOCK_KHZ;
        FNET_MK_PIT_LDVAL(FNET_CFG_CPU_TIMER_NUMBER) = timeout;
    
        /* Enable the timer and enable interrupts */
        FNET_MK_PIT_TCTRL(FNET_CFG_CPU_TIMER_NUMBER) |= FNET_MK_PIT_TCTRL_TEN_MASK | FNET_MK_PIT_TCTRL_TIE_MASK;
    }
    
    return result;
}

/************************************************************************
* NAME: fnet_cpu_timer_release
*
* DESCRIPTION: Relaeses TCP/IP hardware timer.
*              
*************************************************************************/
void fnet_cpu_timer_release( void )
{
    /* Disable the timer and disable interrupts */
    FNET_MK_PIT_TCTRL(FNET_CFG_CPU_TIMER_NUMBER) &= ~(FNET_MK_PIT_TCTRL_TEN_MASK | FNET_MK_PIT_TCTRL_TIE_MASK);
  
    /* Uninstall interrupt handler.
     */
    fnet_isr_vector_release(FNET_CFG_CPU_TIMER_VECTOR_NUMBER);
}



#endif /*FNET_MK*/
