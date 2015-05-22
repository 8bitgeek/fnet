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
* @file fapp_dns.c
*
* @author Andrey Butok
*
* @brief FNET Shell Demo implementation (DNS Resolver).
*
***************************************************************************/

#include "fapp.h"
#include "fapp_prv.h"
#include "fapp_dns.h"
#include "fnet.h"

#if FAPP_CFG_DNS_CMD && FNET_CFG_DNS && FNET_CFG_DNS_RESOLVER
/************************************************************************
*     Definitions.
*************************************************************************/
const char FNET_DNS_RESOLUTION_FAILED[]="Resolution is FAILED";
const char FNET_DNS_UNKNOWN[]="DNS server is unknown";

/************************************************************************
*     Function Prototypes
*************************************************************************/
static void fapp_dns_handler_resolved (fnet_address_family_t addr_family, const char *addr_list, int addr_list_size, long shl_desc);
static void fapp_dns_on_ctrlc(fnet_shell_desc_t desc);

/************************************************************************
* NAME: fapp_dhcp_handler_updated
*
* DESCRIPTION: Event handler on new IP from DHCP client. 
************************************************************************/
static void fapp_dns_handler_resolved (fnet_address_family_t addr_family, const char *addr_list, int addr_list_size, long shl_desc)
{
    char                ip_str[FNET_IP_ADDR_STR_SIZE];
    fnet_shell_desc_t   desc = (fnet_shell_desc_t) shl_desc;
    int                 i;
    
    fnet_shell_unblock((fnet_shell_desc_t)shl_desc); /* Unblock the shell. */
    
    if(addr_list && addr_list_size)
    {
        for(i=0; i < addr_list_size; i++)
        {
            fnet_shell_println(desc, FAPP_SHELL_INFO_FORMAT_S, "Resolved address", 
                                fnet_inet_ntop(addr_family, addr_list, ip_str, sizeof(ip_str)) );

            if(addr_family == AF_INET)
                addr_list+=sizeof(fnet_ip4_addr_t);
            else
                addr_list+=sizeof(fnet_ip6_addr_t);
        }
    }
    else
    {
        fnet_shell_println(desc, FNET_DNS_RESOLUTION_FAILED);
    }
}

/************************************************************************
* NAME: fapp_dhcp_on_ctrlc
*
* DESCRIPTION:
************************************************************************/
static void fapp_dns_on_ctrlc(fnet_shell_desc_t desc)
{
    /* Terminate DNS service. */
    fnet_dns_release();
    fnet_shell_println( desc, FAPP_CANCELLED_STR);  
}

/************************************************************************
* NAME: fapp_dns_cmd
*
* DESCRIPTION: Start DNS client/resolver. 
************************************************************************/
void fapp_dns_cmd( fnet_shell_desc_t desc, int argc, char ** argv )
{
    
    struct fnet_dns_params      dns_params;
    fnet_netif_desc_t           netif = fapp_default_netif;
    char                        ip_str[FNET_IP_ADDR_STR_SIZE];
    int                         error_param;
    
    FNET_COMP_UNUSED_ARG(argc);
    
    /* Set DNS client/resolver parameters.*/
    fnet_memset_zero(&dns_params, sizeof(struct fnet_dns_params));
    
    /**** Define addr type to request ****/
    if (!fnet_strcmp(argv[2], "4"))
    {
        dns_params.addr_family = AF_INET;
    }
    else if (!fnet_strcmp(argv[2], "6"))
    {
        dns_params.addr_family = AF_INET6;
    }
    else
    {
        error_param = 2;
        goto ERROR_PARAMETER;
    }


    /**** Define DNS server address.****/
    if(argc == 4)
    {
        if(fnet_inet_ptos(argv[3], &dns_params.dns_server_addr) == FNET_ERR)
        {
            error_param = 3;
            goto ERROR_PARAMETER;
        }
    }
    else /* The DNS server address is not provided by user.*/
    {
    #if FNET_CFG_IP6
        /* IPv6 DNS has higher priority over IPv4.*/
        if(fnet_netif_get_ip6_dns(netif, 0U, (fnet_ip6_addr_t *)&dns_params.dns_server_addr.sa_data) == FNET_TRUE)
        {
            dns_params.dns_server_addr.sa_family = AF_INET6;
        }
        else
    #endif
    #if FNET_CFG_IP4
        if( (((struct sockaddr_in*)(&dns_params.dns_server_addr))->sin_addr.s_addr = fnet_netif_get_ip4_dns(netif)) != (fnet_ip4_addr_t)0)
        {
            dns_params.dns_server_addr.sa_family = AF_INET;
        }
        else
    #endif
        {
            fnet_shell_println(desc, FNET_DNS_UNKNOWN);
            return;    
        }
    }

    dns_params.host_name = argv[1];                 /* Host name to resolve.*/
    dns_params.handler = fapp_dns_handler_resolved; /* Callback function.*/
    dns_params.cookie = (long)desc;                 /* Application-specific parameter 
                                                       which will be passed to fapp_dns_handler_resolved().*/

    /* Run DNS cliebt/resolver. */
    if(fnet_dns_init(&dns_params) != FNET_ERR)
    {
        fnet_shell_println(desc, FAPP_DELIMITER_STR);
        fnet_shell_println(desc, FAPP_SHELL_INFO_FORMAT_S, "Resolving", dns_params.host_name);
        fnet_shell_println(desc, FAPP_SHELL_INFO_FORMAT_S, "DNS Server", 
                            fnet_inet_ntop(dns_params.dns_server_addr.sa_family, dns_params.dns_server_addr.sa_data, ip_str, sizeof(ip_str)));
        fnet_shell_println(desc, FAPP_TOCANCEL_STR);
        fnet_shell_println(desc, FAPP_DELIMITER_STR);
        
        fnet_shell_block(desc, fapp_dns_on_ctrlc); /* Block the shell input.*/
    }
    else
    {
        fnet_shell_println(desc, FAPP_INIT_ERR, "DNS");
    }
    return;
    
ERROR_PARAMETER:
    fnet_shell_println(desc, FAPP_PARAM_ERR, argv[error_param]);
    return;    
}

#endif /* FAPP_CFG_DNS_CMD && FNET_CFG_DNS && FNET_CFG_DNS_RESOLVER */














