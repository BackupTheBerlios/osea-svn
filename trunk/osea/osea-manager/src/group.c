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


#include "group.h"
#include "connection.h"
#include "user.h"
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <stdlib.h>
gboolean group_process_var = FALSE;

gboolean group_new_process (OseaClientSimpleData * data, gpointer usr_data)
{
	if (data->state == OSEACLIENT_OK) 
		g_print ("Group created successfully\n");
	else
		g_print ("Group creation failed: %s\n", data->text_response);
		
	
	group_process_var = TRUE;

	return TRUE;
}

gboolean group_new (gchar * line, gpointer data)
{
	gchar * group;
	gchar * description;
	gchar ** splited_string;

	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}

	// Get group name and password
	splited_string = g_strsplit (line, " ", 3);
	if (splited_string[2] == NULL) {
		group = readline ("Enter new group name: ");
	}else
		group = g_strstrip (splited_string[2]);

	description = readline ("Enter new group description: ");

	// Create group
	if (!aos_kernel_group_new (group, description, group_new_process, NULL)) {
		g_print ("Unable to create group\n");
		return FALSE;
	}

	group_process_var = FALSE;
	while (group_process_var == FALSE) {
		sleep (1);
		printf (".");
	}
	
	
	return TRUE;
}

gboolean group_remove_process (OseaClientNulData * data, gpointer usr_data)
{
	if (data->state == OSEACLIENT_OK) {
		g_print ("Group removed\n");
	}else {
		g_print ("Failed to remove group: %s\n", data->text_response);
	}
	group_process_var = TRUE;
	return TRUE;
}


gboolean group_remove (gchar * line, gpointer data)
{
	gchar * group_id;
	gchar * aux_string;
	gchar ** splited_string; 
	gint    id;

	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	// Get group is to remove
	splited_string = g_strsplit (line, " ", 3);
	if (splited_string[2] == NULL) {
		group_id = readline ("Enter group id to remove (listed by 'list group' command): ");
	}else
		group_id = g_strstrip (splited_string[2]);
	
	id = strtol (group_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid group id..group del failed.\n");
		return FALSE;
	}
	
	if (!aos_kernel_group_remove (id,  group_remove_process, NULL)) {
		g_print ("Unable to remove group\n");
		return FALSE;
	}

	group_process_var = FALSE;
	while (group_process_var == FALSE) 
		sleep (1);

	return TRUE;
}


gboolean group_list_foreach_function (gpointer key, gpointer value, gpointer data) 
{
	AosKernelGroup * group;
	group = (AosKernelGroup *) value;

	if (group->description)
		g_print ("   %-4d %-12s %s\n", group->id, group->name,  group->description);
	else
		g_print ("   %-4d %-12s\n", group->id, group->name );
	
	return FALSE;
}



gboolean group_list_process (OseaClientData * data, gpointer group_data) {
	
	if (data->state == OSEACLIENT_OK) {
		g_print ("\n  Number of groups %d.\n", oseaclient_list_length (data->data));
		g_print ("   %-3s %-10s %s\n", "ID", "GROUP", "DESCRIPTION");
		
		oseaclient_list_foreach (data->data, group_list_foreach_function, group_data);		
	}else {
		g_print ("Unable to list already created groups: %s\n", data->text_response);
		return FALSE;
	}

	group_process_var = TRUE;	

	return TRUE;
}

gboolean group_user_list_process (OseaClientData * data, gpointer user_data) {
	
	if (data->state == OSEACLIENT_OK) {
		g_print ("\n  Number of users %d.\n", oseaclient_list_length (data->data));
		g_print ("   %-3s %-10s %s\n", "ID", "USER", "DESCRIPTION");
		
		oseaclient_list_foreach (data->data, user_list_foreach_function, user_data);		
	}else {
		g_print ("Unable to list already created users: %s\n", data->text_response);
		return FALSE;
	}

	group_process_var = TRUE;

	return TRUE;
}


