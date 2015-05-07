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
* @file fnet_mpc_serial.c
*
* @author Andrey Butok
*
* @date Dec-17-2012
*
* @version 0.1.1.0
*
* @brief MPC Serial port I/O functions.
*
***************************************************************************/
#include "fnet.h"

#if FNET_MPC

#include "fnet.h"
#include "fnet_cpu.h"

/********************************************************************/
void fnet_cpu_serial_putchar( long port_number, int character)
{
#if FNET_CFG_CPU_MPC5744P
    /* Send the character */
    FNET_MPC_LIN_BDRL(port_number) = (unsigned char)character;
    
    while(!(FNET_MPC_LIN_UARTSR(port_number) & 0x2))
    {};
    
    /* Clear Data Send Complete */
    FNET_MPC_LIN_UARTSR(port_number) = 0x2;
#endif
    
#if FNET_CFG_CPU_MPC5668G
	while(!(FNET_MPC_ESCI_IFSR1(port_number) & 0x8000)) 
	{};  /* Wait for previous transmissions to finish */
	
	FNET_MPC_ESCI_IFSR1(port_number) = 0x8000;				/* Clear transmission complete flag */

	FNET_MPC_ESCI_SDR(port_number) = (unsigned char)character;
#endif
	
#if FNET_CFG_CPU_MPC564xBC
	static short sent = 0;//TBD ???
    /* Wait until space is available
     */
     if (sent)
     {
	    while(!(FNET_MPC_LIN_UARTSR(port_number) & 0x2)) 
	    {};
     }
     else
     {
     	sent = 1;
     }
    
	/* Clear Data Send Complete */
    FNET_MPC_LIN_UARTSR(port_number) = 0x2;

    /* Send the character */
    FNET_MPC_LIN_BDRL(port_number) = (unsigned char)character;
#endif
}

/********************************************************************/
int fnet_cpu_serial_getchar( long port_number )
{
  
#if FNET_CFG_CPU_MPC5668G
    /* If character received. */
	if(FNET_MPC_ESCI_IFSR1(port_number) & 0x2000)
	{
        /* Clear recieved flag.*/
        FNET_MPC_ESCI_IFSR1(port_number) = 0x2000;  /* Clear flag */
	    return FNET_MPC_ESCI_SDR(port_number)&0xFF;
	}
#endif 

#if FNET_CFG_CPU_MPC564xBC || FNET_CFG_CPU_MPC5744P
    /* If character received.*/
    if (FNET_MPC_LIN_UARTSR(port_number) & 0x4)
    { 
    	/* Clear recieved flag.*/
    	FNET_MPC_LIN_UARTSR(port_number) = 0x4;
    	return (FNET_MPC_LIN_BDRM(port_number) & 0xFF);
    };
#endif 

    return FNET_ERR;    
}

