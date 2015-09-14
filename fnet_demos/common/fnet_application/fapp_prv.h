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
* @file fapp_prv.h
*
* @author Andrey Butok
*
* @brief FNET Application private definitions.
*
***************************************************************************/

#ifndef _FAPP_PRV_H_

#define _FAPP_PRV_H_

#include "fapp.h"
#include "fapp_params_prv.h"


/************************************************************************
*    Definitions.
*************************************************************************/
extern const fnet_char_t FAPP_HELP_STR[];
extern const fnet_char_t FAPP_DELIMITER_STR[];
extern const fnet_char_t FAPP_CANCELLED_STR[];
extern const fnet_char_t FAPP_TOCANCEL_STR[];
extern const fnet_char_t FAPP_COMMAND_RELEASE[]; /* Service release command */

extern const fnet_char_t FAPP_PARAM_ERR[];
/*DM extern const fnet_char_t FAPP_NET_ERR[] */
extern const fnet_char_t FAPP_INIT_ERR[];

extern const fnet_char_t FAPP_SHELL_INFO_FORMAT_S[];
extern const fnet_char_t FAPP_SHELL_INFO_FORMAT_D[];
extern const fnet_char_t FAPP_SHELL_INFO_ENABLED[];
extern const fnet_char_t FAPP_SHELL_INFO_DISABLED[];
extern const fnet_char_t FAPP_SHELL_CANCELED_CTRL_C[];

/* Boot mode structure. */
struct boot_mode
{
    fapp_params_boot_mode_t index;
    fnet_char_t                    *name;
    void(* handler)(fnet_shell_desc_t desc);
};

extern fnet_netif_desc_t fapp_default_netif;
extern const struct fnet_shell_command fapp_cmd_table [];

/************************************************************************
*     Function Prototypes
*************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

void fapp_help_cmd ( fnet_shell_desc_t desc, fnet_index_t argc, fnet_char_t ** argv );
void fapp_shell_init( fnet_shell_desc_t desc );
void fapp_netif_info_print( fnet_shell_desc_t desc, fnet_netif_desc_t netif );
void fapp_go_cmd ( fnet_shell_desc_t desc, fnet_index_t argc, fnet_char_t ** argv );
void fapp_netif_addr_print(fnet_shell_desc_t desc, fnet_address_family_t family, fnet_netif_desc_t netif, fnet_bool_t print_type);
const struct boot_mode *fapp_boot_mode_by_name (fnet_char_t *name);
const struct boot_mode *fapp_boot_mode_by_index (fapp_params_boot_mode_t index);

#if defined(__cplusplus)
}
#endif

#endif
