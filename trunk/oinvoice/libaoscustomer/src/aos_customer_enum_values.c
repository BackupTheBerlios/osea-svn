//  customer: LibOseaClient layer for client-side
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//  Copyright (C) 2004 David Marín Carreño

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

#include "aos_customer_enum_values.h"

gboolean aos_customer_enum_values_address_set (gint enum_values_id,
						 gint address_id,
						 gboolean value_to_set,
						 OseaClientNulFunc usr_function,
						 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_address_set",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"address_id", OSEACOMM_XML_ARG_STRING, address_id_to_string,
				"value_to_set", OSEACOMM_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (address_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean aos_customer_enum_values_address_update_all (gint address_id,
							OseaClientList * setof_enum_values,
							OseaClientNulFunc usr_function,
							gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_address_update_all",
				"address_id", OSEACOMM_XML_ARG_STRING, address_id_to_string,
				"setof_enum_values", OSEACOMM_XML_ARG_STRING, setof_enum_values,
				NULL);
	g_free (address_id_to_string);
	return result;
}

void __aos_customer_enum_values_address_list_destroy (gpointer data)
{
	AosCustomerEnum_values *enum_values = (AosCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

OseaClientList          *__aos_customer_enum_values_address_list_create_oseaclient_list (OseaCommDataSet *
										 data)
{
	AosCustomerEnum_values *enum_values;
	OseaClientList          *result;
	gint                i;

	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL,
				      __aos_customer_enum_values_address_list_destroy);
	for (i = 0; i < oseacomm_dataset_get_height (data); i++) {
		enum_values = g_new0 (AosCustomerEnum_values, 1);
		enum_values->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (oseacomm_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			oseaclient_support_get_number (oseacomm_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (oseacomm_dataset_get (data, i, 3));
		oseaclient_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __aos_customer_enum_values_address_list_process (RRChannel * channel,
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
	oseaclient_data->data = __aos_customer_enum_values_address_list_create_oseaclient_list (dataset);
	// Call to user defined callback
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);
	return;
}

gboolean aos_customer_enum_values_address_list (gint initial_enum_values,
						  gint max_row_number,
						  gint address_id,
						  OseaClientDataFunc usr_function,
						  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = oseaclient_request (connection, __aos_customer_enum_values_address_list_process,
				(OseaClientFunc)
				usr_function, usr_data, "enum_values_address_list",
				"initial_enum_values", OSEACOMM_XML_ARG_STRING,
				initial_enum_values_to_string, "max_row_number",
				OSEACOMM_XML_ARG_STRING, max_row_number_to_string, "address_id",
				OSEACOMM_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (initial_enum_values_to_string);
	g_free (max_row_number_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_address_remove (gint enum_values_id,
						    gint address_id,
						    OseaClientNulFunc usr_function,
						    gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_address_remove",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"address_id", OSEACOMM_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (enum_values_id_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_address_add (gint enum_values_id,
						 gint address_id,
						 OseaClientNulFunc usr_function,
						 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_address_add",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"address_id", OSEACOMM_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (enum_values_id_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_telephone_set (gint enum_values_id,
						   gint telephone_id,
						   gboolean value_to_set,
						   OseaClientNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_telephone_set",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"telephone_id", OSEACOMM_XML_ARG_STRING, telephone_id_to_string,
				"value_to_set", OSEACOMM_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (telephone_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean aos_customer_enum_values_telephone_update_all (gint telephone_id,
							  OseaClientList * setof_enum_values,
							  OseaClientNulFunc usr_function,
							  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_telephone_update_all",
				"telephone_id", OSEACOMM_XML_ARG_STRING, telephone_id_to_string,
				"setof_enum_values", OSEACOMM_XML_ARG_STRING, setof_enum_values,
				NULL);
	g_free (telephone_id_to_string);
	return result;
}

void __aos_customer_enum_values_telephone_list_destroy (gpointer data)
{
	AosCustomerEnum_values *enum_values = (AosCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

OseaClientList          *__aos_customer_enum_values_telephone_list_create_oseaclient_list (OseaCommDataSet *
										   data)
{
	AosCustomerEnum_values *enum_values;
	OseaClientList          *result;
	gint                i;

	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL,
				      __aos_customer_enum_values_telephone_list_destroy);
	for (i = 0; i < oseacomm_dataset_get_height (data); i++) {
		enum_values = g_new0 (AosCustomerEnum_values, 1);
		enum_values->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (oseacomm_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			oseaclient_support_get_number (oseacomm_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (oseacomm_dataset_get (data, i, 3));
		oseaclient_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __aos_customer_enum_values_telephone_list_process (RRChannel * channel,
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
	oseaclient_data->data =
		__aos_customer_enum_values_telephone_list_create_oseaclient_list (dataset);
	// Call to user defined callback
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);
	return;
}

gboolean aos_customer_enum_values_telephone_list (gint initial_enum_values,
						    gint max_row_number,
						    gint telephone_id,
						    OseaClientDataFunc usr_function,
						    gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = oseaclient_request (connection, __aos_customer_enum_values_telephone_list_process,
				(OseaClientFunc)
				usr_function, usr_data, "enum_values_telephone_list",
				"initial_enum_values", OSEACOMM_XML_ARG_STRING,
				initial_enum_values_to_string, "max_row_number",
				OSEACOMM_XML_ARG_STRING, max_row_number_to_string, "telephone_id",
				OSEACOMM_XML_ARG_STRING, telephone_id_to_string, NULL);
	g_free (initial_enum_values_to_string);
	g_free (max_row_number_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_telephone_remove (gint enum_values_id,
						      gint telephone_id,
						      OseaClientNulFunc usr_function,
						      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_telephone_remove",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"telephone_id", OSEACOMM_XML_ARG_STRING, telephone_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_telephone_add (gint enum_values_id,
						   gint telephone_id,
						   OseaClientNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_telephone_add",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"telephone_id", OSEACOMM_XML_ARG_STRING, telephone_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_inet_data_set (gint enum_values_id,
						   gint inet_data_id,
						   gboolean value_to_set,
						   OseaClientNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_inet_data_set",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"inet_data_id", OSEACOMM_XML_ARG_STRING, inet_data_id_to_string,
				"value_to_set", OSEACOMM_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (inet_data_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean aos_customer_enum_values_inet_data_update_all (gint inet_data_id,
							  OseaClientList * setof_enum_values,
							  OseaClientNulFunc usr_function,
							  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_inet_data_update_all",
				"inet_data_id", OSEACOMM_XML_ARG_STRING, inet_data_id_to_string,
				"setof_enum_values", OSEACOMM_XML_ARG_STRING, setof_enum_values,
				NULL);
	g_free (inet_data_id_to_string);
	return result;
}

void __aos_customer_enum_values_inet_data_list_destroy (gpointer data)
{
	AosCustomerEnum_values *enum_values = (AosCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

OseaClientList          *__aos_customer_enum_values_inet_data_list_create_oseaclient_list (OseaCommDataSet *
										   data)
{
	AosCustomerEnum_values *enum_values;
	OseaClientList          *result;
	gint                i;

	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL,
				      __aos_customer_enum_values_inet_data_list_destroy);
	for (i = 0; i < oseacomm_dataset_get_height (data); i++) {
		enum_values = g_new0 (AosCustomerEnum_values, 1);
		enum_values->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (oseacomm_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			oseaclient_support_get_number (oseacomm_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (oseacomm_dataset_get (data, i, 3));
		oseaclient_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __aos_customer_enum_values_inet_data_list_process (RRChannel * channel,
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
	oseaclient_data->data =
		__aos_customer_enum_values_inet_data_list_create_oseaclient_list (dataset);
	// Call to user defined callback
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);
	return;
}

gboolean aos_customer_enum_values_inet_data_list (gint initial_enum_values,
						    gint max_row_number,
						    gint inet_data_id,
						    OseaClientDataFunc usr_function,
						    gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = oseaclient_request (connection, __aos_customer_enum_values_inet_data_list_process,
				(OseaClientFunc)
				usr_function, usr_data, "enum_values_inet_data_list",
				"initial_enum_values", OSEACOMM_XML_ARG_STRING,
				initial_enum_values_to_string, "max_row_number",
				OSEACOMM_XML_ARG_STRING, max_row_number_to_string, "inet_data_id",
				OSEACOMM_XML_ARG_STRING, inet_data_id_to_string, NULL);
	g_free (initial_enum_values_to_string);
	g_free (max_row_number_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_inet_data_remove (gint enum_values_id,
						      gint inet_data_id,
						      OseaClientNulFunc usr_function,
						      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_inet_data_remove",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"inet_data_id", OSEACOMM_XML_ARG_STRING, inet_data_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_inet_data_add (gint enum_values_id,
						   gint inet_data_id,
						   OseaClientNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_inet_data_add",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"inet_data_id", OSEACOMM_XML_ARG_STRING, inet_data_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_customer_set (gint enum_values_id,
						  gint customer_id,
						  gboolean value_to_set,
						  OseaClientNulFunc usr_function,
						  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_customer_set",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"customer_id", OSEACOMM_XML_ARG_STRING, customer_id_to_string,
				"value_to_set", OSEACOMM_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (customer_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean aos_customer_enum_values_customer_update_all (gint customer_id,
							 OseaClientList * setof_enum_values,
							 OseaClientNulFunc usr_function,
							 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_customer_update_all",
				"customer_id", OSEACOMM_XML_ARG_STRING, customer_id_to_string,
				"setof_enum_values", OSEACOMM_XML_ARG_STRING, setof_enum_values,
				NULL);
	g_free (customer_id_to_string);
	return result;
}

void __aos_customer_enum_values_customer_list_destroy (gpointer data)
{
	AosCustomerEnum_values *enum_values = (AosCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

OseaClientList          *__aos_customer_enum_values_customer_list_create_oseaclient_list (OseaCommDataSet *
										  data)
{
	AosCustomerEnum_values *enum_values;
	OseaClientList          *result;
	gint                i;

	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL,
				      __aos_customer_enum_values_customer_list_destroy);
	for (i = 0; i < oseacomm_dataset_get_height (data); i++) {
		enum_values = g_new0 (AosCustomerEnum_values, 1);
		enum_values->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (oseacomm_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			oseaclient_support_get_number (oseacomm_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (oseacomm_dataset_get (data, i, 3));
		oseaclient_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __aos_customer_enum_values_customer_list_process (RRChannel * channel,
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
	oseaclient_data->data = __aos_customer_enum_values_customer_list_create_oseaclient_list (dataset);
	// Call to user defined callback
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);
	return;
}

gboolean aos_customer_enum_values_customer_list (gint initial_enum_values,
						   gint max_row_number,
						   gint customer_id,
						   OseaClientDataFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = oseaclient_request (connection, __aos_customer_enum_values_customer_list_process,
				(OseaClientFunc)
				usr_function, usr_data, "enum_values_customer_list",
				"initial_enum_values", OSEACOMM_XML_ARG_STRING,
				initial_enum_values_to_string, "max_row_number",
				OSEACOMM_XML_ARG_STRING, max_row_number_to_string, "customer_id",
				OSEACOMM_XML_ARG_STRING, customer_id_to_string, NULL);
	g_free (initial_enum_values_to_string);
	g_free (max_row_number_to_string);
	g_free (customer_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_customer_remove (gint enum_values_id,
						     gint customer_id,
						     OseaClientNulFunc usr_function,
						     gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_customer_remove",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"customer_id", OSEACOMM_XML_ARG_STRING, customer_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (customer_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_customer_add (gint enum_values_id,
						  gint customer_id,
						  OseaClientNulFunc usr_function,
						  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_customer_add",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"customer_id", OSEACOMM_XML_ARG_STRING, customer_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (customer_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_new (gchar * enum_type,
					 gint enum_order_in_type,
					 gchar * enum_value,
					 OseaClientSimpleFunc usr_function,
					 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_order_in_type_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_order_in_type_to_string = g_strdup_printf ("%d", enum_order_in_type);
	result = oseaclient_request (connection, oseaclient_request_process_simple_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_new",
				"enum_type", OSEACOMM_XML_ARG_STRING, enum_type,
				"enum_order_in_type", OSEACOMM_XML_ARG_STRING,
				enum_order_in_type_to_string, "enum_value", OSEACOMM_XML_ARG_STRING,
				enum_value, NULL);
	g_free (enum_order_in_type_to_string);
	return result;
}

gboolean aos_customer_enum_values_remove (gint enum_values_id,
					    OseaClientNulFunc usr_function,
					    gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_remove",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	return result;
}

gboolean aos_customer_enum_values_edit (gint enum_values_id,
					  gchar * new_enum_type,
					  gint new_enum_order_in_type,
					  gchar * new_enum_value,
					  OseaClientNulFunc usr_function,
					  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *new_enum_order_in_type_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	new_enum_order_in_type_to_string = g_strdup_printf ("%d", new_enum_order_in_type);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_edit",
				"enum_values_id", OSEACOMM_XML_ARG_STRING, enum_values_id_to_string,
				"new_enum_type", OSEACOMM_XML_ARG_STRING, new_enum_type,
				"new_enum_order_in_type", OSEACOMM_XML_ARG_STRING,
				new_enum_order_in_type_to_string, "new_enum_value",
				OSEACOMM_XML_ARG_STRING, new_enum_value, NULL);
	g_free (enum_values_id_to_string);
	g_free (new_enum_order_in_type_to_string);
	return result;
}

void __aos_customer_enum_values_list_destroy (gpointer data)
{
	AosCustomerEnum_values *enum_values = (AosCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

OseaClientList          *__aos_customer_enum_values_list_create_oseaclient_list (OseaCommDataSet * data)
{
	AosCustomerEnum_values *enum_values;
	OseaClientList          *result;
	gint                i;

	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL,
				      __aos_customer_enum_values_list_destroy);
	for (i = 0; i < oseacomm_dataset_get_height (data); i++) {
		enum_values = g_new0 (AosCustomerEnum_values, 1);
		enum_values->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (oseacomm_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			oseaclient_support_get_number (oseacomm_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (oseacomm_dataset_get (data, i, 3));
		oseaclient_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __aos_customer_enum_values_list_process (RRChannel * channel,
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
	oseaclient_data->data = __aos_customer_enum_values_list_create_oseaclient_list (dataset);
	// Call to user defined callback
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);
	return;
}

gboolean aos_customer_enum_values_list (gint max_row_number,
					  gint initial_enum_values,
					  OseaClientDataFunc usr_function,
					  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	result = oseaclient_request (connection, __aos_customer_enum_values_list_process, (OseaClientFunc)
				usr_function, usr_data,
				"enum_values_list",
				"max_row_number", OSEACOMM_XML_ARG_STRING, max_row_number_to_string,
				"initial_enum_values", OSEACOMM_XML_ARG_STRING,
				initial_enum_values_to_string, NULL);
	g_free (max_row_number_to_string);
	g_free (initial_enum_values_to_string);
	return result;
}
