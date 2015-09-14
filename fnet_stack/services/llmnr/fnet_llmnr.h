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
* @file fnet_llmnr.h
*
* @author Andrey Butok
*
* @brief LLMNR Server/Responder API.
*
***************************************************************************/

#ifndef _FNET_LLMNR_H_

#define _FNET_LLMNR_H_

#include "fnet.h"


#if FNET_CFG_LLMNR || defined(__DOXYGEN__)



/*! @addtogroup fnet_llmnr 
*
* Link-Local Multicast Name Resolution (LLMNR) is used to enable
* name resolution in scenarios in which conventional DNS name resolution is not possible.
* It allows both IPv4 and IPv6 hosts to perform name resolution for hosts on the same local link.@n
* The protocol specification is defined by RFC4795.@n
* It is natively supported by Windows OSs.@n
* @n
* After the LLMNR server is initialized by calling the @ref fnet_llmnr_init() function,
* the user application should call the main service-polling function  
* @ref fnet_poll_services() periodically in background. @n
* @n
* For the LLMNR-server service example, refer to the FNET Shell demo source code.@n
* 
* Configuration parameters:
* - @ref FNET_CFG_LLMNR 
* - @ref FNET_CFG_LLMNR_MAX
* - @ref FNET_CFG_LLMNR_PORT  
* - @ref FNET_CFG_LLMNR_HOSTNAME_TTL
*  
*/

/*! @{ */


/**************************************************************************/ /*!
 * @brief Initialization parameters for the @ref fnet_llmnr_init() function.
 ******************************************************************************/
struct fnet_llmnr_params
{
    fnet_netif_desc_t           netif_desc;     /**< @brief Network interface descriptor to be used by the LLMNR server.*/
    const fnet_char_t                  *host_name;     /**< @brief Link-local host name advertised by LLMNR server. */
    fnet_uint32_t                 host_name_ttl;  /**< @brief TTL value that indicates for how many seconds link-local host name is valid for LLMNR querier, in seconds (it is optional).@n
                                                 * Default value is defined by @ref FNET_CFG_LLMNR_HOSTNAME_TTL. */
    fnet_address_family_t       addr_family;    /**< @brief Address family (IPv6 or IPv4 or both) the server will listen for LLMNR query (it is optional).@n
                                                   Default value is defined by @ref AF_SUPPORTED.*/
};

/**************************************************************************/ /*!
 * @brief LLMNR server descriptor.
 * @see fnet_llmnr_init()
 ******************************************************************************/
typedef fnet_int32_t fnet_llmnr_desc_t;

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************************************************/ /*!
 *
 * @brief    Initializes Link-Local Multicast Name Resolution (LLMNR) server/responder service.
 *
 * @param params     Initialization parameters defined by @ref fnet_llmnr_params.
 *
 * @return This function returns:
 *   - LLMNR server descriptor if no error occurs.
 *   - @ref FNET_ERR if an error occurs.
 *
 * @see fnet_llmnr_release(), fnet_llmnr_params
 *
 ******************************************************************************
 *
 * This function initializes the Link-Local Multicast Name Resolution (LLMNR) 
 * server/responder service.@n
 * It allocates all needed resources and registers the LLMNR service in the polling list.@n
 * After the initialization, the user application should call the main polling 
 * function @ref fnet_poll_services() periodically to run the LLMNR service routine 
 * in the background.
 *
 ******************************************************************************/
fnet_llmnr_desc_t fnet_llmnr_init( struct fnet_llmnr_params *params );

/***************************************************************************/ /*!
 *
 * @brief    Releases the Link-Local Multicast Name Resolution (LLMNR) server/responder service.
 *
 * @param desc     LLMNR server descriptor to be unregistered.
 *
 * @see fnet_llmnr_init()
 *
 ******************************************************************************
 *
 * This function releases the LLMNR Server assigned to the @c desc 
 * descriptor.@n 
 * It releases all occupied resources, and unregisters the LLMNR service from 
 * the polling list.
 *
 ******************************************************************************/
void fnet_llmnr_release(fnet_llmnr_desc_t desc);

/***************************************************************************/ /*!
 *
 * @brief    Detects if the LLMNR Server service is enabled or disabled.
 *
 * @param desc     LLMNR server descriptor
 *
 * @return This function returns:
 *          - @ref FNET_TRUE if the LLMNR Server is successfully initialized.
 *          - @ref FNET_FALSE if the LLMNR Server is not initialized or is released.
 *
 ******************************************************************************
 *
 * This function detects if the LLMNR Server service is initialized or is released.
 *
 ******************************************************************************/
fnet_bool_t fnet_llmnr_enabled(fnet_llmnr_desc_t desc);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* FNET_CFG_LLMNR */

#endif /* _FNET_LLMNR_H_ */
