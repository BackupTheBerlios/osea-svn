//  os_customer: Daemon for the server-side
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

#include "os_customer_customer.h"
#include <liboseaserver/oseaserver.h>
#include <liboseacomm/oseacomm.h>
gboolean os_customer_customer_address_set (OseaCommXmlServiceData * data,
					   gpointer customer_data,
					   RRChannel * channel,
					   gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "address_id", "", "customer_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_customer_address_update_all (OseaCommXmlServiceData * data,
						  gpointer customer_data,
						  RRChannel * channel,
						  gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "address_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_customer_address_list (OseaCommXmlServiceData * data,
					    gpointer customer_data,
					    RRChannel * channel,
					    gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_customer", "max_row_number",
					    "address_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Check out request information
	if (!g_strcasecmp ("0", g_list_nth_data (values, 1)))
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer, os_customer_located WHERE id = customer_id AND address_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer, os_customer_located WHERE id = customer_id AND address_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "customer elements which has address",
				OSEACOMM_CODE_OK, response, NULL);

	return TRUE;



}

gboolean os_customer_customer_address_remove (OseaCommXmlServiceData * data,
					      gpointer customer_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "address_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add address to customer 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_located (customer_id, address_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to add address to customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "address added to customer", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_address_add (OseaCommXmlServiceData * data,
					   gpointer customer_data,
					   RRChannel * channel,
					   gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "address_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add address to customer 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_located (customer_id, address_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to add address to customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "address added to customer", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_telephone_set (OseaCommXmlServiceData * data,
					     gpointer customer_data,
					     RRChannel * channel,
					     gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "telephone_id", "", "customer_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_customer_telephone_update_all (OseaCommXmlServiceData * data,
						    gpointer customer_data,
						    RRChannel * channel,
						    gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "telephone_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_customer_telephone_list (OseaCommXmlServiceData * data,
					      gpointer customer_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_customer", "max_row_number",
					    "telephone_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Check out request information
	if (!g_strcasecmp ("0", g_list_nth_data (values, 1)))
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer, os_customer_has_phone WHERE id = customer_id AND telephone_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer, os_customer_has_phone WHERE id = customer_id AND telephone_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "customer elements which has telephone",
				OSEACOMM_CODE_OK, response, NULL);

	return TRUE;



}

gboolean os_customer_customer_telephone_remove (OseaCommXmlServiceData * data,
						gpointer customer_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "telephone_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add telephone to customer 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_has_phone (customer_id, telephone_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to add telephone to customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "telephone added to customer", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_telephone_add (OseaCommXmlServiceData * data,
					     gpointer customer_data,
					     RRChannel * channel,
					     gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "telephone_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add telephone to customer 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_has_phone (customer_id, telephone_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to add telephone to customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "telephone added to customer", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_inet_data_set (OseaCommXmlServiceData * data,
					     gpointer customer_data,
					     RRChannel * channel,
					     gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "inet_data_id", "", "customer_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_customer_inet_data_update_all (OseaCommXmlServiceData * data,
						    gpointer customer_data,
						    RRChannel * channel,
						    gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "inet_data_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_customer_inet_data_list (OseaCommXmlServiceData * data,
					      gpointer customer_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_customer", "max_row_number",
					    "inet_data_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Check out request information
	if (!g_strcasecmp ("0", g_list_nth_data (values, 1)))
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer, os_customer_has_web_data WHERE id = customer_id AND inet_data_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer, os_customer_has_web_data WHERE id = customer_id AND inet_data_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "customer elements which has inet_data",
				OSEACOMM_CODE_OK, response, NULL);

	return TRUE;



}

gboolean os_customer_customer_inet_data_remove (OseaCommXmlServiceData * data,
						gpointer customer_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "inet_data_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add inet_data to customer 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_has_web_data (customer_id, inet_data_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to add inet_data to customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "inet_data added to customer", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_inet_data_add (OseaCommXmlServiceData * data,
					     gpointer customer_data,
					     RRChannel * channel,
					     gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "inet_data_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add inet_data to customer 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_has_web_data (customer_id, inet_data_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to add inet_data to customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "inet_data added to customer", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_contact_set (OseaCommXmlServiceData * data,
					   gpointer customer_data,
					   RRChannel * channel,
					   gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "contact_id", "", "customer_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_customer_contact_update_all (OseaCommXmlServiceData * data,
						  gpointer customer_data,
						  RRChannel * channel,
						  gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "contact_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_customer_contact_list (OseaCommXmlServiceData * data,
					    gpointer customer_data,
					    RRChannel * channel,
					    gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_customer", "max_row_number",
					    "contact_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Check out request information
	if (!g_strcasecmp ("0", g_list_nth_data (values, 1)))
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer, os_customer_contact_with WHERE id = customer_id AND contact_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer, os_customer_contact_with WHERE id = customer_id AND contact_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "customer elements which has contact",
				OSEACOMM_CODE_OK, response, NULL);

	return TRUE;



}

