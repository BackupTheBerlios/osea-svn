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


#include <coyote/coyote.h>
#include <afdalkernel/afdal_kernel.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include "connection.h"
#include "permission.h"

gboolean permission_exit_var;
gboolean permission_set_user_exit_var;
gboolean permission_unset_user_exit_var;

gboolean permission_set_group_exit_var;
gboolean permission_unset_group_exit_var;

gboolean permission_list_foreach (gpointer key, gpointer value, gpointer data) 
{
	AfDalKernelPermission * permission;
	permission = (AfDalKernelPermission *) value;

	if (permission->description) {
		g_print ("%-3d %-24s %-16s %-7s %s\n",
			 permission->id, 
			 permission->name, 
			 permission->server_name,
			 permission->server_version,
			 permission->description);
	}else{
		g_print ("%-3d %-24s %-16s %-7s\n",
			 permission->id, 
			 permission->name, 
			 permission->server_name,
			 permission->server_version);

	}
	
	return FALSE;
}

gboolean permission_list_process (AfDalData * data, gpointer user_data)
{
	if (data->state == AFDAL_OK) {
		g_print ("%-3s %-24s %-16s %-7s %s\n\n",
			 "Id", "Name", "Server Name", "Version", "Description");
		afdal_list_foreach (data->data, permission_list_foreach, NULL);
	}else{
		g_print ("Unable to get permission list: %s\n", data->text_response);
	}
	
	permission_exit_var = TRUE;

	return TRUE;
}

gboolean permission_list          (gchar * line, gpointer data)
{
	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	if (!afdal_kernel_permission_list (0, 0,  permission_list_process, NULL)) {
		g_print ("Unable to get permission list\n");
		return FALSE;
	}
	
	permission_exit_var = FALSE;
	while (permission_exit_var == FALSE)
		sleep (1);

	return TRUE;
}

gboolean permission_list_by_user  (gchar * line, gpointer data)
{
	gchar ** splitted_string = NULL;
	gchar  * user_id;
	gchar  * aux_string = NULL;
	gint     id;
	
	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	splitted_string = g_strsplit (line, " ", 4);
	if (splitted_string[3] == NULL) {
		user_id = readline ("Enter user id to get permission list: ");
	}else
		user_id = g_strstrip (splitted_string[3]);
	
	id = strtol (user_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid user id..permission user list failed.\n");
		return FALSE;
	}


	if (!afdal_kernel_permission_list_by_user (0, 0, id,  permission_list_process, NULL)) {
		g_print ("Unable to get permission list\n");
		return FALSE;
	}
	
	permission_exit_var = FALSE;
	while (permission_exit_var == FALSE)
		sleep (1);

	return TRUE;
}


gboolean permission_list_by_group (gchar * line, gpointer data)
{
	gchar ** splitted_string = NULL;
	gchar  * group_id;
	gchar  * aux_string = NULL;
	gint     id;
	
	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	splitted_string = g_strsplit (line, " ", 4);
	if (splitted_string[3] == NULL) {
		group_id = readline ("Enter group id to get permission list: ");
	}else
		group_id = g_strstrip (splitted_string[3]);
	
	id = strtol (group_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid group id..permission group list failed.\n");
		return FALSE;
	}


	if (!afdal_kernel_permission_list_by_group (0, 0, id,  permission_list_process, NULL)) {
		g_print ("Unable to get permission list\n");
		return FALSE;
	}
	
	permission_exit_var = FALSE;
	while (permission_exit_var == FALSE)
		sleep (1);

	return TRUE;
}


gboolean permission_set_user_process (AfDalNulData * data, gpointer usr_data)
{
	if (data->state == AFDAL_OK) {
		g_print ("Permission set to user correctly.\n");
	}else{
		g_print ("Unable to set permission to user: %s\n", data->text_response);
	}
	permission_set_user_exit_var = TRUE;

	return TRUE;
}

gboolean permission_set_user      (gchar * line, gpointer data)
{
	gchar ** splitted_string = NULL;
	gchar  * user_id;
	gchar  * permission_id;
	gchar  * aux_string = NULL;
	gchar  * aux = NULL;
	gint     id_user;
	gint     id_permission;
	
	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	splitted_string = g_strsplit (line, " ", 4);
	if (splitted_string[3] == NULL) {
		user_id = readline ("Enter user id to set permission: ");
	}else
		user_id = g_strstrip (splitted_string[3]);
	
	id_user = strtol (user_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid permission id.. set perm user failed.\n");
		return FALSE;
	}

	permission_id = readline ("Enter permission id to set permission: ");
	aux = NULL;
	id_permission = strtol (permission_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid permission id.. set perm user failed.\n");
		return FALSE;
	}
	

	if (!afdal_kernel_permission_user_set (id_permission, id_user,  
					       permission_set_user_process, NULL)) {
		g_print ("Unable to set user permission\n");
		return FALSE;
	}
					       
	permission_set_user_exit_var = FALSE;
	while (permission_set_user_exit_var == FALSE)
		sleep (1);

	return TRUE;
}

