//
//  LibAosKernel: interface library to the kernel daemon
//  Copyright (C) 2002  Advanced Software Production Line, S.L.
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
#include "aos_kernel_permission.h"


void __aos_kernel_permission_destroy (gpointer data)
{
	AosKernelPermission * permission = (AosKernelPermission *) data;
	
	g_free (permission->name);
	g_free (permission->description);
	g_free (permission->server_name);
	g_free (permission->server_version);
	g_free (permission->depends);
	g_free (permission);
	return;
}

OseaClientList * __aos_kernel_list_permission_create_tree (OseaCommDataSet *data)
{
	AosKernelPermission * permission;
	OseaClientList             * result;
	gint i;
	
	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL, __aos_kernel_permission_destroy);

	for (i=0; i < oseacomm_dataset_get_height (data); i++) {
		permission = g_new0 (AosKernelPermission,1);

		permission->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		permission->name = g_strdup (oseacomm_dataset_get (data, i, 1));
		permission->description = g_strdup (oseacomm_dataset_get (data, i, 2));
		permission->server_name = g_strdup (oseacomm_dataset_get (data, i, 3));
		permission->server_version = g_strdup (oseacomm_dataset_get (data, i, 4));
		permission->depends = g_strdup (oseacomm_dataset_get (data, i, 5));

		oseaclient_list_insert (result, GINT_TO_POINTER (permission->id), permission);
	}

	return result;
}
static  void __aos_kernel_permission_list_process (RRChannel * channel, 
						     RRFrame   * frame,
						     GString   * message, 
						     gpointer    data, 
						     gpointer    custom_data)
{
	OseaClientData     * oseaclient_data = NULL;
	OseaCommDataSet * dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_data = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_DATA, channel,
								  frame, message, &dataset, NULL, &data, &custom_data);

	if (! oseaclient_data)
		return;

	// Fill permission data inside permission_data
	oseaclient_data->data = __aos_kernel_list_permission_create_tree (dataset);

	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);

	return;
}



