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

#include "afdal_customer_address.h"

gboolean afdal_customer_address_new (gchar * address,
				     gchar * city,
				     gchar * state,
				     gchar * zip_code,
				     gchar * country,
				     gint customer_id,
				     gint enum_values_id,
				     AfDalSimpleFunc usr_function,
				     gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gchar              *enum_values_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	enum_values_id_to_string = g_strdup_printf ("%d", enum_values_id);
	result = afdal_request (connection, afdal_request_process_simple_data, (AfDalFunc)
				usr_function, usr_data,
				"address_new",
				"address", COYOTE_XML_ARG_STRING, address, "city",
				COYOTE_XML_ARG_STRING, city, "state", COYOTE_XML_ARG_STRING,
				state, "zip_code", COYOTE_XML_ARG_STRING, zip_code, "country",
				COYOTE_XML_ARG_STRING, country, "customer_id",
				COYOTE_XML_ARG_STRING, customer_id_to_string, "enum_values_id",
				COYOTE_XML_ARG_STRING, enum_values_id_to_string, NULL);
	g_free (customer_id_to_string);
	g_free (enum_values_id_to_string);
	return result;
}

gboolean afdal_customer_address_remove (gint address_id,
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
				"address_remove",
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string, NULL);
	g_free (address_id_to_string);
	return result;
}

gboolean afdal_customer_address_edit (gint address_id,
				      gchar * new_address,
				      gchar * new_city,
				      gchar * new_state,
				      gchar * new_zip_code,
				      gchar * new_country,
				      gint new_customer_id,
				      gint new_enum_values_id,
				      AfDalNulFunc usr_function,
				      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *address_id_to_string = NULL;
	gchar              *new_customer_id_to_string = NULL;
	gchar              *new_enum_values_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	address_id_to_string = g_strdup_printf ("%d", address_id);
	new_customer_id_to_string = g_strdup_printf ("%d", new_customer_id);
	new_enum_values_id_to_string = g_strdup_printf ("%d", new_enum_values_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"address_edit",
				"address_id", COYOTE_XML_ARG_STRING, address_id_to_string,
				"new_address", COYOTE_XML_ARG_STRING, new_address, "new_city",
				COYOTE_XML_ARG_STRING, new_city, "new_state",
				COYOTE_XML_ARG_STRING, new_state, "new_zip_code",
				COYOTE_XML_ARG_STRING, new_zip_code, "new_country",
				COYOTE_XML_ARG_STRING, new_country, "new_customer_id",
				COYOTE_XML_ARG_STRING, new_customer_id_to_string,
				"new_enum_values_id", COYOTE_XML_ARG_STRING,
				new_enum_values_id_to_string, NULL);
	g_free (address_id_to_string);
	g_free (new_customer_id_to_string);
	g_free (new_enum_values_id_to_string);
	return result;
}

void __afdal_customer_address_list_destroy (gpointer data)
{
	AfDalCustomerAddress *address = (AfDalCustomerAddress *) data;

	g_free (address->address);
	g_free (address->city);
	g_free (address->state);
	g_free (address->zip_code);
	g_free (address->country);
	g_free (address);
	return;
}

AfDalList          *__afdal_customer_address_list_create_afdal_list (CoyoteDataSet * data)
{
	AfDalCustomerAddress *address;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_address_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		address = g_new0 (AfDalCustomerAddress, 1);
		address->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		address->enum_values_id =
			afdal_support_get_number (coyote_dataset_get (data, i, 1));
		address->customer_id = afdal_support_get_number (coyote_dataset_get (data, i, 2));
		address->address = g_strdup (coyote_dataset_get (data, i, 3));
		address->city = g_strdup (coyote_dataset_get (data, i, 4));
		address->state = g_strdup (coyote_dataset_get (data, i, 5));
		address->zip_code = g_strdup (coyote_dataset_get (data, i, 6));
		address->country = g_strdup (coyote_dataset_get (data, i, 7));
		afdal_list_insert (result, GINT_TO_POINTER (address->id), address);
	}
	return result;
}

static void __afdal_customer_address_list_process (RRChannel * channel,
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
	afdal_data->data = __afdal_customer_address_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_address_list (gint max_row_number,
				      gint initial_address,
				      AfDalDataFunc usr_function,
				      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *initial_address_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	initial_address_to_string = g_strdup_printf ("%d", initial_address);
	result = afdal_request (connection, __afdal_customer_address_list_process, (AfDalFunc)
				usr_function, usr_data,
				"address_list",
				"max_row_number", COYOTE_XML_ARG_STRING, max_row_number_to_string,
				"initial_address", COYOTE_XML_ARG_STRING,
				initial_address_to_string, NULL);
	g_free (max_row_number_to_string);
	g_free (initial_address_to_string);
	return result;
}