gboolean group_list   (gchar * line, gpointer data)
{
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}
	
	if (!aos_kernel_group_list (0, 0,  group_list_process, NULL)) {
		g_print ("Unable to list already created groups\n");
		return FALSE;
	}
	
	group_process_var = FALSE;
	while (group_process_var == FALSE) 
		sleep (1);

	return TRUE;
}

gboolean group_list_by_user   (gchar * line, gpointer data)
{
	gchar ** splited_string;
	gchar  * group_id;
	gchar  * aux_string;
	gint     id;
	
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}

	// Get group is to list
	splited_string = g_strsplit (line, " ", 4);
	if (splited_string[3] == NULL) {
		group_id = readline ("Enter group id to list user: ");
	}else
		group_id = g_strstrip (splited_string[2]);
	
	id = strtol (group_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid group id..list group user failed.\n");
		return FALSE;
	}

	if (!aos_kernel_group_list_user (0, 0, id, group_user_list_process, NULL)) {
		g_print ("Unable to list users in group\n");
		return FALSE;
	}
	
	group_process_var = FALSE;
	while (group_process_var == FALSE) 
		sleep (1);

	return TRUE;
}

gboolean group_add_user_process (OseaClientNulData * data, gpointer user_data)
{
	if (data->state == OSEACLIENT_OK) {
		g_print ("User added Ok\n");
	}else {
		g_print ("Unable to add user..add group user failed: %s\n", data->text_response);
	}
	group_process_var = TRUE;
	return TRUE;
}

gboolean group_add_user   (gchar * line, gpointer data)
{
	gchar  * group_id;
	gchar  * user_id;
	gchar  * aux_string;
	gchar ** splited_string;
	gint     id_group;
	gint     id_user;
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}

	// Get group is to remove
	splited_string = g_strsplit (line, " ", 4);
	if (splited_string[3] == NULL) {
		group_id = readline ("Enter group id where user will be included: ");
	}else
		group_id = g_strstrip (splited_string[2]);
	
	id_group = strtol (group_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid group id..add group user failed.\n");
		return FALSE;
	}
	
	user_id = readline ("Enter user id to be included: ");
	if (!user_id) {
		g_print ("Invalid user id..add group user failed.\n");
		return FALSE;
	}

	id_user = strtol (user_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid user id..add group user failed.\n");
		return FALSE;
	}

	if (!aos_kernel_group_add_user (id_group, id_user,  group_add_user_process, NULL)) {
		g_print ("Unable to add user in group\n");
		return FALSE;
	}
	
	group_process_var = FALSE;
	while (group_process_var == FALSE) 
		sleep (1);

	return TRUE;
}

gboolean group_del_user_process (OseaClientNulData * data, gpointer user_data)
{
	if (data->state == OSEACLIENT_OK) {
		g_print ("User deleted Ok\n");
	}else {
		g_print ("Unable to delete user..del group user failed: %s\n", data->text_response);
	}
	group_process_var = TRUE;
	return TRUE;
}

gboolean group_del_user     (gchar * line, gpointer data)
{

	gchar  * group_id;
	gchar  * user_id;
	gchar  * aux_string;
	gchar ** splited_string;
	gint     id_group;
	gint     id_user;
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}

	// Get group is to remove
	splited_string = g_strsplit (line, " ", 4);
	if (splited_string[3] == NULL) {
		group_id = readline ("Enter group id where user will be included: ");
	}else
		group_id = g_strstrip (splited_string[2]);
	
	id_group = strtol (group_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid group id..del group user failed.\n");
		return FALSE;
	}
	
	user_id = readline ("Enter user id to be included: ");
	if (!user_id) {
		g_print ("Invalid user id..del group user failed.\n");
		return FALSE;
	}

	id_user = strtol (user_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid user id..del group user failed.\n");
		return FALSE;
	}

	if (!aos_kernel_group_remove_user (id_group, id_user,  group_del_user_process, NULL)) {
		g_print ("Unable to del user in group\n");
		return FALSE;
	}
	
	group_process_var = FALSE;
	while (group_process_var == FALSE) 
		sleep (1);

	return TRUE;

}
