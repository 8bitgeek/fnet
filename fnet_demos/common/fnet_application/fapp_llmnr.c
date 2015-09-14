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
* @file fapp_llmnr.c
*
* @author Andrey Butok
*
* @brief FNET Shell Demo (HTTP Server Shell interface).
*
***************************************************************************/

#include "fapp.h"

#if FAPP_CFG_LLMNR_CMD && FNET_CFG_LLMNR

#include "fapp_prv.h"
#include "fapp_llmnr.h"

static fnet_llmnr_desc_t fapp_llmnr_desc = 0; /* LLMNR service descriptor. */

/************************************************************************
* NAME: fapp_llmnr_release
*
* DESCRIPTION: Releases HTTP server.
*************************************************************************/
void fapp_llmnr_release(void)
{
    fnet_llmnr_release(fapp_llmnr_desc);
    fapp_llmnr_desc = 0;    
}

/************************************************************************
* NAME: fapp_llmnr_cmd
*
* DESCRIPTION: Run LLMNR server.
*************************************************************************/
void fapp_llmnr_cmd( fnet_shell_desc_t desc, fnet_index_t argc, fnet_char_t ** argv )
{
    struct fnet_llmnr_params    params;
    fnet_llmnr_desc_t           llmnr_desc;

    if(argc == 1u) /* By default is "init".*/
    {
        /* Init parameters.*/
        fnet_memset_zero(&params, sizeof(params));
        params.netif_desc = fapp_default_netif;   
        params.host_name = fapp_params_host_name;

        /* Start LLMNR server */
        llmnr_desc = fnet_llmnr_init(&params);
        if(llmnr_desc != FNET_ERR)
        {
            fnet_shell_println(desc, FAPP_DELIMITER_STR);
            fnet_shell_println(desc, " LLMNR server started.");
            fnet_shell_println(desc, FAPP_SHELL_INFO_FORMAT_S, "Host Name", params.host_name ); 
            fnet_shell_println(desc, FAPP_DELIMITER_STR);
            
            fapp_llmnr_desc = llmnr_desc;
        }
        else
        {
            fnet_shell_println(desc, FAPP_INIT_ERR, "LLMNR");
        }
    }
    else if((argc == 2u) && (fnet_strcasecmp(&FAPP_COMMAND_RELEASE[0], argv[1]) == 0)) /* [release] */
    {
        fapp_llmnr_release();
    }
    else
    {
        fnet_shell_println(desc, FAPP_PARAM_ERR, argv[1]);
    }
}

/************************************************************************
* NAME: fapp_llmnr_info
*
* DESCRIPTION:
*************************************************************************/
void fapp_llmnr_info(fnet_shell_desc_t desc)
{
    fnet_shell_println(desc, FAPP_SHELL_INFO_FORMAT_S, "LLMNR Server", 
                        fnet_llmnr_enabled(fapp_llmnr_desc) ? FAPP_SHELL_INFO_ENABLED : FAPP_SHELL_INFO_DISABLED);
}


#endif /* FAPP_CFG_HTTP_CMD */
