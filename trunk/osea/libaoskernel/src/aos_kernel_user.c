//
//  LibAosKernel: interface library to the kernel daemon
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

#include "aos-kernel-user.h"
#include "aos_kernel_user.h"

void __aos_kernel_user_destroy (gpointer data)
{
	AosKernelUser * user = AOS_KERNEL_USER(data);
	
	g_object_unref ((GObject *) user);

	return; 
}

OseaClientList * __aos_kernel_list_user_create_tree (OseaCommDataSet *data)
{
	AosKernelUser * user;
	OseaClientList       * result;
	gint i;
	
	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL, __aos_kernel_user_destroy);

	for (i=0; i < oseacomm_dataset_get_height (data); i++) {
		user = aos_kernel_user_create (oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0)),
					      (gchar *) oseacomm_dataset_get (data, i, 1),
					      (gchar *) oseacomm_dataset_get (data, i, 2));

		oseaclient_list_insert (result, GINT_TO_POINTER (user->id), user);
	}

	return result;
}

static  void __aos_kernel_user_list_process (RRChannel * channel, 
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

	// Fill user data inside user_data
	oseaclient_data->data = __aos_kernel_list_user_create_tree (dataset);

	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);

	return;
}

/**
 * aos_kernel_user_list:
 * @initial_user: to list for
 * @max_row_number: max rows to be listed 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all available user.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_user_list     (gint           initial_user, 
					       gint           max_row_number,
					       OseaClientDataFunc  usr_function, 
					       gpointer       usr_data)
{
	gboolean result = FALSE;
	GString * init, * rows;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	init = g_string_new (NULL);
	rows = g_string_new (NULL);

	g_string_sprintf(init, "%d", initial_user);
	g_string_sprintf(rows, "%d", max_row_number);

	result = oseaclient_request (connection, 
				__aos_kernel_user_list_process, 
				(OseaClientFunc) usr_function, usr_data,
				"list_user", 
				"initial", OSEACOMM_XML_ARG_STRING, init->str, 
				"row_number", OSEACOMM_XML_ARG_STRING, rows->str, 
				NULL);

	g_string_free (init, TRUE);
	g_string_free (rows, TRUE);
	
	return result;
};

/**
 * aos_kernel_user_new:
 * @nick: user short name
 * @password: user password
 * @description: user long description
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Creates a new user.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_user_new      (gchar           * nick, 
					       gchar           * password, 
					       gchar           * description,
					       OseaClientSimpleFunc   usr_function, 
					       gpointer          usr_data)
{
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;

	return oseaclient_request (connection, oseaclient_request_process_simple_data, (OseaClientFunc) usr_function, usr_data,
			      "new_user",
			      "nick", OSEACOMM_XML_ARG_STRING, nick,
			      "password", OSEACOMM_XML_ARG_STRING, password,
			      "description", OSEACOMM_XML_ARG_STRING, description,
			      NULL);
};

/**
 * aos_kernel_user_remove:
 * @id: user id.
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Removes the given user identified by @id from the kernel
 * 
 * Return value: 
 **/
gboolean           aos_kernel_user_remove   (gint id,
					       OseaClientNulFunc   usr_function,
					       gpointer       usr_data)
{
	gchar        * identifier = NULL;
	gboolean       result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	identifier = g_strdup_printf ("%d", id);
	
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"remove_user", 
				"id", OSEACOMM_XML_ARG_STRING, identifier,
				NULL);
	
	g_free (identifier);
	
	return result;
};


/**
 * aos_kernel_user_edit:
 * @id: 
 * @new_nick: new nick name
 * @new_password: new user password
 * @new_description: new user description
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Edit the given user identified by @id.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_user_edit     (gint           id,
					       gchar        * new_nick, 
					       gchar        * new_password, 
					       gchar        * new_description,
					       OseaClientNulFunc   usr_function, 
					       gpointer       usr_data)
{
	gchar        * identifier = NULL;
	gboolean  result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);
	if (! connection)
		return FALSE;
	
	identifier = g_strdup_printf ("%d", id);
	
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"edit_user", 
				"id", OSEACOMM_XML_ARG_STRING, identifier,
				"new_nick", OSEACOMM_XML_ARG_STRING, new_nick,
				"new_password", OSEACOMM_XML_ARG_STRING, new_password,
				"new_description", OSEACOMM_XML_ARG_STRING, new_description,
				NULL);
	
	g_free (identifier);
	
	return result;
};


/**
 * aos_kernel_user_list_group:
 * @initial_user: to list for
 * @max_row_number: max rows to be listed 
 * @user_id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * This function list all group which contains the given user @user_id.
 * 
 * Return value: 
 **/
gboolean          aos_kernel_user_list_group   (gint           initial_group, 
						  gint           max_row_number,
	                                          gint           user_id,
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
	id_group = g_strdup_printf ("%d", user_id);

	result = oseaclient_request (connection, 
				__aos_kernel_user_list_process, 
				(OseaClientFunc) usr_function, usr_data,
				"list_group_user", 
				"initial", OSEACOMM_XML_ARG_STRING, init, 
				"row_number", OSEACOMM_XML_ARG_STRING, rows,
				"user_id", OSEACOMM_XML_ARG_STRING, id_group,
				NULL);

	g_free (init);
	g_free (rows);	
	g_free (id_group);

	return result;
}			 

