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

#include "afdal-customer-customer.h"
#include "afdal_customer_customer.h"

gboolean afdal_customer_customer_address_set (gint customer_id,
					      gint address_id,
					      gboolean value_to_set,
					      AfDalNulFunc usr_function,
					      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_address_set",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string,
				"value_to_set", COYOTE_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (customer_id_to_string);
	g_free (address_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean afdal_customer_customer_address_update_all (gint address_id,
						     AfDalList * setof_customer,
						     AfDalNulFunc usr_function,
						     gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_address_update_all",
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string,
				"setof_customer", COYOTE_XML_ARG_STRING, setof_customer, NULL);
	g_free (address_id_to_string);
	return result;
}

void __afdal_customer_customer_address_list_destroy (gpointer data)
{
	AfDalCustomerCustomer *customer = (AfDalCustomerCustomer *) data;

	g_object_unref (G_OBJECT(customer));
	return;
}

AfDalList          *__afdal_customer_customer_address_list_create_afdal_list (CoyoteDataSet *
									      data)
{
	AfDalCustomerCustomer *customer;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_customer_address_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		customer = afdal_customer_customer_create ();
		customer->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		customer->legal_id = g_strdup (coyote_dataset_get (data, i, 1));
		customer->name = g_strdup (coyote_dataset_get (data, i, 2));
		customer->surname = g_strdup (coyote_dataset_get (data, i, 3));
		customer->salesman = g_strdup (coyote_dataset_get (data, i, 4));
		customer->vat_customer_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 5));
		customer->customer_ref = g_strdup (coyote_dataset_get (data, i, 6));
		customer->enum_values_id =
			afdal_support_get_number (coyote_dataset_get (data, i, 7));
		afdal_list_insert (result, GINT_TO_POINTER (customer->id), customer);
	}
	return result;
}

