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

#include "aos_customer_contact.h"

gboolean aos_customer_contact_new (gchar * name,
				     gchar * position,
				     gint customer_id,
				     OseaClientSimpleFunc usr_function,
				     gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *customer_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	customer_id_to_string = g_strdup_printf ("%d", customer_id);
	result = oseaclient_request (connection, oseaclient_request_process_simple_data, (OseaClientFunc)
				usr_function, usr_data,
				"contact_new",
				"name", OSEACOMM_XML_ARG_STRING, name, "position",
				OSEACOMM_XML_ARG_STRING, position, "customer_id",
				OSEACOMM_XML_ARG_STRING, customer_id_to_string, NULL);
	g_free (customer_id_to_string);
	return result;
}

gboolean aos_customer_contact_remove (gint contact_id,
					OseaClientNulFunc usr_function,
					gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *contact_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"contact_remove",
				"contact_id", OSEACOMM_XML_ARG_STRING, contact_id_to_string, NULL);
	g_free (contact_id_to_string);
	return result;
}

gboolean aos_customer_contact_edit (gint contact_id,
				      gchar * new_name,
				      gchar * new_position,
				      gint new_customer_id,
				      OseaClientNulFunc usr_function,
				      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *contact_id_to_string = NULL;
	gchar              *new_customer_id_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	contact_id_to_string = g_strdup_printf ("%d", contact_id);
	new_customer_id_to_string = g_strdup_printf ("%d", new_customer_id);
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc)
				usr_function, usr_data,
				"contact_edit",
				"contact_id", OSEACOMM_XML_ARG_STRING, contact_id_to_string,
				"new_name", OSEACOMM_XML_ARG_STRING, new_name, "new_position",
				OSEACOMM_XML_ARG_STRING, new_position, "new_customer_id",
				OSEACOMM_XML_ARG_STRING, new_customer_id_to_string, NULL);
	g_free (contact_id_to_string);
	g_free (new_customer_id_to_string);
	return result;
}

void __aos_customer_contact_list_destroy (gpointer data)
{
	AosCustomerContact *contact = (AosCustomerContact *) data;

	g_free (contact->name);
	g_free (contact->position);
	g_free (contact);
	return;
}

OseaClientList          *__aos_customer_contact_list_create_oseaclient_list (OseaCommDataSet * data)
{
	AosCustomerContact *contact;
	OseaClientList          *result;
	gint                i;

	result = oseaclient_list_new_full (oseaclient_support_compare_id, NULL,
				      __aos_customer_contact_list_destroy);
	for (i = 0; i < oseacomm_dataset_get_height (data); i++) {
		contact = g_new0 (AosCustomerContact, 1);
		contact->id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 0));
		contact->customer_id = oseaclient_support_get_number (oseacomm_dataset_get (data, i, 1));
		contact->name = g_strdup (oseacomm_dataset_get (data, i, 2));
		contact->position = g_strdup (oseacomm_dataset_get (data, i, 3));
		oseaclient_list_insert (result, GINT_TO_POINTER (contact->id), contact);
	}
	return result;
}

static void __aos_customer_contact_list_process (RRChannel * channel,
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
	oseaclient_data->data = __aos_customer_contact_list_create_oseaclient_list (dataset);
	// Call to user defined callback
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);
	return;
}

gboolean aos_customer_contact_list (gint max_row_number,
				      gint initial_contact,
				      OseaClientDataFunc usr_function,
				      gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *initial_contact_to_string = NULL;
	gboolean            result;

	connection = oseaclient_session_get_connection ("os_customer", NULL);
	if (!connection)
		return FALSE;
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	initial_contact_to_string = g_strdup_printf ("%d", initial_contact);
	result = oseaclient_request (connection, __aos_customer_contact_list_process, (OseaClientFunc)
				usr_function, usr_data,
				"contact_list",
				"max_row_number", OSEACOMM_XML_ARG_STRING, max_row_number_to_string,
				"initial_contact", OSEACOMM_XML_ARG_STRING,
				initial_contact_to_string, NULL);
	g_free (max_row_number_to_string);
	g_free (initial_contact_to_string);
	return result;
}
