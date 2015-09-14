/**************************************************************************
* 
* Copyright 2014 by Andrey Butok. FNET Community.
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
* @file fnet_llmnr_config.h
*
* @author Andrey Butok
*
* @brief LLMNR Server/Responder configuration file.
*
***************************************************************************/

/**************************************************************************
 * !!!DO NOT MODIFY THIS FILE!!!
 **************************************************************************/

#ifndef _FNET_LLMNR_CONFIG_H_

#define _FNET_LLMNR_CONFIG_H_

/*! @addtogroup fnet_llmnr_config */
/*! @{ */

/**************************************************************************/ /*!
 * @def      FNET_CFG_LLMNR
 * @brief    Link-Local Multicast Name Resolution (LLMNR) Server/Responder support:
 *               - @c 1 = is enabled.
 *               - @b @c 0 = is disabled (Default value).
 ******************************************************************************/
#ifndef FNET_CFG_LLMNR
    #define FNET_CFG_LLMNR                      (0)
#endif

/**************************************************************************/ /*!
 * @def     FNET_CFG_LLMNR_MAX
 * @brief   Maximum number of the LLMNR Servers that can be run simultaneously. @n
 *          Default value is @b @c 1. 
 * @showinitializer
 ******************************************************************************/
#ifndef FNET_CFG_LLMNR_MAX
    #define FNET_CFG_LLMNR_MAX                 (1u)
#endif

/**************************************************************************/ /*!
 * @def     FNET_CFG_LLMNR_PORT
 * @brief   Default LLMNR port number (in network byte order).@n
 *          The LLMNR server/responder uses this port for sending and receiving of LLMNR 
 *          messages. @n
 *          RFC 4795: Responders MUST listen on UDP port 5355.@n
 *          Default value is FNET_HTONS(5355). It is not recommended to change it.
 * @showinitializer 
 ******************************************************************************/  
#ifndef FNET_CFG_LLMNR_PORT
    #define FNET_CFG_LLMNR_PORT                 (FNET_HTONS(5355u))
#endif

/**************************************************************************/ /*!
 * @def     FNET_CFG_LLMNR_HOSTNAME_TTL
 * @brief   Default TTL value that indicates for how many seconds link-local 
 *          host name is valid for LLMNR querier.@n
 *          A default is 30 seconds (recommended by RFC4795).@n 
 *          In highly dynamic environments (such as mobile ad-hoc
 *          networks), the TTL value may need to be reduced.
 * @showinitializer 
 ******************************************************************************/  
#ifndef FNET_CFG_LLMNR_HOSTNAME_TTL
    #define FNET_CFG_LLMNR_HOSTNAME_TTL         (30)
#endif


/*! @} */

#endif /* _FNET_LLMNR_CONFIG_H_ */
