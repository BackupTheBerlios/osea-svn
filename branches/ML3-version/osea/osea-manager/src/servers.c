//  ASPL Clm: Command Line Manager for ASPL Fact system
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//  Copyright (C) 2004 David Marín Carreño
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


#include "servers.h"
#include "connection.h"
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <stdlib.h>
gboolean servers_process_var = FALSE;


gboolean servers_remove_process (OseaClientNulData * data, gpointer usr_data)
{
	if (data->state == OSEACLIENT_OK) {
		g_print ("Server removed\n");
	}else {
		g_print ("Failed to remove server: %s\n", data->text_response);
	}
	servers_process_var = TRUE;
	return TRUE;
}


gboolean servers_remove (gchar * line, gpointer data)
{
	gchar  * servers_id;
	gchar  * aux_string;
	gchar ** splited_string; 
	gint     id;

	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	// Get servers is to remove
	splited_string = g_strsplit (line, " ", 3);
	if (splited_string[2] == NULL) {
		servers_id = readline ("Enter server id to remove (listed by 'list server' command): ");
	}else
		servers_id = g_strstrip (splited_string[2]);
	
	id = strtol (servers_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid server id..del server del failed.\n");
		return FALSE;
	}
	
	if (!aos_kernel_server_remove (id,  servers_remove_process, NULL)) {
		g_print ("Unable to remove servers\n");
		return FALSE;
	}

	servers_process_var = FALSE;
	while (servers_process_var == FALSE) 
		sleep (1);

	return TRUE;
}


gboolean servers_list_foreach_function (gpointer key, gpointer value, gpointer data) 
{
	AosKernelServer * server;

	server = (AosKernelServer *) value;

	if (server->description)
		g_print ("   %-4d %-12s %-7d %s\n", server->id, server->name, server->version, server->description);
	else
		g_print ("   %-4d %-12s %-7d\n", server->id, server->name, server->version);
	
	return FALSE;
}



gboolean servers_list_process (OseaClientData * data, gpointer servers_data) {
	
	if (data->state == OSEACLIENT_OK) {
		g_print ("\n  Number of servers %d.\n", oseaclient_list_length (data->data));
		g_print ("   %-3s %-10s %-2s %s\n", "ID", "SERVER", "VERSION", "DESCRIPTION");
		
		oseaclient_list_foreach (data->data, servers_list_foreach_function, servers_data);		
	}else {
		g_print ("Unable to list already created servers: %s\n", data->text_response);
	}

	servers_process_var = TRUE;	

	return TRUE;
}

gboolean servers_list   (gchar * line, gpointer data)
{
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}
	
	if (!aos_kernel_server_list (0, 0,  servers_list_process, NULL)) {
		g_print ("Unable to list already created servers\n");
		return FALSE;
	}
	
	servers_process_var = FALSE;
	while (servers_process_var == FALSE) 
		sleep (1);

	return TRUE;
}
