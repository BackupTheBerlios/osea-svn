//  ASPL Fact Server Reg Connection: 
//  Copyright (C) 2002, 2003  Advanced Software Production Line, S.L.
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

#include <config.h>
#include <librr/rr.h>
#include <liboseaclient/oseaclient.h>
#include "oseaserver_reg.h"
#include <locale.h>

#define LOG_DOMAIN "oseaserver_reg"

static const gchar *server_name = NULL;


/**
 * oseaserver_reg_register:
 * @server: server name to be registered.
 * @version_number: version number of the server to be registered.
 * @usr_function: function to be executed when response arrives
 * 
 * Common satellite server function which tries to register a
 * satellite server on the kernel server.
 * Once a kernel server response arrives @usr_function is executed.
 * 
 * Return value: TRUE if registration process was OK or FALSE in other case.
 **/
gboolean oseaserver_reg_register (const gchar *server, gint version_number, OseaClientNulFunc usr_function)
{
	gchar     * hostname = NULL;
	gchar     * port = NULL;
	gchar     * kernel_server = NULL;
	gchar     * kernel_port = NULL;
	gchar     * aux_string = NULL;
		
	g_return_val_if_fail (server, FALSE);

	server_name = server;

	hostname = oseaserver_config_get (NULL, "listening hostname");
	if (! hostname) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'listening hostname' key in config file");
		return FALSE;
	}

	port = oseaserver_config_get (NULL, "listening port");
	if (! port) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'listening port' key in config file");
		return FALSE;
	}

	kernel_server = oseaserver_config_get (NULL, "kernel server");
	if (! kernel_server) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'kernel server' key in config file");
		return FALSE;
	}

	kernel_port = oseaserver_config_get (NULL, "kernel port");
	if (! kernel_port) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'kernel port' key in config file");
		return FALSE;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sending register request to kernel");

//	waiting_loop = g_main_loop_new (NULL, TRUE);

	aux_string = g_strdup (server_name);
	
	if (! oseaclient_session_register (aux_string, version_number, hostname, port, kernel_server, kernel_port, usr_function, NULL)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't register");
		g_free (aux_string);
		return FALSE;
	}

	g_free (aux_string);
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sent register request to kernel. Waiting...");

	return TRUE;
}




/**
 * oseaserver_reg_unregister:
 * @usr_function: function to be executed when response arrives
 * 
 * The opposite function to oseaserver_reg_unregister.
 * 
 * Return value: TRUE if unregister process was ok or FALSE if something fails.
 **/
gboolean oseaserver_reg_unregister (OseaClientNulFunc usr_function, gpointer user_data)
{
	return oseaclient_session_unregister (usr_function, user_data);
}


