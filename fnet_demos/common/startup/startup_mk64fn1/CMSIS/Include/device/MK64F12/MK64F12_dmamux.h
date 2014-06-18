/*
** ###################################################################
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    K64P144M120SF5RM, Rev.2, January 2014
**     Version:             rev. 2.5, 2014-02-10
**     Build:               b140526
**
**     Abstract:
**         Extension to the CMSIS register access layer header.
**
**     Copyright: 2014 Freescale Semiconductor, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 1.0 (2013-08-12)
**         Initial version.
**     - rev. 2.0 (2013-10-29)
**         Register accessor macros added to the memory map.
**         Symbols for Processor Expert memory map compatibility added to the memory map.
**         Startup file for gcc has been updated according to CMSIS 3.2.
**         System initialization updated.
**         MCG - registers updated.
**         PORTA, PORTB, PORTC, PORTE - registers for digital filter removed.
**     - rev. 2.1 (2013-10-30)
**         Definition of BITBAND macros updated to support peripherals with 32-bit acces disabled.
**     - rev. 2.2 (2013-12-09)
**         DMA - EARS register removed.
**         AIPS0, AIPS1 - MPRA register updated.
**     - rev. 2.3 (2014-01-24)
**         Update according to reference manual rev. 2
**         ENET, MCG, MCM, SIM, USB - registers updated
**     - rev. 2.4 (2014-02-10)
**         The declaration of clock configurations has been moved to separate header file system_MK64F12.h
**         Update of SystemInit() and SystemCoreClockUpdate() functions.
**     - rev. 2.5 (2014-02-10)
**         The declaration of clock configurations has been moved to separate header file system_MK64F12.h
**         Update of SystemInit() and SystemCoreClockUpdate() functions.
**         Module access macro module_BASES replaced by module_BASE_PTRS.
**
** ###################################################################
*/

/*
 * WARNING! DO NOT EDIT THIS FILE DIRECTLY!
 *
 * This file was generated automatically and any changes may be lost.
 */
#ifndef __HW_DMAMUX_REGISTERS_H__
#define __HW_DMAMUX_REGISTERS_H__

#include "MK64F12.h"
#include "fsl_bitaccess.h"

/*
 * MK64F12 DMAMUX
 *
 * DMA channel multiplexor
 *
 * Registers defined in this header file:
 * - HW_DMAMUX_CHCFGn - Channel Configuration register
 *
 * - hw_dmamux_t - Struct containing all module registers.
 */

#define HW_DMAMUX_INSTANCE_COUNT (1U) /*!< Number of instances of the DMAMUX module. */

/*******************************************************************************
 * HW_DMAMUX_CHCFGn - Channel Configuration register
 ******************************************************************************/

/*!
 * @brief HW_DMAMUX_CHCFGn - Channel Configuration register (RW)
 *
 * Reset value: 0x00U
 *
 * Each of the DMA channels can be independently enabled/disabled and associated
 * with one of the DMA slots (peripheral slots or always-on slots) in the
 * system. Setting multiple CHCFG registers with the same source value will result in
 * unpredictable behavior. This is true, even if a channel is disabled (ENBL==0).
 * Before changing the trigger or source settings, a DMA channel must be disabled
 * via CHCFGn[ENBL].
 */
typedef union _hw_dmamux_chcfgn
{
    uint8_t U;
    struct _hw_dmamux_chcfgn_bitfields
    {
        uint8_t SOURCE : 6;            /*!< [5:0] DMA Channel Source (Slot) */
        uint8_t TRIG : 1;              /*!< [6] DMA Channel Trigger Enable */
        uint8_t ENBL : 1;              /*!< [7] DMA Channel Enable */
    } B;
} hw_dmamux_chcfgn_t;

/*!
 * @name Constants and macros for entire DMAMUX_CHCFGn register
 */
/*@{*/
#define HW_DMAMUX_CHCFGn_COUNT (16U)