static void __afdal_customer_customer_address_list_process (RRChannel * channel,
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
	afdal_data->data = __afdal_customer_customer_address_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_customer_address_list (gint initial_customer,
					       gint max_row_number,
					       gint address_id,
					       AfDalDataFunc usr_function,
					       gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_customer_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	initial_customer_to_string = g_strdup_printf ("%d", initial_customer);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = afdal_request (connection, __afdal_customer_customer_address_list_process,
				(AfDalFunc)
				usr_function, usr_data, "customer_address_list",
				"initial_customer", COYOTE_XML_ARG_STRING,
				initial_customer_to_string, "max_row_number",
				COYOTE_XML_ARG_STRING, max_row_number_to_string, "address_id",
				COYOTE_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (initial_customer_to_string);
	g_free (max_row_number_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean afdal_customer_customer_address_remove (gint customer_id,
						 gint address_id,
						 AfDalNulFunc usr_function,
						 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_address_remove",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (customer_id_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean afdal_customer_customer_address_add (gint customer_id,
					      gint address_id,
					      AfDalNulFunc usr_function,
					      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *address_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	address_id_to_string = g_strdup_printf ("%d", address_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_address_add",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (customer_id_to_string);
	g_free (address_id_to_string);
	return result;
}

gboolean afdal_customer_customer_telephone_set (gint customer_id,
						gint telephone_id,
						gboolean value_to_set,
						AfDalNulFunc usr_function,
						gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_telephone_set",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				"value_to_set", COYOTE_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (customer_id_to_string);
	g_free (telephone_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean afdal_customer_customer_telephone_update_all (gint telephone_id,
						       AfDalList * setof_customer,
						       AfDalNulFunc usr_function,
						       gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_telephone_update_all",
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				"setof_customer", COYOTE_XML_ARG_STRING, setof_customer, NULL);
	g_free (telephone_id_to_string);
	return result;
}

void __afdal_customer_customer_telephone_list_destroy (gpointer data)
{
	AfDalCustomerCustomer *customer = (AfDalCustomerCustomer *) data;

	g_object_unref (G_OBJECT(customer));
	return;
}

AfDalList          *__afdal_customer_customer_telephone_list_create_afdal_list (CoyoteDataSet *
										data)
{
	AfDalCustomerCustomer *customer;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_customer_telephone_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		customer = afdal_customer_customer_create ();
		customer->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		customer->enum_values_id =
			afdal_support_get_number (coyote_dataset_get (data, i, 1));
		customer->legal_id = g_strdup (coyote_dataset_get (data, i, 2));
		customer->name = g_strdup (coyote_dataset_get (data, i, 3));
		customer->surname = g_strdup (coyote_dataset_get (data, i, 4));
		customer->salesman = g_strdup (coyote_dataset_get (data, i, 5));
		customer->vat_customer_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 6));
		customer->customer_ref = g_strdup (coyote_dataset_get (data, i, 7));
		afdal_list_insert (result, GINT_TO_POINTER (customer->id), customer);
	}
	return result;
}

static void __afdal_customer_customer_telephone_list_process (RRChannel * channel,
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
	afdal_data->data = __afdal_customer_customer_telephone_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_customer_telephone_list (gint initial_customer,
						 gint max_row_number,
						 gint telephone_id,
						 AfDalDataFunc usr_function,
						 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_customer_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	initial_customer_to_string = g_strdup_printf ("%d", initial_customer);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = afdal_request (connection, __afdal_customer_customer_telephone_list_process,
				(AfDalFunc)
				usr_function, usr_data, "customer_telephone_list",
				"initial_customer", COYOTE_XML_ARG_STRING,
				initial_customer_to_string, "max_row_number",
				COYOTE_XML_ARG_STRING, max_row_number_to_string, "telephone_id",
				COYOTE_XML_ARG_STRING, telephone_id_to_string, NULL);
	g_free (initial_customer_to_string);
	g_free (max_row_number_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean afdal_customer_customer_telephone_remove (gint customer_id,
						   gint telephone_id,
						   AfDalNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_telephone_remove",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				NULL);
	g_free (customer_id_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean afdal_customer_customer_telephone_add (gint customer_id,
						gint telephone_id,
						AfDalNulFunc usr_function,
						gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *telephone_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_telephone_add",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				NULL);
	g_free (customer_id_to_string);
	g_free (telephone_id_to_string);
	return result;
}

gboolean afdal_customer_customer_inet_data_set (gint customer_id,
						gint inet_data_id,
						gboolean value_to_set,
						AfDalNulFunc usr_function,
						gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_inet_data_set",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"inet_data_id", COYOTE_XML_ARG_STRING, inet_data_id_to_string,
				"value_to_set", COYOTE_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (customer_id_to_string);
	g_free (inet_data_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean afdal_customer_customer_inet_data_update_all (gint inet_data_id,
						       AfDalList * setof_customer,
						       AfDalNulFunc usr_function,
						       gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_inet_data_update_all",
				"inet_data_id", COYOTE_XML_ARG_STRING, inet_data_id_to_string,
				"setof_customer", COYOTE_XML_ARG_STRING, setof_customer, NULL);
	g_free (inet_data_id_to_string);
	return result;
}

void __afdal_customer_customer_inet_data_list_destroy (gpointer data)
{
	AfDalCustomerCustomer *customer = (AfDalCustomerCustomer *) data;

	g_object_unref (G_OBJECT(customer));
	return;
}

AfDalList          *__afdal_customer_customer_inet_data_list_create_afdal_list (CoyoteDataSet *
										data)
{
	AfDalCustomerCustomer *customer;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_customer_inet_data_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		customer = afdal_customer_customer_create ();
		customer->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		customer->enum_values_id =
			afdal_support_get_number (coyote_dataset_get (data, i, 1));
		customer->legal_id = g_strdup (coyote_dataset_get (data, i, 2));
		customer->name = g_strdup (coyote_dataset_get (data, i, 3));
		customer->surname = g_strdup (coyote_dataset_get (data, i, 4));
		customer->salesman = g_strdup (coyote_dataset_get (data, i, 5));
		customer->vat_customer_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 6));
		customer->customer_ref = g_strdup (coyote_dataset_get (data, i, 7));
		afdal_list_insert (result, GINT_TO_POINTER (customer->id), customer);
	}
	return result;
}

static void __afdal_customer_customer_inet_data_list_process (RRChannel * channel,
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
	afdal_data->data = __afdal_customer_customer_inet_data_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_customer_inet_data_list (gint initial_customer,
						 gint max_row_number,
						 gint inet_data_id,
						 AfDalDataFunc usr_function,
						 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_customer_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	initial_customer_to_string = g_strdup_printf ("%d", initial_customer);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = afdal_request (connection, __afdal_customer_customer_inet_data_list_process,
				(AfDalFunc)
				usr_function, usr_data, "customer_inet_data_list",
				"initial_customer", COYOTE_XML_ARG_STRING,
				initial_customer_to_string, "max_row_number",
				COYOTE_XML_ARG_STRING, max_row_number_to_string, "inet_data_id",
				COYOTE_XML_ARG_STRING, inet_data_id_to_string, NULL);
	g_free (initial_customer_to_string);
	g_free (max_row_number_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean afdal_customer_customer_inet_data_remove (gint customer_id,
						   gint inet_data_id,
						   AfDalNulFunc usr_function,
						   gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_inet_data_remove",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"inet_data_id", COYOTE_XML_ARG_STRING, inet_data_id_to_string,
				NULL);
	g_free (customer_id_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean afdal_customer_customer_inet_data_add (gint customer_id,
						gint inet_data_id,
						AfDalNulFunc usr_function,
						gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *inet_data_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	inet_data_id_to_string = g_strdup_printf ("%d", inet_data_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_inet_data_add",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"inet_data_id", COYOTE_XML_ARG_STRING, inet_data_id_to_string,
				NULL);
	g_free (customer_id_to_string);
	g_free (inet_data_id_to_string);
	return result;
}

gboolean afdal_customer_customer_contact_set (gint customer_id,
					      gint contact_id,
					      gboolean value_to_set,
					      AfDalNulFunc usr_function,
					      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *contact_id_to_string = NULL;
	gchar              *value_to_set_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	value_to_set_to_string = g_strdup_printf ("%d", value_to_set);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_contact_set",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"contact_id", COYOTE_XML_ARG_STRING, contact_id_to_string,
				"value_to_set", COYOTE_XML_ARG_STRING, value_to_set_to_string,
				NULL);
	g_free (customer_id_to_string);
	g_free (contact_id_to_string);
	g_free (value_to_set_to_string);
	return result;
}

gboolean afdal_customer_customer_contact_update_all (gint contact_id,
						     AfDalList * setof_customer,
						     AfDalNulFunc usr_function,
						     gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *contact_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_contact_update_all",
				"contact_id", COYOTE_XML_ARG_STRING, contact_id_to_string,
				"setof_customer", COYOTE_XML_ARG_STRING, setof_customer, NULL);
	g_free (contact_id_to_string);
	return result;
}

void __afdal_customer_customer_contact_list_destroy (gpointer data)
{
	AfDalCustomerCustomer *customer = (AfDalCustomerCustomer *) data;

	g_object_unref (G_OBJECT(customer));
	return;
}

AfDalList          *__afdal_customer_customer_contact_list_create_afdal_list (CoyoteDataSet *
									      data)
{
	AfDalCustomerCustomer *customer;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_customer_contact_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		customer = afdal_customer_customer_create ();
		customer->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		customer->enum_values_id =
			afdal_support_get_number (coyote_dataset_get (data, i, 1));
		customer->legal_id = g_strdup (coyote_dataset_get (data, i, 2));
		customer->name = g_strdup (coyote_dataset_get (data, i, 3));
		customer->surname = g_strdup (coyote_dataset_get (data, i, 4));
		customer->salesman = g_strdup (coyote_dataset_get (data, i, 5));
		customer->vat_customer_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 6));
		customer->customer_ref = g_strdup (coyote_dataset_get (data, i, 7));
		afdal_list_insert (result, GINT_TO_POINTER (customer->id), customer);
	}
	return result;
}

static void __afdal_customer_customer_contact_list_process (RRChannel * channel,
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
	afdal_data->data = __afdal_customer_customer_contact_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_customer_contact_list (gint initial_customer,
					       gint max_row_number,
					       gint contact_id,
					       AfDalDataFunc usr_function,
					       gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *initial_customer_to_string = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *contact_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	initial_customer_to_string = g_strdup_printf ("%d", initial_customer);
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	result = afdal_request (connection, __afdal_customer_customer_contact_list_process,
				(AfDalFunc)
				usr_function, usr_data, "customer_contact_list",
				"initial_customer", COYOTE_XML_ARG_STRING,
				initial_customer_to_string, "max_row_number",
				COYOTE_XML_ARG_STRING, max_row_number_to_string, "contact_id",
				COYOTE_XML_ARG_STRING, contact_id_to_string, NULL);
	g_free (initial_customer_to_string);
	g_free (max_row_number_to_string);
	g_free (contact_id_to_string);
	return result;
}

gboolean afdal_customer_customer_contact_remove (gint customer_id,
						 gint contact_id,
						 AfDalNulFunc usr_function,
						 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *contact_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_contact_remove",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"contact_id", COYOTE_XML_ARG_STRING, contact_id_to_string, NULL);
	g_free (customer_id_to_string);
	g_free (contact_id_to_string);
	return result;
}

gboolean afdal_customer_customer_contact_add (gint customer_id,
					      gint contact_id,
					      AfDalNulFunc usr_function,
					      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *contact_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_contact_add",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"contact_id", COYOTE_XML_ARG_STRING, contact_id_to_string, NULL);
	g_free (customer_id_to_string);
	g_free (contact_id_to_string);
	return result;
}

gboolean afdal_customer_customer_new (gchar * legal_id,
				      gchar * name,
				      gchar * surname,
				      gchar * salesman,
				      gint vat_customer_type,
				      gchar * customer_ref,
				      gint enum_values_id,
				      AfDalSimpleFunc usr_function,
				      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *vat_customer_type_to_string = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	vat_customer_type_to_string = g_strdup_printf ("%d", vat_customer_type);
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	result = afdal_request (connection, afdal_request_process_simple_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_new",
				"legal_id", COYOTE_XML_ARG_STRING, legal_id, "name",
				COYOTE_XML_ARG_STRING, name, "surname", COYOTE_XML_ARG_STRING,
				surname, "salesman", COYOTE_XML_ARG_STRING, salesman,
				"vat_customer_type", COYOTE_XML_ARG_STRING,
				vat_customer_type_to_string, "customer_ref",
				COYOTE_XML_ARG_STRING, customer_ref, "enum_values_id",
				COYOTE_XML_ARG_STRING, enum_values_id_to_string, NULL);
	g_free (vat_customer_type_to_string);
	g_free (enum_values_id_to_string);
	return result;
}

gboolean afdal_customer_customer_remove (gint customer_id,
					 AfDalNulFunc usr_function,
					 gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_remove",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				NULL);
	g_free (customer_id_to_string);
	return result;
}

gboolean afdal_customer_customer_edit (gint customer_id,
				       gchar * new_legal_id,
				       gchar * new_name,
				       gchar * new_surname,
				       gchar * new_salesman,
				       gint new_vat_customer_type,
				       gchar * new_customer_ref,
				       gint new_enum_values_id,
				       AfDalNulFunc usr_function,
				       gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *new_vat_customer_type_to_string = NULL;
	gchar              *new_enum_values_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	new_vat_customer_type_to_string = g_strdup_printf ("%d", new_vat_customer_type);
	new_enum_values_id_to_string = g_strdup_printf ("%d", new_enum_values_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"customer_edit",
				"customer_id", COYOTE_XML_ARG_STRING, customer_id_to_string,
				"new_legal_id", COYOTE_XML_ARG_STRING, new_legal_id, "new_name",
				COYOTE_XML_ARG_STRING, new_name, "new_surname",
				COYOTE_XML_ARG_STRING, new_surname, "new_salesman",
				COYOTE_XML_ARG_STRING, new_salesman, "new_vat_customer_type",
				COYOTE_XML_ARG_STRING, new_vat_customer_type_to_string,
				"new_customer_ref", COYOTE_XML_ARG_STRING, new_customer_ref,
				"new_enum_values_id", COYOTE_XML_ARG_STRING,
				new_enum_values_id_to_string, NULL);
	g_free (customer_id_to_string);
	g_free (new_vat_customer_type_to_string);
	g_free (new_enum_values_id_to_string);
	return result;
}

void __afdal_customer_customer_list_destroy (gpointer data)
{
	AfDalCustomerCustomer *customer = (AfDalCustomerCustomer *) data;

	g_object_unref (G_OBJECT(customer));
	return;
}

AfDalList          *__afdal_customer_customer_list_create_afdal_list (CoyoteDataSet * data)
{
	AfDalCustomerCustomer *customer;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_customer_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		customer = afdal_customer_customer_create ();
		customer->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		customer->enum_values_id =
			afdal_support_get_number (coyote_dataset_get (data, i, 7));
		customer->legal_id = g_strdup (coyote_dataset_get (data, i, 1));
		customer->name = g_strdup (coyote_dataset_get (data, i, 2));
		customer->surname = g_strdup (coyote_dataset_get (data, i, 3));
		customer->salesman = g_strdup (coyote_dataset_get (data, i, 4));
		customer->vat_customer_type =
			afdal_support_get_number (coyote_dataset_get (data, i, 5));
		customer->customer_ref = g_strdup (coyote_dataset_get (data, i, 6));
		afdal_list_insert (result, GINT_TO_POINTER (customer->id), customer);
	}
	return result;
}

static void __afdal_customer_customer_list_process (RRChannel * channel,
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
	afdal_data->data = __afdal_customer_customer_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_customer_list (gint max_row_number,
				       gint initial_customer,
				       AfDalDataFunc usr_function,
				       gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *initial_customer_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af-customer", NULL);
	if (!connection)
		return FALSE;
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	initial_customer_to_string = g_strdup_printf ("%d", initial_customer);
	result = afdal_request (connection, __afdal_customer_customer_list_process, (AfDalFunc)
				usr_function, usr_data,
				"af_customer_customer_list",
				"initial", COYOTE_XML_ARG_STRING, initial_customer_to_string,
				"row_number", COYOTE_XML_ARG_STRING, max_row_number_to_string,
				NULL);
	g_free (max_row_number_to_string);
	g_free (initial_customer_to_string);
	return result;
}