/********************************************************************/
static inline void fnet_cpu_serial_gpio_init(long port_number) 
{
    /* Enable the proper UART pins */
    
#if FNET_CFG_CPU_MPC5668G
    switch (port_number)
    {
        case 1:
        	FNET_MPC_GPIO_PCR(62) = 0x600;
        	FNET_MPC_GPIO_PCR(63) = 0x600;
            break;
        case 0:
        default:
        	FNET_MPC_GPIO_PCR(60) = 0x600;
            FNET_MPC_GPIO_PCR(61) = 0x600;
            break;
    }
#endif

#if FNET_CFG_CPU_MPC5744P
    switch (port_number)
    {
        case 1:

            break;
        case 0:
        default:
               /* LINFlex 0 is connected to the MPC5746M MB
               PB[2]    MSCR[18]   TXD      LIN0 O      SRC[1:0]=00    OBE=1    ODE=0    SMC=0    APC=0    IBE=0    HYS=0    PUS=0    PUE=0    INV=0    SSS=0001
               PB[3]    MSCR[19]   RXD      LIN0 O      SRC[1:0]=00    OBE=0    ODE=0    SMC=0    APC=0    IBE=1    HYS=0    PUS=0    PUE=0    INV=0    SSS=0000
               */
               FNET_MPC5744_GPIO_MSCR(18)  = 0x02000001;    /* Set to LIN0_TXD */
               FNET_MPC5744_GPIO_MSCR(19)  = 0x00080000;    /* Set to LIN0_RXD */

               FNET_MPC5744_GPIO_IMCR(165) = 0x00000001;
            break;
    }
#endif
    

#if FNET_CFG_CPU_MPC564xBC
    switch (port_number)
    {
        case 9:
        	FNET_MPC_GPIO_PCR(130) = 0xA00;
        	FNET_MPC_GPIO_PCR(131) = 0x100;
            break;
        case 8:
        	FNET_MPC_GPIO_PCR(128) = 0xA00;
        	FNET_MPC_GPIO_PCR(129) = 0x100;
            break;
        case 7:
        	FNET_MPC_GPIO_PCR(104) = 0xA00;
        	FNET_MPC_GPIO_PCR(105) = 0x100;
            break;
        case 6:
        	FNET_MPC_GPIO_PCR(102) = 0xA00;
        	FNET_MPC_GPIO_PCR(103) = 0x100;
            break;
        case 5:
        	FNET_MPC_GPIO_PCR(92) = 0xA00;
        	FNET_MPC_GPIO_PCR(93) = 0x100;
            break;
        case 4:
        	FNET_MPC_GPIO_PCR(90) = 0xA00;
        	FNET_MPC_GPIO_PCR(91) = 0x100;
            break;
        case 3:
        	FNET_MPC_GPIO_PCR(74) = 0x600;
        	FNET_MPC_GPIO_PCR(75) = 0x100;
            break;
        case 2:
        	FNET_MPC_GPIO_PCR(40) = 0x600;
        	FNET_MPC_GPIO_PCR(41) = 0x100;
            break;
        case 1:
        	FNET_MPC_GPIO_PCR(38) = 0x600;
        	FNET_MPC_GPIO_PCR(39) = 0x100;
            break;
        case 0:
        default:
        	FNET_MPC_GPIO_PCR(18) = 0x600;
        	FNET_MPC_GPIO_PCR(19) = 0x100;
            break;
    }
#endif
}

/********************************************************************/
void fnet_cpu_serial_init(long port_number, unsigned long baud_rate)
{
    int lfdivx16;
    
	/*
	 * Initialize UART for serial communications
	 */

    /* Init GPIO.*/
    fnet_cpu_serial_gpio_init(port_number); 

#if FNET_CFG_CPU_MPC5668G

    FNET_MPC_ESCI_BRR(port_number) = (unsigned short)((FNET_CFG_CPU_CLOCK_HZ / 64 + baud_rate/2) / baud_rate);		/* Assuming peripheral clock divided by 8 */
	FNET_MPC_ESCI_CR2(port_number) = 0x2000;										/* Enable module */		
	FNET_MPC_ESCI_CR1(port_number) = 0xC;	    									/* Tx & Rx Enabled */

#endif	

#if FNET_CFG_CPU_MPC564xBC || FNET_CFG_CPU_MPC5744P
	/*
	 * Reset Transmitter - set sleep = 0 and init = 1
	 */
	FNET_MPC_LIN_CR1(port_number) = (FNET_MPC_LIN_CR1(port_number) & 0x0000FFFC) | 0x00000001;
	FNET_MPC_LIN_UARTCR(port_number) = 0x0001;	/* Turn on UART mode so settings can be... set.*/
	FNET_MPC_LIN_UARTCR(port_number) = 0x0033;

#if FNET_CFG_CPU_MPC5744P    
	/* Calculate LINIBRR and LINFBRR based on baud rate, assumes 200MHz and /2 on HALFSYS_CLK on Panther
			
	*/
	lfdivx16 = (FNET_CFG_CPU_CLOCK_HZ / 2) / baud_rate;
#else
    /* Calculate LINIBRR and LINFBRR based on baud rate, assumes 120MHz and /4 on Peripheral Set 1 on B3M
            
    */
    lfdivx16 = (FNET_CFG_CPU_CLOCK_HZ / 4) / baud_rate;   
#endif
    
	FNET_MPC_LIN_LINIBRR(port_number) = lfdivx16 / 16;	
	FNET_MPC_LIN_LINFBRR(port_number) = lfdivx16 % 16;
	
	FNET_MPC_LIN_CR1(port_number) = FNET_MPC_LIN_CR1(port_number) & 0x0000FFFE;
#endif
}

#endif /*FNET_MPC*/
