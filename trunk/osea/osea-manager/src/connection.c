//  ASPL Clm: Command Line Manager for ASPL Fact system
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
#include "connection.h"
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <liboseaclient/oseaclient.h>

RRConnection * connection = NULL;
gchar        * hostname   = NULL;
gchar        * port       = NULL;


gboolean connection_process_exit_var = FALSE;
gboolean connection_connected = FALSE;


gboolean __connection_ask_password (OseaClientPasswordData *pwd_data)
{
	gchar * password = NULL;

	password = getpass ("Session expired. Enter password: ");
	while (!password) {
		g_print ("Invalid password\n");
		password = getpass ("Session expired. Enter password: ");
	}

	pwd_data->notify_cb (password, pwd_data->user_data);	

	return FALSE;
}


gboolean connection_process (OseaClientNulData * data, gpointer user_data) 
{
	if (data->state == OSEACLIENT_OK) {
		g_print ("Login ok\n");
		connection_connected = TRUE;
	}else {
// 		g_print ("Login failed%s\n", data­>text_response);
		connection_connected = FALSE;
	}

	connection_process_exit_var = TRUE;

	oseaclient_password_set_callback (__connection_ask_password);

	return TRUE;
}

gboolean connection_open (gchar * server_name, gpointer data)
{
	gchar ** string_splited;
	gchar  * user, * password;
	
	// Get server name we are going to connect to
	string_splited = g_strsplit (server_name, " ", 2);

	if (string_splited[1] == NULL) {
		g_print ("Invalid host name.\n");
		return FALSE;
	}
	
	hostname = g_strstrip (string_splited[1]);

	// Get server port we are going to connect to
	string_splited = g_strsplit (hostname, ":", 2);

	if (string_splited[1] == NULL)
		port = "55000";
	else {
		hostname = g_strstrip (string_splited[0]);
		port = g_strstrip (string_splited[1]);
	}

	user = readline ("Enter login name: ");
	if (!user) {
		g_print ("Invalidad user name\n");
		return FALSE;
	}

	password = getpass ("Enter password: ");
	if (!password) {
		g_print ("Invalidad password\n");
		return FALSE;
	}

	g_print ("Connecting to %s:%s...", hostname, port);
	
	if (!oseaclient_session_login (user, password, hostname, port, connection_process, NULL)) {
		g_print ("Unable to make login\n");
		return FALSE;
	}

	connection_process_exit_var = FALSE;	
	while (connection_process_exit_var == FALSE) 
		sleep (1);

	return connection_connected; 
}

gboolean connection_get             ()
{
	return connection_connected;
}

gchar        * connection_get_server_name ()
{
	return hostname;
}

gchar        * connection_get_port        ()
{
	return port;
}
