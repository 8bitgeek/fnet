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
**********************************************************************/
/*!
*
* @file fnet_shell.h
*
* @author Andrey Butok
*
* @brief FNET Shell service API.
*
***************************************************************************/
#ifndef _FNET_SSL_H_

#define _FNET_SSL_H_

#include "fnet_config.h"

#if FNET_CFG_SSL /* TBD under development */
 
#include "fnet.h"

/**************************************************************************/ /*!
 * @brief SSL session descriptor.
 * @see fnet_ssl_session_init()
 ******************************************************************************/
typedef long fnet_ssl_session_desc_t;

/**************************************************************************/ /*!
 * @brief SSL Socket descriptor.
 ******************************************************************************/
typedef long SSL_SOCKET;

/**************************************************************************/ /*!
 * @brief    SSL Session types
 ******************************************************************************/
typedef enum
{
    FNET_SSL_SESSION_ROLE_SERVER            = 1,     /**< @brief SSL Server */
    FNET_SSL_SESSION_ROLE_CLIENT            = 2      /**< @brief SSL Client */
} fnet_ssl_session_role_t;

/**************************************************************************/ /*!
 * @brief Input parameters structure for @ref fnet_ssl_session_init()
 ******************************************************************************/
struct fnet_ssl_session_params
{
    fnet_ssl_session_role_t session_role;           /**< @brief SSL Session type. Client or server.*/
    char                    *cert_file_path;        /**< @brief Certificate file path (null-terminated string). It is optional. */
    char                    *priv_key_file_path;    /**< @brief Private Key file path (null-terminated string). It is optional. */
    char                    *ca_cert_file_path;     /**< @brief CA (Certificate Authority) certificate file path (null-terminated string). It is optional. */
};
 

fnet_ssl_session_desc_t fnet_ssl_session_init(struct fnet_ssl_session_params *params);
void fnet_ssl_session_release(fnet_ssl_session_desc_t ssl_desc);

SSL_SOCKET fnet_ssl_socket(fnet_ssl_session_desc_t session_desc, SOCKET sock);
int fnet_ssl_close(SSL_SOCKET ssl_sock);
int fnet_ssl_recv(SSL_SOCKET ssl_sock, char *buf, int len, int flags);
int fnet_ssl_send(SSL_SOCKET ssl_sock, char *buf, int len, int flags);

#endif /* FNET_CFG_SSL */

#endif /* _FNET_SSL_H_ */
