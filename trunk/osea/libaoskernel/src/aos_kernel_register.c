//  LibAosKernel: interface library to the kernel daemonstdin: is not a tty

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


OseaClientList *__aos_kernel_register_create_tree (OseaCommDataSet *data)
{
	AosKernelServerInfo *server;
	OseaClientList *result;
	gint i;
	
	result = oseaclient_list_new (oseaclient_support_compare_string);
	for (i=0; i < oseacomm_dataset_get_height (data); i++) {
		server = g_new (AosKernelServerInfo, 1);
		server->name = g_strdup (oseacomm_dataset_get (data, i, 0));
		server->host = g_strdup (oseacomm_dataset_get (data, i, 1));
		server->port = g_strdup (oseacomm_dataset_get (data, i, 2));
		oseaclient_list_insert (result, server->name, server);
	}

	return result;
}



static void __aos_kernel_register_get_servers (RRChannel * channel, 
						 RRFrame * frame,
						 GString * message, 
						 gpointer data, 
						 gpointer custom_data)
{

	OseaClientData     * oseaclient_data = NULL;
	OseaCommDataSet * dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_data = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_DATA,
								  channel,
								  frame,
								  message, 
								  &dataset, 
								  NULL, &data, &custom_data);

	if (! oseaclient_data)
		return;

	// Fill user data inside user_data
	oseaclient_data->data = __aos_kernel_register_create_tree (dataset);

	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, 
					  custom_data, oseaclient_data);


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
gboolean aos_kernel_register_get_servers (OseaClientDataFunc usr_function, 
					    gpointer usr_data)
{
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("af-kernel", NULL);
	if (! connection)
		return FALSE;

	return oseaclient_request (connection, __aos_kernel_register_get_servers, 
			      (OseaClientFunc) usr_function, usr_data,
			      "get_servers",
			      NULL);
}

