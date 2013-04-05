/* Coldfire C Header File
 * Copyright Freescale Semiconductor Inc
 * All rights reserved.
 *
 * 2008/05/23 Revision: 0.95
 *
 * (c) Copyright UNIS, a.s. 1997-2008
 * UNIS, a.s.
 * Jundrovska 33
 * 624 00 Brno
 * Czech Republic
 * http      : www.processorexpert.com
 * mail      : info@processorexpert.com
 */

#ifndef __MCF52235_H__
#define __MCF52235_H__


/********************************************************************/
/*
 * The basic data types
 */

typedef unsigned char           uint8;   /*  8 bits */
typedef unsigned short int      uint16;  /* 16 bits */
typedef unsigned long int       uint32;  /* 32 bits */

typedef signed char             int8;    /*  8 bits */
typedef signed short int        int16;   /* 16 bits */
typedef signed long int         int32;   /* 32 bits */

typedef volatile uint8          vuint8;  /*  8 bits */
typedef volatile uint16         vuint16; /* 16 bits */
typedef volatile uint32         vuint32; /* 32 bits */

#ifdef __cplusplus
extern "C" {
#endif

#pragma define_section system ".system" far_absolute RW

/***
 * MCF52235 Derivative Memory map definitions from linker command files:
 * __IPSBAR, __RAMBAR, __RAMBAR_SIZE, __FLASHBAR, __FLASHBAR_SIZE linker
 * symbols must be defined in the linker command file.
 */

extern __declspec(system)  uint8 __IPSBAR[];
extern __declspec(system)  uint8 __RAMBAR[];
extern __declspec(system)  uint8 __RAMBAR_SIZE[];
extern __declspec(system)  uint8 __FLASHBAR[];
extern __declspec(system)  uint8 __FLASHBAR_SIZE[];

#define IPSBAR_ADDRESS   (uint32)__IPSBAR
#define RAMBAR_ADDRESS   (uint32)__RAMBAR
#define RAMBAR_SIZE      (uint32)__RAMBAR_SIZE
#define FLASHBAR_ADDRESS (uint32)__FLASHBAR
#define FLASHBAR_SIZE    (uint32)__FLASHBAR_SIZE


#include "MCF52235_SCM.h"
#include "MCF52235_DMA.h"
#include "MCF52235_UART.h"
#include "MCF52235_I2C.h"
#include "MCF52235_QSPI.h"
#include "MCF52235_RTC.h"
#include "MCF52235_DTIM.h"
#include "MCF52235_INTC.h"
#include "MCF52235_FEC.h"
#include "MCF52235_GPIO.h"
#include "MCF52235_PAD.h"
#include "MCF52235_RCM.h"
#include "MCF52235_CCM.h"
#include "MCF52235_PMM.h"
#include "MCF52235_CLOCK.h"
#include "MCF52235_EPORT.h"
#include "MCF52235_PIT.h"
#include "MCF52235_ADC.h"
#include "MCF52235_GPTA.h"
#include "MCF52235_PWM.h"
#include "MCF52235_FlexCAN.h"
#include "MCF52235_CANMB.h"
#include "MCF52235_CFM.h"
#include "MCF52235_EPHY.h"
#include "MCF52235_RNGA.h"

#ifdef __cplusplus
}
#endif


#endif /* __MCF52235_H__ */