#define HW_DMAMUX_CHCFGn_ADDR(x, n) ((x) + 0x0U + (0x1U * (n)))

#define HW_DMAMUX_CHCFGn(x, n)   (*(__IO hw_dmamux_chcfgn_t *) HW_DMAMUX_CHCFGn_ADDR(x, n))
#define HW_DMAMUX_CHCFGn_RD(x, n) (HW_DMAMUX_CHCFGn(x, n).U)
#define HW_DMAMUX_CHCFGn_WR(x, n, v) (HW_DMAMUX_CHCFGn(x, n).U = (v))
#define HW_DMAMUX_CHCFGn_SET(x, n, v) (HW_DMAMUX_CHCFGn_WR(x, n, HW_DMAMUX_CHCFGn_RD(x, n) |  (v)))
#define HW_DMAMUX_CHCFGn_CLR(x, n, v) (HW_DMAMUX_CHCFGn_WR(x, n, HW_DMAMUX_CHCFGn_RD(x, n) & ~(v)))
#define HW_DMAMUX_CHCFGn_TOG(x, n, v) (HW_DMAMUX_CHCFGn_WR(x, n, HW_DMAMUX_CHCFGn_RD(x, n) ^  (v)))
/*@}*/

/*
 * Constants & macros for individual DMAMUX_CHCFGn bitfields
 */

/*!
 * @name Register DMAMUX_CHCFGn, field SOURCE[5:0] (RW)
 *
 * Specifies which DMA source, if any, is routed to a particular DMA channel.
 * See your device's chip configuration details for information about the
 * peripherals and their slot numbers.
 */
/*@{*/
#define BP_DMAMUX_CHCFGn_SOURCE (0U)       /*!< Bit position for DMAMUX_CHCFGn_SOURCE. */
#define BM_DMAMUX_CHCFGn_SOURCE (0x3FU)    /*!< Bit mask for DMAMUX_CHCFGn_SOURCE. */
#define BS_DMAMUX_CHCFGn_SOURCE (6U)       /*!< Bit field size in bits for DMAMUX_CHCFGn_SOURCE. */

/*! @brief Read current value of the DMAMUX_CHCFGn_SOURCE field. */
#define BR_DMAMUX_CHCFGn_SOURCE(x, n) (HW_DMAMUX_CHCFGn(x, n).B.SOURCE)

/*! @brief Format value for bitfield DMAMUX_CHCFGn_SOURCE. */
#define BF_DMAMUX_CHCFGn_SOURCE(v) ((uint8_t)((uint8_t)(v) << BP_DMAMUX_CHCFGn_SOURCE) & BM_DMAMUX_CHCFGn_SOURCE)

/*! @brief Set the SOURCE field to a new value. */
#define BW_DMAMUX_CHCFGn_SOURCE(x, n, v) (HW_DMAMUX_CHCFGn_WR(x, n, (HW_DMAMUX_CHCFGn_RD(x, n) & ~BM_DMAMUX_CHCFGn_SOURCE) | BF_DMAMUX_CHCFGn_SOURCE(v)))
/*@}*/

/*!
 * @name Register DMAMUX_CHCFGn, field TRIG[6] (RW)
 *
 * Enables the periodic trigger capability for the triggered DMA channel.
 *
 * Values:
 * - 0 - Triggering is disabled. If triggering is disabled and ENBL is set, the
 *     DMA Channel will simply route the specified source to the DMA channel.
 *     (Normal mode)
 * - 1 - Triggering is enabled. If triggering is enabled and ENBL is set, the
 *     DMAMUX is in Periodic Trigger mode.
 */
/*@{*/
#define BP_DMAMUX_CHCFGn_TRIG (6U)         /*!< Bit position for DMAMUX_CHCFGn_TRIG. */
#define BM_DMAMUX_CHCFGn_TRIG (0x40U)      /*!< Bit mask for DMAMUX_CHCFGn_TRIG. */
#define BS_DMAMUX_CHCFGn_TRIG (1U)         /*!< Bit field size in bits for DMAMUX_CHCFGn_TRIG. */

