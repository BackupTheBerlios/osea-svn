//  customer: LibOseaClient layer for client-side
//  Copyright (C) 2002,2003 Advanced Software Production Line, S.L.

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

#include "aos_customer_inet_data.h"

gboolean aos_customer_inet_data_new (gchar * inet_data,
				       gint customer_id,
				       gint enum_values_id,
				       OseaClientSimpleFunc usr_function,
				       gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	result = oseaclient_request (connection, oseaclient_request_process_simple_data, (OseaClientFunc)
				usr_function, usr_data,
				"inet_data_new",
				"inet_data", OSEACOMM_XML_ARG_STRING, inet_data, "customer_id",
				OSEACOMM_XML_ARG_STRING, customer_id_to_string, "enum_values_id",
				OSEACOMM_XML_ARG_STRING, enum_values_id_to_string, NULL);
	g_free (customer_id_to_string);
	g_free (enum_values_id_to_string);
	return result;
}

gboolean aos_customer_inet_data_remove (gint inet_data_id,
					  OseaClientNulFunc usr_function,
					  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"inet_data_remove",
				"inet_data_id", OSEACOMM_XML_ARG_STRING, inet_data_id_to_string,
				NULL);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean aos_customer_inet_data_edit (gint inet_data_id,
					gchar * new_inet_data,
					gint new_customer_id,
					gint new_enum_values_id,
					OseaClientNulFunc usr_function,
					gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gchar              *new_customer_id_to_string = NULL;
	gchar              *new_enum_values_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	new_customer_id_to_string = g_strdup_printf ("%d", new_customer_id);
	new_enum_values_id_to_string = g_strdup_printf ("%d", new_enum_values_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"inet_data_edit",
				"inet_data_id", OSEACOMM_XML_ARG_STRING, inet_data_id_to_string,
				"new_inet_data", OSEACOMM_XML_ARG_STRING, new_inet_data,
				"new_customer_id", OSEACOMM_XML_ARG_STRING,
				new_customer_id_to_string, "new_enum_values_id",
				OSEACOMM_XML_ARG_STRING, new_enum_values_id_to_string, NULL);
	g_free (inet_data_id_to_string);
	g_free (new_customer_id_to_string);
	g_free (new_enum_values_id_to_string);
	return result;
}

void __aos_customer_inet_data_list_destroy (gpointer data)
{
	AosCustomerInet_data *inet_data = (AosCustomerInet_data *) data;

	g_free (inet_data->inet_data);
	g_free (inet_data);
	return;
}

OseaClientList          *__aos_customer_inet_data_list_create_oseaclient_list (OseaCommDataSet * data)
{
	AosCustomerInet_data *inet_data;
	OseaClientList          *result;
	gint                i;

	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL,
				      __aos_customer_inet_data_list_destroy);
	for (i = 0; i < oseacomm_dataset_get_height (data); i++) {
		inet_data = g_new0 (AosCustomerInet_data, 1);
		inet_data->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		inet_data->enum_values_id =
			oseaclient_support_get_number (oseacomm_dataset_get (data, i, 1));
		inet_data->customer_id =
			oseaclient_support_get_number (oseacomm_dataset_get (data, i, 2));
		inet_data->inet_data = g_strdup (oseacomm_dataset_get (data, i, 3));
		oseaclient_list_insert (result, GINT_TO_POINTER (inet_data->id), inet_data);
	}
	return result;
}

static void __aos_customer_inet_data_list_process (RRChannel * channel,
						     RRFrame * frame,
						     GString * message,
						     gpointer data,
						     gpointer custom_data)
{
	OseaClientData          *oseaclient_data = NULL;
	OseaCommDataSet      *dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);
	//Close the channel properly and get the incoming oseacomm dataset
	oseaclient_data = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_DATA, channel,
								  frame, message, &dataset, NULL,
								  &data, &custom_data);
	if (!oseaclient_data)
		return;
	// Translate incoming string data into typed-data
	oseaclient_data->data = __aos_customer_inet_data_list_create_oseaclient_list (dataset);
	// Call to user defined callback
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);
	return;
}

gboolean aos_customer_inet_data_list (gint max_row_number,
					gint initial_inet_data,
					OseaClientDataFunc usr_function,
					gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *initial_inet_data_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	initial_inet_data_to_string = g_strdup_printf ("%d", initial_inet_data);
	result = oseaclient_request (connection, __aos_customer_inet_data_list_process, (OseaClientFunc)
				usr_function, usr_data,
				"inet_data_list",
				"max_row_number", OSEACOMM_XML_ARG_STRING, max_row_number_to_string,
				"initial_inet_data", OSEACOMM_XML_ARG_STRING,
				initial_inet_data_to_string, NULL);
	g_free (max_row_number_to_string);
	g_free (initial_inet_data_to_string);
	return result;
}
