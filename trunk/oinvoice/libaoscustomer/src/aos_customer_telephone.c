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

#include "afdal_customer_telephone.h"

gboolean afdal_customer_telephone_new (gchar * phone,
				       gchar * description,
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
				"telephone_new",
				"phone", COYOTE_XML_ARG_STRING, phone, "description",
				COYOTE_XML_ARG_STRING, description, "customer_id",
				COYOTE_XML_ARG_STRING, customer_id_to_string, "enum_values_id",
				COYOTE_XML_ARG_STRING, enum_values_id_to_string, NULL);
	g_free (customer_id_to_string);
	g_free (enum_values_id_to_string);
	return result;
}

gboolean afdal_customer_telephone_remove (gint telephone_id,
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
				"telephone_remove",
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				NULL);
	g_free (telephone_id_to_string);
	return result;
}

gboolean afdal_customer_telephone_edit (gint telephone_id,
					gchar * new_phone,
					gchar * new_description,
					gint new_customer_id,
					gint new_enum_values_id,
					AfDalNulFunc usr_function,
					gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *telephone_id_to_string = NULL;
	gchar              *new_customer_id_to_string = NULL;
	gchar              *new_enum_values_id_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	telephone_id_to_string = g_strdup_printf ("%d", telephone_id);
	new_customer_id_to_string = g_strdup_printf ("%d", new_customer_id);
	new_enum_values_id_to_string = g_strdup_printf ("%d", new_enum_values_id);
	result = afdal_request (connection, afdal_request_process_nul_data, (AfDalFunc)
				usr_function, usr_data,
				"telephone_edit",
				"telephone_id", COYOTE_XML_ARG_STRING, telephone_id_to_string,
				"new_phone", COYOTE_XML_ARG_STRING, new_phone, "new_description",
				COYOTE_XML_ARG_STRING, new_description, "new_customer_id",
				COYOTE_XML_ARG_STRING, new_customer_id_to_string,
				"new_enum_values_id", COYOTE_XML_ARG_STRING,
				new_enum_values_id_to_string, NULL);
	g_free (telephone_id_to_string);
	g_free (new_customer_id_to_string);
	g_free (new_enum_values_id_to_string);
	return result;
}

void __afdal_customer_telephone_list_destroy (gpointer data)
{
	AfDalCustomerTelephone *telephone = (AfDalCustomerTelephone *) data;

	g_free (telephone->phone);
	g_free (telephone->description);
	g_free (telephone);
	return;
}

AfDalList          *__afdal_customer_telephone_list_create_afdal_list (CoyoteDataSet * data)
{
	AfDalCustomerTelephone *telephone;
	AfDalList          *result;
	gint                i;

	result = afdal_list_new_full (afdal_support_compare_id, NULL,
				      __afdal_customer_telephone_list_destroy);
	for (i = 0; i < coyote_dataset_get_height (data); i++) {
		telephone = g_new0 (AfDalCustomerTelephone, 1);
		telephone->id = afdal_support_get_number (coyote_dataset_get (data, i, 0));
		telephone->enum_values_id =
			afdal_support_get_number (coyote_dataset_get (data, i, 1));
		telephone->customer_id =
			afdal_support_get_number (coyote_dataset_get (data, i, 2));
		telephone->phone = g_strdup (coyote_dataset_get (data, i, 3));
		telephone->description = g_strdup (coyote_dataset_get (data, i, 4));
		afdal_list_insert (result, GINT_TO_POINTER (telephone->id), telephone);
	}
	return result;
}

static void __afdal_customer_telephone_list_process (RRChannel * channel,
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
	afdal_data->data = __afdal_customer_telephone_list_create_afdal_list (dataset);
	// Call to user defined callback
	afdal_request_call_user_function (AFDAL_REQUEST_DATA, data, custom_data, afdal_data);
	return;
}

gboolean afdal_customer_telephone_list (gint max_row_number,
					gint initial_telephone,
					AfDalDataFunc usr_function,
					gpointer usr_data)
{
	RRConnection       *connection = NULL;
	gchar              *max_row_number_to_string = NULL;
	gchar              *initial_telephone_to_string = NULL;
	gboolean            result;

	connection = afdal_session_get_connection ("af_customer", NULL);
	if (!connection)
		return FALSE;
	max_row_number_to_string = g_strdup_printf ("%d", max_row_number);
	initial_telephone_to_string = g_strdup_printf ("%d", initial_telephone);
	result = afdal_request (connection, __afdal_customer_telephone_list_process, (AfDalFunc)
				usr_function, usr_data,
				"telephone_list",
				"max_row_number", COYOTE_XML_ARG_STRING, max_row_number_to_string,
				"initial_telephone", COYOTE_XML_ARG_STRING,
				initial_telephone_to_string, NULL);
	g_free (max_row_number_to_string);
	g_free (initial_telephone_to_string);
	return result;
}
