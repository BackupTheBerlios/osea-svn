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

#include "aos-kernel-group.h"
#include "aos_kernel_group.h"


void __aos_kernel_group_destroy (gpointer data)
{
	AosKernelGroup * group = AOS_KERNEL_GROUP (data);
	
	g_object_unref (G_OBJECT (data));
	
	return; 
}

OseaClientList * __aos_kernel_list_group_create_tree (OseaCommDataSet *data)
{
	AosKernelGroup * group;
	OseaClientList        * result;
	gint i;
	
	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL, __aos_kernel_group_destroy);

	for (i=0; i < oseacomm_dataset_get_height (data); i++) {
		group = aos_kernel_group_create (oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0)),
						   (gchar *) oseacomm_dataset_get (data, i, 1),
						   (gchar *) oseacomm_dataset_get (data, i, 2));

		oseaclient_list_insert (result, GINT_TO_POINTER (group->id), group);
	}

	return result;
}

static  void __aos_kernel_group_list_process (RRChannel * channel, 
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

	// Fill group data inside group_data
	oseaclient_data->data = __aos_kernel_list_group_create_tree (dataset);

	// Call to group defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);

	return;
}

/**
 * aos_kernel_group_list:
 * @initial_group: to list for
 * @max_row_number: max rows to be listed
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all available group at os-kernel. 
 * 
 * Return value: 
 **/
gboolean           aos_kernel_group_list     (gint           initial_group, 
						gint           max_row_number,
						OseaClientDataFunc  usr_function, 
						gpointer       usr_data)
{
	gboolean result = FALSE;
	gchar * init, * rows;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	init = g_strdup_printf ("%d", initial_group);
	rows = g_strdup_printf ("%d", max_row_number);

	result = oseaclient_request (connection, 
				__aos_kernel_group_list_process, 
				(OseaClientFunc) usr_function, usr_data,
				"list_group", 
				"initial", OSEACOMM_XML_ARG_STRING, init, 
				"row_number", OSEACOMM_XML_ARG_STRING, rows, 
				NULL);

	g_free (init);
	g_free (rows);
	
	return result;
};

/**
 * aos_kernel_group_new:
 * @name: group name
 * @description:  group description
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Creates a new group. 
 * 
 * Return value: 
 **/
gboolean           aos_kernel_group_new      (gchar           * name, 
						gchar           * description,
						OseaClientSimpleFunc   usr_function, 
						gpointer          usr_data)
{
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	return oseaclient_request (connection, oseaclient_request_process_simple_data, (OseaClientFunc) usr_function, usr_data,
			      "new_group",
			      "name", OSEACOMM_XML_ARG_STRING, name,
			      "description", OSEACOMM_XML_ARG_STRING, description,
			      NULL);
};

/**
 * aos_kernel_group_remove:
 * @id: group id obtained from list_group services request.
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Removes a group identified by @id.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_group_remove   (gint id,
					       OseaClientNulFunc   usr_function,
					       gpointer       usr_data)
{
	GString * identifier = NULL;
	gboolean  result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	identifier = g_string_new (NULL);
	
	g_string_sprintf (identifier, "%d", id);
	
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"remove_group", 
				"id", OSEACOMM_XML_ARG_STRING, identifier->str,
				NULL);
	
	g_string_free (identifier, TRUE);
	
	return result;
};


/**
 * aos_kernel_group_edit:
 * @id: Group id to be edited
 * @new_name: 
 * @new_description: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Edit group attributes setting @new_name and @new_description.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_group_edit     (gint           id,
						gchar        * new_name, 
						gchar        * new_description,
						OseaClientNulFunc   usr_function, 
						gpointer       usr_data)
{
	GString * identifier = NULL;
	gboolean  result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	identifier = g_string_new (NULL);
	
	g_string_sprintf (identifier, "%d", id);
	
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"edit_group", 
				"id", OSEACOMM_XML_ARG_STRING, identifier->str,
				"new_name", OSEACOMM_XML_ARG_STRING, new_name,
				"new_description", OSEACOMM_XML_ARG_STRING, new_description,
				NULL);
	
	g_string_free (identifier, TRUE);
	
	return result;
};

/**
 * aos_kernel_group_add_user:
 * @group_id: 
 * @user_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Adds an user identified by @user_id to the given group @group_id.
 * 
 * Return value: 
 **/
			 
gboolean           aos_kernel_group_add_user     (gint           group_id,
						    gint           user_id,
						    OseaClientNulFunc   usr_function, 
						    gpointer       usr_data) 
{
	gchar        * string_group_id;
	gchar        * string_user_id;
	gboolean  result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	string_group_id = g_strdup_printf ("%d", group_id);
	string_user_id = g_strdup_printf ("%d", user_id);
	
	
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"add_user_group", 
				"group_id", OSEACOMM_XML_ARG_STRING, string_group_id,
				"user_id", OSEACOMM_XML_ARG_STRING, string_user_id,
				NULL);
	
	g_free (string_group_id);
	g_free (string_user_id);
	
	return result;
}

/**
 * aos_kernel_group_remove_user:
 * @group_id: 
 * @user_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Removes the given user identified by @user_id from the group @group_id.
 * 
 * Return value: 
 **/
gboolean          aos_kernel_group_remove_user   (gint           group_id,
						    gint           user_id,
						    OseaClientNulFunc   usr_function, 
						    gpointer       usr_data)
{
	RRConnection * connection = NULL;
	gchar        * string_group_id;
	gchar        * string_user_id;
	gboolean       result;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	string_group_id = g_strdup_printf ("%d", group_id);
	string_user_id = g_strdup_printf ("%d", user_id);
	
	
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"remove_user_group", 
				"group_id", OSEACOMM_XML_ARG_STRING, string_group_id,
				"user_id", OSEACOMM_XML_ARG_STRING, string_user_id,
				NULL);
	
	g_free (string_group_id);
	g_free (string_user_id);
	
	return result;
}

/**
 * aos_kernel_group_list_user:
 * @initial_group: to list for
 * @max_row_number: max rows to be listed 
 * @group_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all user at the given group @group_id.
 * 
 * Return value: 
 **/
gboolean          aos_kernel_group_list_user   (gint           initial_group, 
						  gint           max_row_number,
	                                          gint           group_id,
						  OseaClientDataFunc  usr_function, 
						  gpointer       usr_data)
{
	gboolean result = FALSE;
	gchar * init, * rows, * id_group;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	init     = g_strdup_printf ("%d", initial_group);
	rows     = g_strdup_printf ("%d", max_row_number);
	id_group = g_strdup_printf ("%d", group_id);

	result = oseaclient_request (connection, 
				__aos_kernel_group_list_process, 
				(OseaClientFunc) usr_function, usr_data,
				"list_user_group", 
				"initial", OSEACOMM_XML_ARG_STRING, init, 
				"row_number", OSEACOMM_XML_ARG_STRING, rows,
				"group_id", OSEACOMM_XML_ARG_STRING, id_group,
				NULL);

	g_free (init);
	g_free (rows);	
	g_free (id_group);

	return result;
}
