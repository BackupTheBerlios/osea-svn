//
//  LibAfdal: common functions to liboseaclient* level and architectural functions.
//  Copyright (C) 2003  Advanced Software Production Line, S.L.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or   
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef __AFDAL_KERNEL_SESSION_H__
#define __AFDAL_KERNEL_SESSION_H__

#include "oseaclient.h"

typedef struct __AfDalSessionServer {
	gchar * name;
	gchar * host;
	gchar * port;
}AfDalSessionServer;


gboolean        oseaclient_session_login                (gchar * usr,
						    gchar * passwd,
						    gchar * kernel_hostname,
						    gchar * kernel_port,
						    AfDalNulFunc usr_function,
						    gpointer usr_data);

gboolean        oseaclient_session_logout               (AfDalNulFunc usr_function,
						    gpointer usr_data);

gboolean        oseaclient_session_refresh_key          (gchar * server,
						    AfDalNulFunc usr_function,
						    gpointer usr_data);

gboolean        oseaclient_session_refresh_session      (gchar * passwd,
						    AfDalNulFunc usr_function,
						    gpointer usr_data);

gboolean        oseaclient_session_send_afkey           (gchar * server,
						    AfDalNulFunc usr_function,
						    gpointer usr_data);

gboolean        oseaclient_session_register             (gchar * name, 
						    gint    version_number,
						    gchar * host, 
						    gchar * port,
						    gchar * kernel_hostname,
						    gchar * kernel_port,
						    AfDalNulFunc usr_function, 
						    gpointer usr_data);

gboolean        oseaclient_session_unregister           (AfDalNulFunc usr_function,
						    gpointer usr_data);

gboolean        oseaclient_session_server_exists        (gchar *server_name);

GList         * oseaclient_session_active_servers       ();

void            oseaclient_session_server_free          (AfDalSessionServer *pserv);

GList         * oseaclient_session_get_permission_list  (gchar * server_name);

void            oseaclient_session_free_permission_list (GList * permissions);

gboolean        oseaclient_session_get_permission       (gchar * server_name, gchar * permission_name);

RRConnection  * oseaclient_session_get_connection       (gchar *server_name, 
						    GError **error);

void            oseaclient_session_delete_connection    (gchar *server_name);

RRConnection  * oseaclient_session_new_connection       (gchar *server_name, 
						    GError **error);

gchar         * oseaclient_session_get_server_name      (RRConnection *connection, 
						    GError **error);



#endif
