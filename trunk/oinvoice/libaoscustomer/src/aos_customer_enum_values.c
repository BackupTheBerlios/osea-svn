//  customer: LibAfDal layer for client-side
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

#include "afdal_customer_enum_values.h"

gboolean afdal_customer_enum_values_address_set (gint enum_values_id,
						 gint address_id,
						 gboolean value_to_set,
						 AfDalNulFunc usr_function,
						 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_address_set",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string,
				"value_to_set", COYOTE_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (address_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean afdal_customer_enum_values_address_update_all (gint address_id,
							AfDalList * setof_enum_values,
							AfDalNulFunc usr_function,
							gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_address_update_all",
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string,
				"setof_enum_values", COYOTE_XML_ARG_STRING, setof_enum_values,
				NULL);
	g_free (address_id_to_string);
	return result;
}

void __afdal_customer_enum_values_address_list_destroy (gpointer data)
{
	AfDalCustomerEnum_values *enum_values = (AfDalCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

AfDalList          *__afdal_customer_enum_values_address_list_create_afdal_list (CoyoteDataSet *
										 data)
{
	AfDalCustomerEnum_values *enum_values;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_enum_values_address_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		enum_values = g_new0 (AfDalCustomerEnum_values, 1);
		enum_values->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (coyote_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (coyote_dataset_get (data, i, 3));
		afdal_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __afdal_customer_enum_values_address_list_process (RRChannel * channel,
							       RRFrame * frame,
							       GString * message,
							       gpointer data,
							       gpointer custom_data)
{
	AfDalData          *afdal_data = NULL;
	CoyoteDataSet      *dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);
	//Close the channel properly and get the incoming coyote dataset
	afdal_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_DATA, channel,
								  frame, message, &dataset, NULL,
								  &data, &custom_data);
	if (!afdal_data)
		return;
	// Translate incoming string data into typed-data
	afdal_data->data = __afdal_customer_enum_values_address_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_enum_values_address_list (gint initial_enum_values,
						  gint max_row_number,
						  gint address_id,
						  AfDalDataFunc usr_function,
						  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = afdal_request (connection, __afdal_customer_enum_values_address_list_process,
				(AfDalFunc)
				usr_function, usr_data, "enum_values_address_list",
				"initial_enum_values", COYOTE_XML_ARG_STRING,
				initial_enum_values_to_string, "max_row_number",
				COYOTE_XML_ARG_STRING, max_row_number_to_string, "address_id",
				COYOTE_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (initial_enum_values_to_string);
	g_free (max_row_number_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_address_remove (gint enum_values_id,
						    gint address_id,
						    AfDalNulFunc usr_function,
						    gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_address_remove",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (enum_values_id_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_address_add (gint enum_values_id,
						 gint address_id,
						 AfDalNulFunc usr_function,
						 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_address_add",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (enum_values_id_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_telephone_set (gint enum_values_id,
						   gint telephone_id,
						   gboolean value_to_set,
						   AfDalNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_telephone_set",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				"value_to_set", COYOTE_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (telephone_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean afdal_customer_enum_values_telephone_update_all (gint telephone_id,
							  AfDalList * setof_enum_values,
							  AfDalNulFunc usr_function,
							  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_telephone_update_all",
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				"setof_enum_values", COYOTE_XML_ARG_STRING, setof_enum_values,
				NULL);
	g_free (telephone_id_to_string);
	return result;
}

void __afdal_customer_enum_values_telephone_list_destroy (gpointer data)
{
	AfDalCustomerEnum_values *enum_values = (AfDalCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

AfDalList          *__afdal_customer_enum_values_telephone_list_create_afdal_list (CoyoteDataSet *
										   data)
{
	AfDalCustomerEnum_values *enum_values;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_enum_values_telephone_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		enum_values = g_new0 (AfDalCustomerEnum_values, 1);
		enum_values->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (coyote_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (coyote_dataset_get (data, i, 3));
		afdal_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __afdal_customer_enum_values_telephone_list_process (RRChannel * channel,
								 RRFrame * frame,
								 GString * message,
								 gpointer data,
								 gpointer custom_data)
{
	AfDalData          *afdal_data = NULL;
	CoyoteDataSet      *dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);
	//Close the channel properly and get the incoming coyote dataset
	afdal_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_DATA, channel,
								  frame, message, &dataset, NULL,
								  &data, &custom_data);
	if (!afdal_data)
		return;
	// Translate incoming string data into typed-data
	afdal_data->data =
		__afdal_customer_enum_values_telephone_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_enum_values_telephone_list (gint initial_enum_values,
						    gint max_row_number,
						    gint telephone_id,
						    AfDalDataFunc usr_function,
						    gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = afdal_request (connection, __afdal_customer_enum_values_telephone_list_process,
				(AfDalFunc)
				usr_function, usr_data, "enum_values_telephone_list",
				"initial_enum_values", COYOTE_XML_ARG_STRING,
				initial_enum_values_to_string, "max_row_number",
				COYOTE_XML_ARG_STRING, max_row_number_to_string, "telephone_id",
				COYOTE_XML_ARG_STRING, telephone_id_to_string, NULL);
	g_free (initial_enum_values_to_string);
	g_free (max_row_number_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_telephone_remove (gint enum_values_id,
						      gint telephone_id,
						      AfDalNulFunc usr_function,
						      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_telephone_remove",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_telephone_add (gint enum_values_id,
						   gint telephone_id,
						   AfDalNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_telephone_add",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_inet_data_set (gint enum_values_id,
						   gint inet_data_id,
						   gboolean value_to_set,
						   AfDalNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_inet_data_set",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"inet_data_id", COYOTE_XML_ARG_STRING, inet_data_id_to_string,
				"value_to_set", COYOTE_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (inet_data_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean afdal_customer_enum_values_inet_data_update_all (gint inet_data_id,
							  AfDalList * setof_enum_values,
							  AfDalNulFunc usr_function,
							  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_inet_data_update_all",
				"inet_data_id", COYOTE_XML_ARG_STRING, inet_data_id_to_string,
				"setof_enum_values", COYOTE_XML_ARG_STRING, setof_enum_values,
				NULL);
	g_free (inet_data_id_to_string);
	return result;
}

void __afdal_customer_enum_values_inet_data_list_destroy (gpointer data)
{
	AfDalCustomerEnum_values *enum_values = (AfDalCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

AfDalList          *__afdal_customer_enum_values_inet_data_list_create_afdal_list (CoyoteDataSet *
										   data)
{
	AfDalCustomerEnum_values *enum_values;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_enum_values_inet_data_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		enum_values = g_new0 (AfDalCustomerEnum_values, 1);
		enum_values->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (coyote_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (coyote_dataset_get (data, i, 3));
		afdal_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __afdal_customer_enum_values_inet_data_list_process (RRChannel * channel,
								 RRFrame * frame,
								 GString * message,
								 gpointer data,
								 gpointer custom_data)
{
	AfDalData          *afdal_data = NULL;
	CoyoteDataSet      *dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);
	//Close the channel properly and get the incoming coyote dataset
	afdal_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_DATA, channel,
								  frame, message, &dataset, NULL,
								  &data, &custom_data);
	if (!afdal_data)
		return;
	// Translate incoming string data into typed-data
	afdal_data->data =
		__afdal_customer_enum_values_inet_data_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_enum_values_inet_data_list (gint initial_enum_values,
						    gint max_row_number,
						    gint inet_data_id,
						    AfDalDataFunc usr_function,
						    gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = afdal_request (connection, __afdal_customer_enum_values_inet_data_list_process,
				(AfDalFunc)
				usr_function, usr_data, "enum_values_inet_data_list",
				"initial_enum_values", COYOTE_XML_ARG_STRING,
				initial_enum_values_to_string, "max_row_number",
				COYOTE_XML_ARG_STRING, max_row_number_to_string, "inet_data_id",
				COYOTE_XML_ARG_STRING, inet_data_id_to_string, NULL);
	g_free (initial_enum_values_to_string);
	g_free (max_row_number_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_inet_data_remove (gint enum_values_id,
						      gint inet_data_id,
						      AfDalNulFunc usr_function,
						      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_inet_data_remove",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"inet_data_id", COYOTE_XML_ARG_STRING, inet_data_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_inet_data_add (gint enum_values_id,
						   gint inet_data_id,
						   AfDalNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_inet_data_add",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"inet_data_id", COYOTE_XML_ARG_STRING, inet_data_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_customer_set (gint enum_values_id,
						  gint customer_id,
						  gboolean value_to_set,
						  AfDalNulFunc usr_function,
						  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_customer_set",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"value_to_set", COYOTE_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (customer_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean afdal_customer_enum_values_customer_update_all (gint customer_id,
							 AfDalList * setof_enum_values,
							 AfDalNulFunc usr_function,
							 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_customer_update_all",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"setof_enum_values", COYOTE_XML_ARG_STRING, setof_enum_values,
				NULL);
	g_free (customer_id_to_string);
	return result;
}

void __afdal_customer_enum_values_customer_list_destroy (gpointer data)
{
	AfDalCustomerEnum_values *enum_values = (AfDalCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

AfDalList          *__afdal_customer_enum_values_customer_list_create_afdal_list (CoyoteDataSet *
										  data)
{
	AfDalCustomerEnum_values *enum_values;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_enum_values_customer_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		enum_values = g_new0 (AfDalCustomerEnum_values, 1);
		enum_values->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (coyote_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (coyote_dataset_get (data, i, 3));
		afdal_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __afdal_customer_enum_values_customer_list_process (RRChannel * channel,
								RRFrame * frame,
								GString * message,
								gpointer data,
								gpointer custom_data)
{
	AfDalData          *afdal_data = NULL;
	CoyoteDataSet      *dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);
	//Close the channel properly and get the incoming coyote dataset
	afdal_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_DATA, channel,
								  frame, message, &dataset, NULL,
								  &data, &custom_data);
	if (!afdal_data)
		return;
	// Translate incoming string data into typed-data
	afdal_data->data = __afdal_customer_enum_values_customer_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_enum_values_customer_list (gint initial_enum_values,
						   gint max_row_number,
						   gint customer_id,
						   AfDalDataFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = afdal_request (connection, __afdal_customer_enum_values_customer_list_process,
				(AfDalFunc)
				usr_function, usr_data, "enum_values_customer_list",
				"initial_enum_values", COYOTE_XML_ARG_STRING,
				initial_enum_values_to_string, "max_row_number",
				COYOTE_XML_ARG_STRING, max_row_number_to_string, "customer_id",
				COYOTE_XML_ARG_STRING, customer_id_to_string, NULL);
	g_free (initial_enum_values_to_string);
	g_free (max_row_number_to_string);
	g_free (customer_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_customer_remove (gint enum_values_id,
						     gint customer_id,
						     AfDalNulFunc usr_function,
						     gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_customer_remove",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (customer_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_customer_add (gint enum_values_id,
						  gint customer_id,
						  AfDalNulFunc usr_function,
						  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_customer_add",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	g_free (customer_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_new (gchar * enum_type,
					 gint enum_order_in_type,
					 gchar * enum_value,
					 AfDalSimpleFunc usr_function,
					 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_order_in_type_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_order_in_type_to_string = g_strdup_printf ("%d", enum_order_in_type);
	result = afdal_request (connection, afdal_request_process_simple_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_new",
				"enum_type", COYOTE_XML_ARG_STRING, enum_type,
				"enum_order_in_type", COYOTE_XML_ARG_STRING,
				enum_order_in_type_to_string, "enum_value", COYOTE_XML_ARG_STRING,
				enum_value, NULL);
	g_free (enum_order_in_type_to_string);
	return result;
}

gboolean afdal_customer_enum_values_remove (gint enum_values_id,
					    AfDalNulFunc usr_function,
					    gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_remove",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				NULL);
	g_free (enum_values_id_to_string);
	return result;
}

gboolean afdal_customer_enum_values_edit (gint enum_values_id,
					  gchar * new_enum_type,
					  gint new_enum_order_in_type,
					  gchar * new_enum_value,
					  AfDalNulFunc usr_function,
					  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gchar              *new_enum_order_in_type_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	new_enum_order_in_type_to_string = g_strdup_printf ("%d", new_enum_order_in_type);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_edit",
				"enum_values_id", COYOTE_XML_ARG_STRING, enum_values_id_to_string,
				"new_enum_type", COYOTE_XML_ARG_STRING, new_enum_type,
				"new_enum_order_in_type", COYOTE_XML_ARG_STRING,
				new_enum_order_in_type_to_string, "new_enum_value",
				COYOTE_XML_ARG_STRING, new_enum_value, NULL);
	g_free (enum_values_id_to_string);
	g_free (new_enum_order_in_type_to_string);
	return result;
}

void __afdal_customer_enum_values_list_destroy (gpointer data)
{
	AfDalCustomerEnum_values *enum_values = (AfDalCustomerEnum_values *) data;

	g_free (enum_values->enum_type);
	g_free (enum_values->enum_value);
	g_free (enum_values);
	return;
}

AfDalList          *__afdal_customer_enum_values_list_create_afdal_list (CoyoteDataSet * data)
{
	AfDalCustomerEnum_values *enum_values;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_enum_values_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		enum_values = g_new0 (AfDalCustomerEnum_values, 1);
		enum_values->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		enum_values->enum_type = g_strdup (coyote_dataset_get (data, i, 1));
		enum_values->enum_order_in_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 2));
		enum_values->enum_value = g_strdup (coyote_dataset_get (data, i, 3));
		afdal_list_insert (result, GINT_TO_POINTER (enum_values->id), enum_values);
	}
	return result;
}

static void __afdal_customer_enum_values_list_process (RRChannel * channel,
						       RRFrame * frame,
						       GString * message,
						       gpointer data,
						       gpointer custom_data)
{
	AfDalData          *afdal_data = NULL;
	CoyoteDataSet      *dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);
	//Close the channel properly and get the incoming coyote dataset
	afdal_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_DATA, channel,
								  frame, message, &dataset, NULL,
								  &data, &custom_data);
	if (!afdal_data)
		return;
	// Translate incoming string data into typed-data
	afdal_data->data = __afdal_customer_enum_values_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_enum_values_list (gint max_row_number,
					  gint initial_enum_values,
					  AfDalDataFunc usr_function,
					  gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *initial_enum_values_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	initial_enum_values_to_string = g_strdup_printf ("%d", initial_enum_values);
	result = afdal_request (connection, __afdal_customer_enum_values_list_process, (AfDalFunc)
				usr_function, usr_data,
				"enum_values_list",
				"max_row_number", COYOTE_XML_ARG_STRING, max_row_number_to_string,
				"initial_enum_values", COYOTE_XML_ARG_STRING,
				initial_enum_values_to_string, NULL);
	g_free (max_row_number_to_string);
	g_free (initial_enum_values_to_string);
	return result;
}
