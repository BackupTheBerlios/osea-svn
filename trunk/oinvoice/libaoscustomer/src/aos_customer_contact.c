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

#include "afdal_customer_contact.h"

gboolean afdal_customer_contact_new (gchar * name,
				     gchar * position,
				     gint customer_id,
				     AfDalSimpleFunc usr_function,
				     gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = afdal_request (connection, afdal_request_process_simple_data, (AfDalFunc)
				usr_function, usr_data,
				"contact_new",
				"name", COYOTE_XML_ARG_STRING, name, "position",
				COYOTE_XML_ARG_STRING, position, "customer_id",
				COYOTE_XML_ARG_STRING, customer_id_to_string, NULL);
	g_free (customer_id_to_string);
	return result;
}

gboolean afdal_customer_contact_remove (gint contact_id,
					AfDalNulFunc usr_function,
					gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *contact_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"contact_remove",
				"contact_id", COYOTE_XML_ARG_STRING, contact_id_to_string, NULL);
	g_free (contact_id_to_string);
	return result;
}

gboolean afdal_customer_contact_edit (gint contact_id,
				      gchar * new_name,
				      gchar * new_position,
				      gint new_customer_id,
				      AfDalNulFunc usr_function,
				      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *contact_id_to_string = NULL;
	gchar              *new_customer_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	new_customer_id_to_string = g_strdup_printf ("%d", new_customer_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"contact_edit",
				"contact_id", COYOTE_XML_ARG_STRING, contact_id_to_string,
				"new_name", COYOTE_XML_ARG_STRING, new_name, "new_position",
				COYOTE_XML_ARG_STRING, new_position, "new_customer_id",
				COYOTE_XML_ARG_STRING, new_customer_id_to_string, NULL);
	g_free (contact_id_to_string);
	g_free (new_customer_id_to_string);
	return result;
}

void __afdal_customer_contact_list_destroy (gpointer data)
{
	AfDalCustomerContact *contact = (AfDalCustomerContact *) data;

	g_free (contact->name);
	g_free (contact->position);
	g_free (contact);
	return;
}

AfDalList          *__afdal_customer_contact_list_create_afdal_list (CoyoteDataSet * data)
{
	AfDalCustomerContact *contact;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_contact_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		contact = g_new0 (AfDalCustomerContact, 1);
		contact->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		contact->customer_id = afdal_support_get_number (coyote_dataset_get (data, i, 1));
		contact->name = g_strdup (coyote_dataset_get (data, i, 2));
		contact->position = g_strdup (coyote_dataset_get (data, i, 3));
		afdal_list_insert (result, GINT_TO_POINTER (contact->id), contact);
	}
	return result;
}

static void __afdal_customer_contact_list_process (RRChannel * channel,
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
	afdal_data->data = __afdal_customer_contact_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_contact_list (gint max_row_number,
				      gint initial_contact,
				      AfDalDataFunc usr_function,
				      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *initial_contact_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	initial_contact_to_string = g_strdup_printf ("%d", initial_contact);
	result = afdal_request (connection, __afdal_customer_contact_list_process, (AfDalFunc)
				usr_function, usr_data,
				"contact_list",
				"max_row_number", COYOTE_XML_ARG_STRING, max_row_number_to_string,
				"initial_contact", COYOTE_XML_ARG_STRING,
				initial_contact_to_string, NULL);
	g_free (max_row_number_to_string);
	g_free (initial_contact_to_string);
	return result;
}