/*! @brief Read current value of the DMAMUX_CHCFGn_TRIG field. */
#define BR_DMAMUX_CHCFGn_TRIG(x, n) (BITBAND_ACCESS8(HW_DMAMUX_CHCFGn_ADDR(x, n), BP_DMAMUX_CHCFGn_TRIG))

/*! @brief Format value for bitfield DMAMUX_CHCFGn_TRIG. */
#define BF_DMAMUX_CHCFGn_TRIG(v) ((uint8_t)((uint8_t)(v) << BP_DMAMUX_CHCFGn_TRIG) & BM_DMAMUX_CHCFGn_TRIG)

/*! @brief Set the TRIG field to a new value. */
#define BW_DMAMUX_CHCFGn_TRIG(x, n, v) (BITBAND_ACCESS8(HW_DMAMUX_CHCFGn_ADDR(x, n), BP_DMAMUX_CHCFGn_TRIG) = (v))
/*@}*/

/*!
 * @name Register DMAMUX_CHCFGn, field ENBL[7] (RW)
 *
 * Enables the DMA channel.
 *
 * Values:
 * - 0 - DMA channel is disabled. This mode is primarily used during
 *     configuration of the DMAMux. The DMA has separate channel enables/disables, which
 *     should be used to disable or reconfigure a DMA channel.
 * - 1 - DMA channel is enabled
 */
/*@{*/
#define BP_DMAMUX_CHCFGn_ENBL (7U)         /*!< Bit position for DMAMUX_CHCFGn_ENBL. */
#define BM_DMAMUX_CHCFGn_ENBL (0x80U)      /*!< Bit mask for DMAMUX_CHCFGn_ENBL. */
#define BS_DMAMUX_CHCFGn_ENBL (1U)         /*!< Bit field size in bits for DMAMUX_CHCFGn_ENBL. */

/*! @brief Read current value of the DMAMUX_CHCFGn_ENBL field. */
#define BR_DMAMUX_CHCFGn_ENBL(x, n) (BITBAND_ACCESS8(HW_DMAMUX_CHCFGn_ADDR(x, n), BP_DMAMUX_CHCFGn_ENBL))

/*! @brief Format value for bitfield DMAMUX_CHCFGn_ENBL. */
#define BF_DMAMUX_CHCFGn_ENBL(v) ((uint8_t)((uint8_t)(v) << BP_DMAMUX_CHCFGn_ENBL) & BM_DMAMUX_CHCFGn_ENBL)

/*! @brief Set the ENBL field to a new value. */
#define BW_DMAMUX_CHCFGn_ENBL(x, n, v) (BITBAND_ACCESS8(HW_DMAMUX_CHCFGn_ADDR(x, n), BP_DMAMUX_CHCFGn_ENBL) = (v))
/*@}*/

/*******************************************************************************
 * hw_dmamux_t - module struct
 ******************************************************************************/
/*!
 * @brief All DMAMUX module registers.
 */
#pragma pack(1)
typedef struct _hw_dmamux
{
    __IO hw_dmamux_chcfgn_t CHCFGn[16];    /*!< [0x0] Channel Configuration register */
} hw_dmamux_t;
#pragma pack()

/*! @brief Macro to access all DMAMUX registers. */
/*! @param x DMAMUX module instance base address. */
/*! @return Reference (not a pointer) to the registers struct. To get a pointer to the struct,
 *     use the '&' operator, like <code>&HW_DMAMUX(DMAMUX_BASE)</code>. */
#define HW_DMAMUX(x)   (*(hw_dmamux_t *)(x))

#endif /* __HW_DMAMUX_REGISTERS_H__ */
/* EOF */
