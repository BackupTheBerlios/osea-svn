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


#include "user.h"
#include "connection.h"
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <stdlib.h>
gboolean user_process_var = FALSE;


gboolean user_new_process (OseaClientSimpleData * data, gpointer usr_data)
{
	if (data->state == OSEACLIENT_OK) 
		g_print ("User created successfully\n");
	else
		g_print ("User creation failed: %s\n", data->text_response);
		
	user_process_var = TRUE;

	return TRUE;
}

gboolean user_new (gchar * line, gpointer data)
{
	gchar * user;
	gchar * password;
	gchar * password_repeated;
	gchar * description;
	gchar ** splited_string;

	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}

	// Get user name and password
	splited_string = g_strsplit (line, " ", 3);
	if (splited_string[2] == NULL) {
		user = readline ("Enter new user name: ");
	}else
		user = g_strstrip (splited_string[2]);

	password = readline ("Enter new user password: ");

	password_repeated = readline ("Reenter the password: ");
	
	if (g_strcasecmp (password, password_repeated)) {
		g_print ("Passwords didn't match.. add user failed\n");
		return FALSE;
	}

	description = readline ("Enter new user description: ");

	// Create user
	if (!aos_kernel_user_new (user, password, description, user_new_process, NULL)) {
		g_print ("Unable to create user\n");
		return FALSE;
	}

	user_process_var = FALSE;
	while (user_process_var == FALSE) {
		sleep (1);
		printf (".");
	}
	
	
	return TRUE;
}

gboolean user_remove_process (OseaClientNulData * data, gpointer usr_data)
{
	if (data->state == OSEACLIENT_OK) {
		g_print ("User removed\n");
	}else {
		g_print ("Failed to remove user: %s\n", data->text_response);
	}
	user_process_var = TRUE;
	return TRUE;
}


gboolean user_remove (gchar * line, gpointer data)
{
	gchar * user_id;
	gchar * aux_string;
	gchar ** splited_string; 
	gint    id;

	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	// Get user is to remove
	splited_string = g_strsplit (line, " ", 3);
	if (splited_string[2] == NULL) {
		user_id = readline ("Enter user id to remove (listed by 'list user' command): ");
	}else
		user_id = g_strstrip (splited_string[2]);
	
	id = strtol (user_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid user id..user del failed.\n");
		return FALSE;
	}
	
	if (!aos_kernel_user_remove (id,  user_remove_process, NULL)) {
		g_print ("Unable to remove user\n");
		return FALSE;
	}

	user_process_var = FALSE;
	while (user_process_var == FALSE) 
		sleep (1);

	return TRUE;
}


gboolean user_list_foreach_function (gpointer key, gpointer value, gpointer data) 
{
	AosKernelUser * user;
	user = (AosKernelUser *) value;

	if (user->description)
		g_print ("   %-4d %-12s %s\n", user->id, user->nick, user->description);
	else
		g_print ("   %-4d %-12s\n", user->id, user->nick);
	
	return FALSE;
}



gboolean user_list_process (OseaClientData * data, gpointer user_data) {
	
	if (data->state == OSEACLIENT_OK) {
		g_print ("\n  Number of users %d.\n", oseaclient_list_length (data->data));
		g_print ("   %-3s %-10s %s\n", "ID", "USER", "DESCRIPTION");
		
		oseaclient_list_foreach (data->data, user_list_foreach_function, user_data);		
	}else {
		g_print ("Unable to list already created users: %s\n", data->text_response);
		return FALSE;
	}

	user_process_var = TRUE;	

	return TRUE;
}

gboolean user_list   (gchar * line, gpointer data)
{
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}
	
	if (!aos_kernel_user_list (0, 0,  user_list_process, NULL)) {
		g_print ("Unable to list already created users\n");
		return FALSE;
	}
	
	user_process_var = FALSE;
	while (user_process_var == FALSE) 
		sleep (1);

	return TRUE;
}
