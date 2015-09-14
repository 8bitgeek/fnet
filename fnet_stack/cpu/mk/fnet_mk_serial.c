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
* @file fnet_mk_serial.c
*
* @author Andrey Butok
*
* @brief Kinetis Serial port I/O functions.
*
***************************************************************************/

#include "fnet_config.h"

#if FNET_MK 

#include "fnet.h"


#define FNET_MK_UART_PORT_NUMBER                (6)
static FNET_MK_UART_MemMapPtr fnet_mk_get_uart_port_ptr[FNET_MK_UART_PORT_NUMBER] =
{
    FNET_MK_UART0_BASE_PTR,
    FNET_MK_UART1_BASE_PTR,
    FNET_MK_UART2_BASE_PTR,
    FNET_MK_UART3_BASE_PTR,
    FNET_MK_UART4_BASE_PTR,
    FNET_MK_UART5_BASE_PTR
};

/********************************************************************/
void fnet_cpu_serial_putchar (fnet_index_t port_number, fnet_char_t character)
{
    FNET_MK_UART_MemMapPtr port_ptr = fnet_mk_get_uart_port_ptr[port_number];
      
    /* Wait until space is available in the FIFO */
    while((FNET_MK_UART_S1_REG(port_ptr) & FNET_MK_UART_S1_TDRE_MASK) == 0u)
    {}
    
    /* Send the character */
    FNET_MK_UART_D_REG(port_ptr) = character;
 }
/********************************************************************/
fnet_int32_t fnet_cpu_serial_getchar (fnet_index_t port_number)
{
    FNET_MK_UART_MemMapPtr port_ptr = fnet_mk_get_uart_port_ptr[port_number];
   
       /* Wait until character has been received */
    if((FNET_MK_UART_S1_REG(port_ptr) & FNET_MK_UART_S1_RDRF_MASK) != 0u)
    {
        /* Return the 8-bit data from the receiver */
        return (fnet_int32_t)FNET_MK_UART_D_REG(port_ptr);
    }
    
    return FNET_ERR;   
}

