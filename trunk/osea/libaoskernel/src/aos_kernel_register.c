//  LibAfdalKernel: interface library to the kernel daemonstdin: is not a tty

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

#include "aos_kernel_register.h"


AfDalList *__aos_kernel_register_create_tree (CoyoteDataSet *data)
{
	AfDalKernelServerInfo *server;
	AfDalList *result;
	gint i;
	
	result = afdal_list_new (afdal_support_compare_string);
	for (i=0; i < coyote_dataset_get_height (data); i++) {
		server = g_new (AfDalKernelServerInfo, 1);
		server->name = g_strdup (coyote_dataset_get (data, i, 0));
		server->host = g_strdup (coyote_dataset_get (data, i, 1));
		server->port = g_strdup (coyote_dataset_get (data, i, 2));
		afdal_list_insert (result, server->name, server);
	}

	return result;
}



static void __aos_kernel_register_get_servers (RRChannel * channel, 
						 RRFrame * frame,
						 GString * message, 
						 gpointer data, 
						 gpointer custom_data)
{

	AfDalData     * afdal_data = NULL;
	CoyoteDataSet * dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	afdal_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_DATA,
								  channel,
								  frame,
								  message, 
								  &dataset, 
								  NULL, &data, &custom_data);

	if (! afdal_data)
		return;

	// Fill user data inside user_data
	afdal_data->data = __aos_kernel_register_create_tree (dataset);

	// Call to user defined callback.	
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, 
					  custom_data, afdal_data);


	return;

}



/**
 * aos_kernel_register_get_servers:
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all servers available.
 * 
 * Return value: 
 **/
gboolean aos_kernel_register_get_servers (AfDalDataFunc usr_function, 
					    gpointer usr_data)
{
	RRConnection * connection = NULL;

	connection = afdal_session_get_connection ("af-kernel", NULL);
	if (! connection)
		return FALSE;

	return afdal_request (connection, __aos_kernel_register_get_servers, 
			      (AfDalFunc) usr_function, usr_data,
			      "get_servers",
			      NULL);
}