gboolean permission_unset_user_process (AfDalNulData * data, gpointer usr_data)
{
	if (data->state == AFDAL_OK) {
		g_print ("Permission unset to user correctly.\n");
	}else{
		g_print ("Unable to unset permission to user: %s\n", data->text_response);
	}
	permission_unset_user_exit_var = TRUE;

	return TRUE;
}


gboolean permission_unset_user      (gchar * line, gpointer data)
{
	gchar ** splitted_string = NULL;
	gchar  * user_id;
	gchar  * permission_id;
	gchar  * aux_string = NULL;
	gchar  * aux = NULL;
	gint     id_user;
	gint     id_permission;
	
	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	splitted_string = g_strsplit (line, " ", 4);
	if (splitted_string[3] == NULL) {
		user_id = readline ("Enter user id to unset permission: ");
	}else
		user_id = g_strstrip (splitted_string[3]);
	
	id_user = strtol (user_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid permission id.. unset perm user failed.\n");
		return FALSE;
	}

	permission_id = readline ("Enter permission id to unset permission: ");
	aux = NULL;
	id_permission = strtol (permission_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid permission id.. unset perm user failed.\n");
		return FALSE;
	}
	

	if (!afdal_kernel_permission_user_unset (id_permission, id_user,  
						 permission_unset_user_process, NULL)) {
		g_print ("Unable to unset user permission\n");
		return FALSE;
	}
					       
	permission_unset_user_exit_var = FALSE;
	while (permission_unset_user_exit_var == FALSE)
		sleep (1);

	return TRUE;
}

gboolean permission_set_group_process (AfDalNulData * data, gpointer usr_data)
{
	if (data->state == AFDAL_OK) {
		g_print ("Permission set to group correctly.\n");
	}else{
		g_print ("Unable to set permission to group: %s\n", data->text_response);
	}
	permission_set_group_exit_var = TRUE;

	return TRUE;
}

gboolean permission_set_group      (gchar * line, gpointer data)
{
	gchar ** splitted_string = NULL;
	gchar  * group_id;
	gchar  * permission_id;
	gchar  * aux_string = NULL;
	gchar  * aux = NULL;
	gint     id_group;
	gint     id_permission;
	
	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	splitted_string = g_strsplit (line, " ", 4);
	if (splitted_string[3] == NULL) {
		group_id = readline ("Enter group id to set permission: ");
	}else
		group_id = g_strstrip (splitted_string[3]);
	
	id_group = strtol (group_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid permission id.. set perm group failed.\n");
		return FALSE;
	}

	permission_id = readline ("Enter permission id to set permission: ");
	aux = NULL;
	id_permission = strtol (permission_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid permission id.. set perm group failed.\n");
		return FALSE;
	}
	

	if (!afdal_kernel_permission_group_set (id_permission, id_group,  
					       permission_set_group_process, NULL)) {
		g_print ("Unable to set group permission\n");
		return FALSE;
	}
					       
	permission_set_group_exit_var = FALSE;
	while (permission_set_group_exit_var == FALSE)
		sleep (1);

	return TRUE;
}

gboolean permission_unset_group_process (AfDalNulData * data, gpointer usr_data)
{
	if (data->state == AFDAL_OK) {
		g_print ("Permission unset to group correctly.\n");
	}else{
		g_print ("Unable to unset permission to group: %s\n", data->text_response);
	}
	permission_unset_group_exit_var = TRUE;

	return TRUE;
}


gboolean permission_unset_group      (gchar * line, gpointer data)
{
	gchar ** splitted_string = NULL;
	gchar  * group_id;
	gchar  * permission_id;
	gchar  * aux_string = NULL;
	gchar  * aux = NULL;
	gint     id_group;
	gint     id_permission;
	
	// Are we connected ?
	if (!connection_get ()) {
		g_print ("Not connected to a kernel server\n");
		return FALSE;
	}	

	splitted_string = g_strsplit (line, " ", 4);
	if (splitted_string[3] == NULL) {
		group_id = readline ("Enter group id to unset permission: ");
	}else
		group_id = g_strstrip (splitted_string[3]);
	
	id_group = strtol (group_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid permission id.. unset perm group failed.\n");
		return FALSE;
	}

	permission_id = readline ("Enter permission id to unset permission: ");
	aux = NULL;
	id_permission = strtol (permission_id, &aux_string, 10);
	if (strlen (aux_string)) {
		g_print ("Invalid permission id.. unset perm group failed.\n");
		return FALSE;
	}
	

	if (!afdal_kernel_permission_group_unset (id_permission, id_group,  
						 permission_unset_group_process, NULL)) {
		g_print ("Unable to unset group permission\n");
		return FALSE;
	}
					       
	permission_unset_group_exit_var = FALSE;
	while (permission_unset_group_exit_var == FALSE)
		sleep (1);

	return TRUE;
}
