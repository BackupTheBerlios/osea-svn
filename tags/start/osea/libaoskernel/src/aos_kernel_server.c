//
//  LibAfdalKernel: interface library to the kernel daemon
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
	AfDalKernelServer * server = (AfDalKernelServer *) data;
	
	g_free (server->name);
	g_free (server->description);
	g_free (server);

	return; 
}

AfDalList * __aos_kernel_list_server_create_tree (CoyoteDataSet *data)
{
	AfDalKernelServer * server;
	AfDalList         * result;
	gint i;
	
	result = afdal_list_new_full (afdal_support_compare_id, NULL, __aos_kernel_server_destroy);

	for (i=0; i < coyote_dataset_get_height (data); i++) {
		server = g_new0 (AfDalKernelServer,1);

		server->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		server->name = g_strdup (coyote_dataset_get (data, i, 1));
		server->version = afdal_support_get_number (coyote_dataset_get (data, i, 2));
		server->description = g_strdup (coyote_dataset_get (data, i, 3));

		afdal_list_insert (result, GINT_TO_POINTER (server->id), server);
	}

	return result;
}

static  void __aos_kernel_server_list_process (RRChannel * channel, 
					       RRFrame   * frame,
					       GString   * message, 
					       gpointer    data, 
					       gpointer    custom_data)
{
	AfDalData     * afdal_data = NULL;
	CoyoteDataSet * dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	afdal_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_DATA, channel,
								  frame, message, &dataset, NULL, &data, &custom_data);

	if (! afdal_data)
		return;

	// Fill server data inside server_data
	afdal_data->data = __aos_kernel_list_server_create_tree (dataset);

	// Call to server defined callback.	
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);

	return;
}

/**
 * aos_kernel_server_list:
 * @initial_server: to list for
 * @max_row_number: max rows to be listed 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all installed servers at af-kernel.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_server_list     (gint           initial_server, 
						 gint           max_row_number,
						 AfDalDataFunc  usr_function, 
						 gpointer       usr_data)
{
	gboolean result = FALSE;
	GString * init, * rows;
	RRConnection * connection = NULL;

	connection = afdal_session_get_connection ("af-kernel", NULL);
	if (! connection)
		return FALSE;
	
	init = g_string_new (NULL);
	rows = g_string_new (NULL);

	g_string_sprintf(init, "%d", initial_server);
	g_string_sprintf(rows, "%d", max_row_number);

	result = afdal_request (connection, 
				__aos_kernel_server_list_process, 
				(AfDalFunc) usr_function, usr_data,
				"server_list", 
				"initial", COYOTE_XML_ARG_STRING, init->str, 
				"row_number", COYOTE_XML_ARG_STRING, rows->str, 
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
 * Removes from af-kernel the server identified by @id.
 * 
 * Return value: 
 **/
gboolean           aos_kernel_server_remove   (gint id,
						 AfDalNulFunc   usr_function,
						 gpointer       usr_data)
{
	GString * identifier = NULL;
	gboolean  result;
	RRConnection * connection = NULL;

	connection = afdal_session_get_connection ("af-kernel", NULL);
	if (! connection)
		return FALSE;
	
	identifier = g_string_new (NULL);
	
	g_string_sprintf (identifier, "%d", id);
	
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc) usr_function, usr_data,
				"server_remove", 
				"id", COYOTE_XML_ARG_STRING, identifier->str,
				NULL);
	
	g_string_free (identifier, TRUE);
	
	return result;
};


