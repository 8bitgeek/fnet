/**********************************************************************/ /*!
*
* @file fapp_user_config.h
*
* @brief FNET Application User configuration file.
* It should be used to change any default configuration parameter of FAPP.
*
***************************************************************************/

#ifndef _FAPP_USER_CONFIG_H_

#define _FAPP_USER_CONFIG_H_

#define FAPP_CFG_NAME                   "FNET Shell Application" 
#define FAPP_CFG_SHELL_PROMPT           "SHELL> " 

/*  "dhcp" command.*/
#define FAPP_CFG_DHCP_CMD               (1)
#define FAPP_CFG_DHCP_CMD_DISCOVER_MAX  (5)

/*  "set/get" command.*/
#define FAPP_CFG_SETGET_CMD_IP          (1)
#define FAPP_CFG_SETGET_CMD_GATEWAY     (1)
#define FAPP_CFG_SETGET_CMD_NETMASK     (1)
#define FAPP_CFG_SETGET_CMD_MAC         (1)
#define FAPP_CFG_SETGET_CMD_HOSTNAME    (1)

/*  "info" command. */
#define FAPP_CFG_INFO_CMD               (1)

/*  "http" command.*/
#define FAPP_CFG_HTTP_CMD               (1)

/*  "exp" command.*/
#define FAPP_CFG_EXP_CMD                (1)

/*  "save" command.*/
#define FAPP_CFG_SAVE_CMD               (1)

/*  "reset" command.*/
#define FAPP_CFG_RESET_CMD              (1)

/*  "telnet" command.*/
#define FAPP_CFG_TELNET_CMD             (1)

/*  "dns" command.*/
#define FAPP_CFG_DNS_CMD                (1)

/*  "ping" command.*/
#define FAPP_CFG_PING_CMD               (1) 

/*  "bind" command.*/
#define FAPP_CFG_BIND_CMD               (1) 

/*  "unbind" command.*/
#define FAPP_CFG_UNBIND_CMD             (1)

/*  "llmnr" command.*/
#define FAPP_CFG_LLMNR_CMD              (1)

/* Reading of the configuration parameters from the Flash 
 * memory during the application bootup.*/
#define FAPP_CFG_PARAMS_READ_FLASH      (1)

/* Rewriting of the configuration parameters in the Flash 
 * memory duiring flashing of the application. */
#define FAPP_CFG_PARAMS_REWRITE_FLASH   (1)

#if 1 /* To run servers on startup set to 1. */
    #define FAPP_CFG_STARTUP_SCRIPT_ENABLED	(1)
    #define FAPP_CFG_STARTUP_SCRIPT "llmnr"     /* For example "http; telnet" */
#endif

#endif

