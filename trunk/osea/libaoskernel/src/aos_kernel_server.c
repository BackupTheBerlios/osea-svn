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

#include "aos_kernel_server.h"

void __aos_kernel_server_destroy (gpointer data)
{
	AosKernelServer * server = (AosKernelServer *) data;
	
	g_free (server->name);
	g_free (server->description);
	g_free (server);

	return; 
}

OseaClientList * __aos_kernel_list_server_create_tree (OseaCommDataSet *data)
{
	AosKernelServer * server;
	OseaClientList         * result;
	gint i;
	
	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL, __aos_kernel_server_destroy);

	for (i=0; i < oseacomm_dataset_get_height (data); i++) {
		server = g_new0 (AosKernelServer,1);

		server->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		server->name = g_strdup (oseacomm_dataset_get (data, i, 1));
		server->version = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 2));
		server->description = g_strdup (oseacomm_dataset_get (data, i, 3));

		oseaclient_list_insert (result, GINT_TO_POINTER (server->id), server);
	}

	return result;
}

static  void __aos_kernel_server_list_process (RRChannel * channel, 
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

	// Fill server data inside server_data
	oseaclient_data->data = __aos_kernel_list_server_create_tree (dataset);

	// Call to server defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);

	return;
}

/**
 * aos_kernel_server_list:
 * @initial_server: to list for
 * @max_row_number: max rows to be listed 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all installed servers at os-kernel.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_server_list     (gint           initial_server, 
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

	g_string_sprintf(init, "%d", initial_server);
	g_string_sprintf(rows, "%d", max_row_number);

	result = oseaclient_request (connection, 
				__aos_kernel_server_list_process, 
				(OseaClientFunc) usr_function, usr_data,
				"server_list", 
				"initial", OSEACOMM_XML_ARG_STRING, init->str, 
				"row_number", OSEACOMM_XML_ARG_STRING, rows->str, 
				NULL);

	g_string_free (init, TRUE);
	g_string_free (rows, TRUE);
	
	return result;
};

/**
 * aos_kernel_server_remove:
 * @id: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * Removes from os-kernel the server identified by @id.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_server_remove   (gint id,
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
				"server_remove", 
				"id", OSEACOMM_XML_ARG_STRING, identifier->str,
				NULL);
	
	g_string_free (identifier, TRUE);
	
	return result;
};