/********************************************************************/
void fnet_cpu_serial_init(fnet_index_t port_number, fnet_uint32_t baud_rate)
{
    fnet_uint32_t			sysclk; /* UART module Clock in kHz.*/
    FNET_MK_UART_MemMapPtr  uartch;
    fnet_uint16_t 			sbr;
    fnet_uint16_t 			brfa;
    fnet_uint8_t 		    temp;

  
    /* Enable the pins for the selected UART */
    /* Enable the clock to the selected UART */ 
    switch(port_number)
    {  
        case 0: /* UART0 */
		#if FNET_CFG_CPU_MK70FN1
            /* Enable the UART0_TXD function on PTF18 */
        	FNET_MK_PORTF_PCR18 = FNET_MK_PORT_PCR_MUX(0x4); /* UART is alt4 function for this pin.*/
            /* Enable the UART0_RXD function on PTF17 */
        	FNET_MK_PORTF_PCR17 = FNET_MK_PORT_PCR_MUX(0x4); /* UART is alt4 function for this pin.*/
		#elif FNET_CFG_CPU_MK64FN1
            /* Enable the UART0_TXD  */
        	FNET_MK_PORTB_PCR16 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the UART0_RXD */
        	FNET_MK_PORTB_PCR17 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
		#else /* K60 */
            /* Enable the UART0_TXD function on PTD6 */
            FNET_MK_PORTD_PCR6 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin. */
            /* Enable the UART0_RXD function on PTD7 */
            FNET_MK_PORTD_PCR7 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin. */
		#endif
            /* Enable the clock to the selected UART */
            FNET_MK_SIM_SCGC4 |= FNET_MK_SIM_SCGC4_UART0_MASK;
            break;
        case 1:
		#if FNET_CFG_CPU_MK70FN1
        	/* Enable the UART1_TXD function on PTE0 */
        	FNET_MK_PORTE_PCR0 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
        	/* Enable the UART1_RXD function on PTE1 */
        	FNET_MK_PORTE_PCR1 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
		#else /* K60 */      	
            /* Enable the UART1_TXD function on PTC4 */
            FNET_MK_PORTC_PCR4 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the UART1_RXD function on PTC3 */
            FNET_MK_PORTC_PCR3 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
        #endif
            /* Enable the clock to the selected UART */ 
            FNET_MK_SIM_SCGC4 |= FNET_MK_SIM_SCGC4_UART1_MASK;
            break;
        case 2:
		#if FNET_CFG_CPU_MK70FN1
        	/* Enable the UART2_TXD function  */
        	FNET_MK_PORTE_PCR16 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
        	/* Enable the UART2_RXD function  */
        	FNET_MK_PORTE_PCR17 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
		#else /* K60 */            
        	/* Enable the UART2_TXD function on PTD3 */
            FNET_MK_PORTD_PCR3 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the UART2_RXD function on PTD2 */
            FNET_MK_PORTD_PCR2 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
        #endif
            /* Enable the clock to the selected UART */ 
            FNET_MK_SIM_SCGC4 |= FNET_MK_SIM_SCGC4_UART2_MASK;
            break;
        case 3:
        #if FNET_CFG_CPU_MK70FN1
            /* Enable the UART3_TXD function  */
            FNET_MK_PORTB_PCR11 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the UART3_RXD function */
            FNET_MK_PORTB_PCR10 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
        #else /* others */
        	/* Enable the UART3_TXD function on PTC17 */
            FNET_MK_PORTC_PCR17 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the UART3_RXD function on PTC16 */
            FNET_MK_PORTC_PCR16 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
        #endif
            /* Enable the clock to the selected UART */ 
            FNET_MK_SIM_SCGC4 |= FNET_MK_SIM_SCGC4_UART3_MASK;
            break;
        case 4:
        #if FNET_CFG_CPU_MK64FN1
            /* Enable the UART4_TXD function on PTC14 */
            FNET_MK_PORTC_PCR14 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the UART4_RXD function on PTC15 */
            FNET_MK_PORTC_PCR15 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
        #else
            /* Enable the UART4_TXD function on PTC17 */
            FNET_MK_PORTE_PCR24 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the UART4_RXD function on PTC16 */
            FNET_MK_PORTE_PCR25 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
        #endif
            /* Enable the clock to the selected UART */ 
            FNET_MK_SIM_SCGC1 |= FNET_MK_SIM_SCGC1_UART4_MASK;
            break;
        default:
        /* case 5:*/
            /* Enable the UART3_TXD function on PTC17 */
            FNET_MK_PORTE_PCR8 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the UART3_RXD function on PTC16 */
            FNET_MK_PORTE_PCR9 = FNET_MK_PORT_PCR_MUX(0x3); /* UART is alt3 function for this pin.*/
            /* Enable the clock to the selected UART */ 
            FNET_MK_SIM_SCGC1 |= FNET_MK_SIM_SCGC1_UART5_MASK;
            break;
    }

    
    /* UART0 and UART1 are clocked from the core clock, but all other UARTs are
    * clocked from the peripheral clock. So we have to determine which clock
    * to send to the uart_init function.
    */
    if ((port_number != 0u) && (port_number != 1u))
    {
        sysclk = FNET_MK_PERIPH_CLOCK_KHZ;
    }
    else
    {
        sysclk = FNET_CPU_CLOCK_KHZ;
    }
		
    /* Initialize the UART for 8N1 operation, interrupts disabled, and
    *  no hardware flow-control.
    */
        
    /* Get UART module basse address.*/
    uartch = fnet_mk_get_uart_port_ptr[port_number];
 	                               
    /* Make sure that the transmitter and receiver are disabled while we 
    * change settings.
    */
    FNET_MK_UART_C2_REG(uartch) &= ~(FNET_MK_UART_C2_TE_MASK | FNET_MK_UART_C2_RE_MASK);
    
    /* Configure the UART for 8-bit mode, no parity */
    FNET_MK_UART_C1_REG(uartch) = 0u;	/* We need all default settings, so entire register is cleared */
        
    /* Calculate baud settings */
    sbr = (fnet_uint16_t)((sysclk*1000u)/(baud_rate * 16u));
            
    /* Save off the current value of the UARTx_BDH except for the SBR field */
    temp = FNET_MK_UART_BDH_REG(uartch) & ~(FNET_MK_UART_BDH_SBR(0x1F));
        
    FNET_MK_UART_BDH_REG(uartch) = temp |  FNET_MK_UART_BDH_SBR(((sbr & 0x1F00u) >> 8));
    FNET_MK_UART_BDL_REG(uartch) = (fnet_uint8_t)(sbr & FNET_MK_UART_BDL_SBR_MASK);
        
    /* Determine if a fractional divider is needed to get closer to the baud rate */
    brfa = (fnet_uint16_t)(((sysclk*32000u)/(baud_rate * 16u)) - ((fnet_uint32_t)sbr * 32u));
        
    /* Save off the current value of the UARTx_C4 register except for the BRFA field */
    temp = FNET_MK_UART_C4_REG(uartch) & ~(FNET_MK_UART_C4_BRFA(0x1F));
        
    FNET_MK_UART_C4_REG(uartch) = temp |  FNET_MK_UART_C4_BRFA(brfa);    
    
    /* Enable receiver and transmitter */
    FNET_MK_UART_C2_REG(uartch) |= (FNET_MK_UART_C2_TE_MASK | FNET_MK_UART_C2_RE_MASK );
 	
}

#endif /*FNET_MK*/ 