/**
 * aos_kernel_permission_list:
 * @initial_user: to list for
 * @max_row_number: max rows to be listed 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all available permissions.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_list     (gint           initial_permission, 
						     gint           max_row_number,
						     OseaClientDataFunc  usr_function, 
						     gpointer       usr_data)
{
	gchar * string_initial_permission;
	gchar * string_max_row_number;
	gboolean result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	string_initial_permission = g_strdup_printf ("%d", initial_permission);
	string_max_row_number = g_strdup_printf ("%d", max_row_number);

	result = oseaclient_request (connection, __aos_kernel_permission_list_process, (OseaClientFunc) usr_function, usr_data,
				"permission_list", 
				"initial_permission", OSEACOMM_XML_ARG_STRING, string_initial_permission,
				"row_number", OSEACOMM_XML_ARG_STRING, string_max_row_number,
				NULL);
	
	g_free (string_initial_permission);
	g_free (string_max_row_number);
	return result;
}

/**
 * aos_kernel_permission_list_by_user:
 * @initial_user: to list for
 * @max_row_number: max rows to be listed 
 * @user_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all permission which has the given user @user_id.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_list_by_user        (gint           initial_permission, 
								gint           max_row_number,
								gint           user_id,
								OseaClientDataFunc  usr_function, 
								gpointer       usr_data)
{
	gchar * string_initial_permission;
	gchar * string_max_row_number;
	gchar * string_user_id;
	gboolean result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	string_initial_permission = g_strdup_printf ("%d", initial_permission);
	string_max_row_number     = g_strdup_printf ("%d", max_row_number);
	string_user_id            = g_strdup_printf ("%d", user_id);

	result = oseaclient_request (connection, __aos_kernel_permission_list_process, (OseaClientFunc) usr_function, usr_data,
				"permission_list_by_user", 
				"initial_permission", OSEACOMM_XML_ARG_STRING, string_initial_permission,
				"row_number", OSEACOMM_XML_ARG_STRING, string_max_row_number,
				"user_id", OSEACOMM_XML_ARG_STRING, string_user_id,
				NULL);
	
	g_free (string_initial_permission);
	g_free (string_max_row_number);
	g_free (string_user_id);

	return result;
}

/**
 * aos_kernel_permission_actual_list_by_user:
 * @initial_user: to list for
 * @max_row_number: max rows to be listed 
 * @user_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * This function list all permissions which really has the given user
 * @user_id excluding those permissions that were inherit from group
 * that the user is in.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_actual_list_by_user        (gint           initial_permission, 
								       gint           max_row_number,
								       gint           user_id,
								       OseaClientDataFunc  usr_function, 
								       gpointer       usr_data)
{
	gchar * string_initial_permission;
	gchar * string_max_row_number;
	gchar * string_user_id;
	gboolean result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	string_initial_permission = g_strdup_printf ("%d", initial_permission);
	string_max_row_number     = g_strdup_printf ("%d", max_row_number);
	string_user_id            = g_strdup_printf ("%d", user_id);

	result = oseaclient_request (connection, __aos_kernel_permission_list_process, (OseaClientFunc) usr_function, usr_data,
				"permission_actual_list_by_user", 
				"initial_permission", OSEACOMM_XML_ARG_STRING, string_initial_permission,
				"row_number", OSEACOMM_XML_ARG_STRING, string_max_row_number,
				"user_id", OSEACOMM_XML_ARG_STRING, string_user_id,
				NULL);
	
	g_free (string_initial_permission);
	g_free (string_max_row_number);
	g_free (string_user_id);

	return result;
}

/**
 * aos_kernel_permission_from_group_list_by_user:
 * @initial_user: to list for
 * @max_row_number: max rows to be listed 
 * @user_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * This function return the permission list which has the given user
 * but only those ones which was inherited from group that user
 * belongs to.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_from_group_list_by_user  (gint           initial_permission, 
								     gint           max_row_number,
								     gint           user_id,
								     OseaClientDataFunc  usr_function, 
								     gpointer       usr_data)
{
	gchar * string_initial_permission;
	gchar * string_max_row_number;
	gchar * string_user_id;
	gboolean result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	string_initial_permission = g_strdup_printf ("%d", initial_permission);
	string_max_row_number     = g_strdup_printf ("%d", max_row_number);
	string_user_id            = g_strdup_printf ("%d", user_id);

	result = oseaclient_request (connection, __aos_kernel_permission_list_process, (OseaClientFunc) usr_function, usr_data,
				"permission_from_group_list_by_user", 
				"initial_permission", OSEACOMM_XML_ARG_STRING, string_initial_permission,
				"row_number", OSEACOMM_XML_ARG_STRING, string_max_row_number,
				"user_id", OSEACOMM_XML_ARG_STRING, string_user_id,
				NULL);
	
	g_free (string_initial_permission);
	g_free (string_max_row_number);
	g_free (string_user_id);

	return result;
}


/**
 * aos_kernel_permission_list_by_group:
 * @initial_user: to list for
 * @max_row_number: max rows to be listed 
 * @group_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all permissions which has the given group identified by @group_id.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_list_by_group       (gint           initial_permission, 
								gint           max_row_number,
								gint           group_id,
								OseaClientDataFunc  usr_function, 
								gpointer       usr_data)
{
	gchar * string_initial_permission;
	gchar * string_max_row_number;
	gchar * string_group_id;
	gboolean result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	string_initial_permission = g_strdup_printf ("%d", initial_permission);
	string_max_row_number     = g_strdup_printf ("%d", max_row_number);
	string_group_id            = g_strdup_printf ("%d", group_id);

	result = oseaclient_request (connection, __aos_kernel_permission_list_process, (OseaClientFunc) usr_function, usr_data,
				"permission_list_by_group", 
				"initial_permission", OSEACOMM_XML_ARG_STRING, string_initial_permission,
				"row_number", OSEACOMM_XML_ARG_STRING, string_max_row_number,
				"group_id", OSEACOMM_XML_ARG_STRING, string_group_id,
				NULL);
	
	g_free (string_initial_permission);
	g_free (string_max_row_number);
	g_free (string_group_id);

	return result;
}

/**
 * aos_kernel_permission_user_set:
 * @id: permission id
 * @user_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Set the permission identified by @id to the user identified by @user_id
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_user_set   (gint           id,
						       gint           user_id,
						       OseaClientNulFunc  usr_function, 
						       gpointer       usr_data)
{
	gchar * string_id;
	gchar * string_user_id;
	gboolean result;
	RRConnection * connection = NULL;

	string_id = g_strdup_printf ("%d", id);
	string_user_id = g_strdup_printf ("%d", user_id);

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"permission_user_set", 
				"id", OSEACOMM_XML_ARG_STRING, string_id,
				"user_id", OSEACOMM_XML_ARG_STRING, string_user_id,
				NULL);
	
	g_free (string_id);
	g_free (string_user_id);
	return result;
	
}

/**
 * aos_kernel_permission_user_unset:
 * @id: permission id
 * @user_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Unset the permission identified by @id to the user identified by @user_id
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_user_unset (gint           id,
						       gint           user_id,
						       OseaClientNulFunc  usr_function, 
						       gpointer       usr_data)
{
	gchar * string_id;
	gchar * string_user_id;
	gboolean result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	string_id = g_strdup_printf ("%d", id);
	string_user_id = g_strdup_printf ("%d", user_id);

	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"permission_user_unset", 
				"id", OSEACOMM_XML_ARG_STRING, string_id,
				"user_id", OSEACOMM_XML_ARG_STRING, string_user_id,
				NULL);
	
	g_free (string_id);
	g_free (string_user_id);
	return result;
}

/**
 * aos_kernel_permission_user_set_value:
 * @id: permission id
 * @user_id: 
 * @value: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Convenience function to aos_kernel_permission_user_set. Sets the
 * given @value to the relation between the given permission
 * identified by @id and the given user identified by @user_id.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_user_set_value   (gint           id,
						             gint           user_id,
							     gboolean       value,
							     OseaClientNulFunc   usr_function, 
							     gpointer       usr_data)
{
	if (value)
		return aos_kernel_permission_user_set (id, user_id, usr_function, usr_data);

	return aos_kernel_permission_user_unset (id, user_id, usr_function, usr_data);

}

/**
 * aos_kernel_permission_group_set:
 * @id: permission id
 * @group_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Set the permission identified by @id to the group identified by @group_id
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_group_set   (gint           id,
							gint           group_id,
							OseaClientNulFunc  usr_function, 
							gpointer       usr_data)
{
	gchar * string_id;
	gchar * string_group_id;
	gboolean result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	string_id = g_strdup_printf ("%d", id);
	string_group_id = g_strdup_printf ("%d", group_id);

	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"permission_group_set", 
				"id", OSEACOMM_XML_ARG_STRING, string_id,
				"group_id", OSEACOMM_XML_ARG_STRING, string_group_id,
				NULL);
	
	g_free (string_id);
	g_free (string_group_id);
	return result;
}


/**
 * aos_kernel_permission_group_unset:
 * @id: permission id
 * @group_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Unset the permission identified by @id to the group identified by @group_id
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_group_unset (gint           id,
							gint           group_id,
							OseaClientNulFunc  usr_function, 
							gpointer       usr_data)
{
	gchar * string_id;
	gchar * string_group_id;
	gboolean result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	string_id = g_strdup_printf ("%d", id);
	string_group_id = g_strdup_printf ("%d", group_id);

	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"permission_group_unset", 
				"id", OSEACOMM_XML_ARG_STRING, string_id,
				"group_id", OSEACOMM_XML_ARG_STRING, string_group_id,
				NULL);
	
	g_free (string_id);
	g_free (string_group_id);
	return result;
}

/**
 * aos_kernel_permission_group_set_value:
 * @id: permission id
 * @group_id: 
 * @value: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Convenience function to aos_kernel_permission_group_set. Sets the
 * given @value to the relation between the given permission
 * identified by @id and the given group identified by @group_id.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_permission_group_set_value   (gint           id,
							      gint           group_id,
							      gboolean       value,
							      OseaClientNulFunc   usr_function, 
							      gpointer       usr_data)
{
	if (value)
		return aos_kernel_permission_group_set (id, group_id, usr_function, usr_data);

	return aos_kernel_permission_group_unset (id, group_id, usr_function, usr_data);

}