gboolean os_customer_customer_contact_remove (OseaCommXmlServiceData * data,
					      gpointer customer_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "contact_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add contact to customer 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_contact_with (customer_id, contact_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to add contact to customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "contact added to customer", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_contact_add (OseaCommXmlServiceData * data,
					   gpointer customer_data,
					   RRChannel * channel,
					   gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "contact_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add contact to customer 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_contact_with (customer_id, contact_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to add contact to customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "contact added to customer", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_new (OseaCommXmlServiceData * data,
				   gpointer user_data,
				   RRChannel * channel,
				   gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *customer_id;
	gboolean            result = FALSE;

	// Check for correct params
	values = oseaserver_message_check_params (data, "legal_id", "name", "surname", "salesman",
					    "vat_customer_type", "customer_ref", "enum_values_id",
					    NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Creates the new customer
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_customer (legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id) VALUES ('%s', '%s', '%s', '%s', %s, '%s', %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2), g_list_nth_data (values, 3),
		 g_list_nth_data (values, 4), g_list_nth_data (values, 5),
		 g_list_nth_data (values, 6));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to create new customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	// Check out the previously created customer identifier
	customer_id =
		oseaserver_command_execute_single_query
		("SELECT id FROM os_customer_customer WHERE legal_id = '%s' AND name = '%s' AND surname = '%s' AND salesman = '%s' AND vat_customer_type = %s AND customer_ref = '%s' AND enum_values_id = %s",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2), g_list_nth_data (values, 3),
		 g_list_nth_data (values, 4), g_list_nth_data (values, 5),
		 g_list_nth_data (values, 6));
	oseaserver_message_ok_answer (channel, msg_no, "customer created", OSEACOMM_CODE_OK, customer_id,
				NULL);

	return TRUE;
}

gboolean os_customer_customer_remove (OseaCommXmlServiceData * data,
				      gpointer user_data,
				      RRChannel * channel,
				      gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;

	// Check for correct params 
	values = oseaserver_message_check_params (data, "id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Delete customer
	result = oseaserver_command_execute_non_query ("DELETE FROM os_customer_customer WHERE id = %s",
						 g_list_nth_data (values, 0));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to delete customer",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	oseaserver_message_ok_answer (channel, msg_no, "customer deleted", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}

gboolean os_customer_customer_edit (OseaCommXmlServiceData * data,
				    gpointer user_data,
				    RRChannel * channel,
				    gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "new_legal_id", "new_name",
					    "new_surname", "new_salesman",
					    "new_vat_customer_type", "new_customer_ref",
					    "new_enum_values_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Update customer values
	result = oseaserver_command_execute_non_query
		("UPDATE os_customer_customer SET legal_id = '%s' , name = '%s' , surname = '%s' , salesman = '%s' , vat_customer_type = %s , customer_ref = '%s' , enum_values_id = %s WHERE id = %s",
		 g_list_nth_data (values, 1), g_list_nth_data (values, 2),
		 g_list_nth_data (values, 3), g_list_nth_data (values, 4),
		 g_list_nth_data (values, 5), g_list_nth_data (values, 6),
		 g_list_nth_data (values, 7), g_list_nth_data (values, 0));
	oseaserver_message_ok_answer (channel, msg_no, "customer successfully edited", OSEACOMM_CODE_OK,
				result, NULL);

	return TRUE;
}

gboolean os_customer_customer_list (OseaCommXmlServiceData * data,
				    gpointer user_data,
				    RRChannel * channel,
				    gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;
	gint                initial, row_number;
	gchar              *aux;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial", "row_number", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Convert initial and row_number to interger
	initial = strtol (g_list_nth_data (values, 0), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Invalid format for initial parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Invalid format for row_number parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	// Check out request information
	if (row_number == 0)
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer WHERE id >= %d",
			 initial);
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM os_customer_customer WHERE id >= %d LIMIT %d",
			 initial, row_number);
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available customers", OSEACOMM_CODE_OK, response,
				NULL);

	return TRUE;

}
