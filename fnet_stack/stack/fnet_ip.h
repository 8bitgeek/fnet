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
* @file fnet_ip.h
*
* @author Andrey Butok
*
* @brief IP protocol API.
*
***************************************************************************/

#ifndef _FNET_IP_H_

#define _FNET_IP_H_


/*! @addtogroup fnet_socket */
/*! @{ */


/**************************************************************************/ /*!
 * @brief 32-bit IPv4 address type.
 ******************************************************************************/
typedef unsigned long fnet_ip4_addr_t; 

/**************************************************************************/ /*!
 * @def FNET_IP4_ADDR_INIT
 * @param a First octet of an IP address.
 * @param b Second octet of an IP address.
 * @param c Third  octet of an IP address.
 * @param d Fourth  octet of an IP address.
 * @hideinitializer
 * @brief Converts the standard dotted-decimal notation @c a.b.c.d 
 *        to an integer value, suitable for use as an Internet address (in network byte order).
 ******************************************************************************/
#define FNET_IP4_ADDR_INIT(a, b, c, d)   (fnet_ip4_addr_t)(FNET_NTOHL(((unsigned long)((a)&0xFFUL)<< 24U) + ((unsigned long)((b)&0xFFUL)<< 16U) + ((unsigned long)((c)&0xFFUL)<< 8U ) + (unsigned long)((d)&0xFFUL)))

/**************************************************************************/ /*!
 * @def     FNET_IP4_ADDR_STR_SIZE
 * @brief   Size of the string buffer that will contain 
 *          null-terminated ASCII string of an IPv4 address
 *          in standard "." notation.
 * @see fnet_inet_ntoa, fnet_inet_ntop
 * @showinitializer 
 ******************************************************************************/
#define FNET_IP4_ADDR_STR_SIZE       sizeof("255.255.255.255")

/************************************************************************
*    Definitions for options.
*************************************************************************/
/* The type field is divided into three internal fields:*/
#define IPOPT_COPIED(t)   ((t)&0x80U)    /* 1-bit copied flag */
#define IPOPT_CLASS (t)   ((t)&0x60U)    /* 2-bit class field */
#define IPOPT_NUMBER(t)   ((t)&0x1fU)    /* 5-bit number field */
/* Class field: */
#define IPOPT_CONTROL     (0x00U)        /* control */
#define IPOPT_RESERVED1   (0x20U)        /* reserved */
#define IPOPT_DEBMEAS     (0x40U)        /* debugging and measurement */
#define IPOPT_RESERVED2   (0x60U)        /* reserved */
/* Currently defined IP options */
#define IPOPT_EOL         (0U)           /* end of option list */
#define IPOPT_NOP         (1U)           /* no operation */

#define IPOPT_RR          (7U)           /* record  route */
#define IPOPT_TS          (68U)          /* timestamp */
#define IPOPT_SECURITY    (130U)         /* basic security */
#define IPOPT_LSRR        (131U)         /* loose source and record route */
#define IPOPT_SATID       (136U)         /* stream id */
#define IPOPT_SSRR        (137U)         /* strict source and record route */

#define IPOPT_TYPE        (0U)
#define IPOPT_LENGTH      (1U)
#define IPOPT_OFFSET      (2U)
#define IPOPT_OFFSET_MIN  (4U)           /* minimum value of 'offset'*/

/************************************************************************
*    Definitions for IP type of service.
*************************************************************************/
#define IP_TOS_NORMAL      (0x0U)
#define IP_TOS_LOWDELAY    (0x10U)
#define IP_TOS_THROUGHPUT  (0x08U)
#define IP_TOS_RELIABILITY (0x04U)

/************************************************************************
*    Timestamp option
*************************************************************************/
#define IPOPT_TS_FLAG_TSONLY     (0U)    /* Record timestamps.*/
#define IPOPT_TS_FLAG_TSANDADDR  (1U)    /* Record addresses and timestamps.*/
#define IPOPT_TS_FLAG_TSPRESPEC  (3U)    /* Record timestamps only at the prespecified systems.*/

/************************************************************************
*    Definitions of five different classes.
*************************************************************************/
#define FNET_IP4_CLASS_A(i)     (((fnet_ip4_addr_t)(i) & FNET_HTONL(0x80000000U)) == 0U)
#define FNET_IP4_CLASS_A_NET    FNET_HTONL(0xff000000U)

#define FNET_IP4_CLASS_B(i)     (((fnet_ip4_addr_t)(i) & FNET_HTONL(0xc0000000U)) == FNET_HTONL(0x80000000U))
#define FNET_IP4_CLASS_B_NET    FNET_HTONL(0xffff0000U)

#define FNET_IP4_CLASS_C(i)     (((fnet_ip4_addr_t)(i) & FNET_HTONL(0xe0000000U)) == FNET_HTONL(0xc0000000U))
#define FNET_IP4_CLASS_C_NET    FNET_HTONL(0xffffff00U)

#define FNET_IP4_CLASS_D(i)     (((fnet_ip4_addr_t)(i) & FNET_HTONL(0xf0000000U)) == FNET_HTONL(0xe0000000U))
#define FNET_IP4_CLASS_D_NET    FNET_HTONL(0xf0000000U)
/* Host groups are identified by class D IP addresses.*/
#define FNET_IP4_ADDR_IS_MULTICAST(addr)   FNET_IP4_CLASS_D(addr)

#define FNET_IP4_ADDR_IS_UNSPECIFIED(addr) ((addr) == INADDR_ANY)

#define FNET_IP4_CLASS_E(i)     (((fnet_ip4_addr_t)(i) & FNET_HTONL(0xf0000000U)) == FNET_HTONL(0xf0000000U))
#define FNET_IP4_EXPERIMENTAL(i) FNET_IP4_CLASS_E(i)
#define FNET_IP4_BADCLASS(i)     FNET_IP4_CLASS_E(i)

#define FNET_IP4_ADDR1(ipaddr)   ((unsigned char)(fnet_ntohl(ipaddr) >> 24U) & 0xffU)
#define FNET_IP4_ADDR2(ipaddr)   ((unsigned char)(fnet_ntohl(ipaddr) >> 16U) & 0xffU)
#define FNET_IP4_ADDR3(ipaddr)   ((unsigned char)(fnet_ntohl(ipaddr) >> 8U) & 0xffU)
#define FNET_IP4_ADDR4(ipaddr)   ((unsigned char)(fnet_ntohl(ipaddr)) & 0xffU)


/*! @} */

#endif
