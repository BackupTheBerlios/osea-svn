//  os_customer: Daemon for the server-side
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

#include "os_customer_enum_values.h"
#include <liboseaserver/oseaserver.h>
#include <liboseacomm/oseacomm.h>
gboolean os_customer_enum_values_address_set (OseaCommXmlServiceData * data,
					      gpointer enum_values_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "address_id", "", "enum_values_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_enum_values_address_update_all (OseaCommXmlServiceData * data,
						     gpointer enum_values_data,
						     RRChannel * channel,
						     gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "address_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_enum_values_address_list (OseaCommXmlServiceData * data,
					       gpointer enum_values_data,
					       RRChannel * channel,
					       gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_enum_values", "max_row_number",
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
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values, os_customer_address_type WHERE id = enum_values_id AND address_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values, os_customer_address_type WHERE id = enum_values_id AND address_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "enum_values elements which has address",
				OSEACOMM_CODE_OK, response, NULL);

	return TRUE;



}

gboolean os_customer_enum_values_address_remove (OseaCommXmlServiceData * data,
						 gpointer enum_values_data,
						 RRChannel * channel,
						 gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "address_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add address to enum_values 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_address_type (enum_values_id, address_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Unable to add address to enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "address added to enum_values", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_enum_values_address_add (OseaCommXmlServiceData * data,
					      gpointer enum_values_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "address_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add address to enum_values 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_address_type (enum_values_id, address_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Unable to add address to enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "address added to enum_values", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_enum_values_telephone_set (OseaCommXmlServiceData * data,
						gpointer enum_values_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "telephone_id", "", "enum_values_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_enum_values_telephone_update_all (OseaCommXmlServiceData * data,
						       gpointer enum_values_data,
						       RRChannel * channel,
						       gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "telephone_id",
					    "value_to_set", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_enum_values_telephone_list (OseaCommXmlServiceData * data,
						 gpointer enum_values_data,
						 RRChannel * channel,
						 gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_enum_values", "max_row_number",
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
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values, os_customer_phone_type WHERE id = enum_values_id AND telephone_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values, os_customer_phone_type WHERE id = enum_values_id AND telephone_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "enum_values elements which has telephone",
				OSEACOMM_CODE_OK, response, NULL);

	return TRUE;



}

gboolean os_customer_enum_values_telephone_remove (OseaCommXmlServiceData * data,
						   gpointer enum_values_data,
						   RRChannel * channel,
						   gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "telephone_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add telephone to enum_values 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_phone_type (enum_values_id, telephone_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Unable to add telephone to enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "telephone added to enum_values", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_enum_values_telephone_add (OseaCommXmlServiceData * data,
						gpointer enum_values_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "telephone_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add telephone to enum_values 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_phone_type (enum_values_id, telephone_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Unable to add telephone to enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "telephone added to enum_values", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_enum_values_inet_data_set (OseaCommXmlServiceData * data,
						gpointer enum_values_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "inet_data_id", "", "enum_values_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_enum_values_inet_data_update_all (OseaCommXmlServiceData * data,
						       gpointer enum_values_data,
						       RRChannel * channel,
						       gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "inet_data_id",
					    "value_to_set", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_enum_values_inet_data_list (OseaCommXmlServiceData * data,
						 gpointer enum_values_data,
						 RRChannel * channel,
						 gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_enum_values", "max_row_number",
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
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values, os_customer_web_data_type WHERE id = enum_values_id AND inet_data_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values, os_customer_web_data_type WHERE id = enum_values_id AND inet_data_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "enum_values elements which has inet_data",
				OSEACOMM_CODE_OK, response, NULL);

	return TRUE;



}

gboolean os_customer_enum_values_inet_data_remove (OseaCommXmlServiceData * data,
						   gpointer enum_values_data,
						   RRChannel * channel,
						   gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "inet_data_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add inet_data to enum_values 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_web_data_type (enum_values_id, inet_data_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Unable to add inet_data to enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "inet_data added to enum_values", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_enum_values_inet_data_add (OseaCommXmlServiceData * data,
						gpointer enum_values_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "inet_data_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add inet_data to enum_values 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_web_data_type (enum_values_id, inet_data_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Unable to add inet_data to enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "inet_data added to enum_values", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_enum_values_customer_set (OseaCommXmlServiceData * data,
					       gpointer enum_values_data,
					       RRChannel * channel,
					       gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "customer_id", "", "enum_values_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_enum_values_customer_update_all (OseaCommXmlServiceData * data,
						      gpointer enum_values_data,
						      RRChannel * channel,
						      gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "customer_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean os_customer_enum_values_customer_list (OseaCommXmlServiceData * data,
						gpointer enum_values_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *response;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_enum_values", "max_row_number",
					    "customer_id", NULL);
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
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values, os_customer_legal_id_type WHERE id = enum_values_id AND customer_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values, os_customer_legal_id_type WHERE id = enum_values_id AND customer_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "enum_values elements which has customer",
				OSEACOMM_CODE_OK, response, NULL);

	return TRUE;



}

gboolean os_customer_enum_values_customer_remove (OseaCommXmlServiceData * data,
						  gpointer enum_values_data,
						  RRChannel * channel,
						  gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "customer_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add customer to enum_values 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_legal_id_type (enum_values_id, customer_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Unable to add customer to enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "customer added to enum_values", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_enum_values_customer_add (OseaCommXmlServiceData * data,
					       gpointer enum_values_data,
					       RRChannel * channel,
					       gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "customer_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add customer to enum_values 
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_legal_id_type (enum_values_id, customer_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no,
					   "Unable to add customer to enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_message_ok_answer (channel, msg_no, "customer added to enum_values", OSEACOMM_CODE_OK,
				NULL);

	return TRUE;
}

gboolean os_customer_enum_values_new (OseaCommXmlServiceData * data,
				      gpointer user_data,
				      RRChannel * channel,
				      gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *enum_values_id;
	gboolean            result = FALSE;

	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_type", "enum_order_in_type", "enum_value",
					    NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Creates the new enum_values
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_enum_values (enum_type,enum_order_in_type,enum_value) VALUES ('%s', %s, '%s')",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to create new enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	// Check out the previously created enum_values identifier
	enum_values_id =
		oseaserver_command_execute_single_query
		("SELECT id FROM os_customer_enum_values WHERE enum_type = '%s' AND enum_order_in_type = %s AND enum_value = '%s'",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2));
	oseaserver_message_ok_answer (channel, msg_no, "enum_values created", OSEACOMM_CODE_OK,
				enum_values_id, NULL);

	return TRUE;
}

gboolean os_customer_enum_values_remove (OseaCommXmlServiceData * data,
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
	// Delete enum_values
	result = oseaserver_command_execute_non_query
		("DELETE FROM os_customer_enum_values WHERE id = %s",
		 g_list_nth_data (values, 0));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to delete enum_values",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	oseaserver_message_ok_answer (channel, msg_no, "enum_values deleted", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}

gboolean os_customer_enum_values_edit (OseaCommXmlServiceData * data,
				       gpointer user_data,
				       RRChannel * channel,
				       gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "enum_values_id", "new_enum_type",
					    "new_enum_order_in_type", "new_enum_value", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Update enum_values values
	result = oseaserver_command_execute_non_query
		("UPDATE os_customer_enum_values SET enum_type = '%s' , enum_order_in_type = %s , enum_value = '%s' WHERE id = %s",
		 g_list_nth_data (values, 1), g_list_nth_data (values, 2),
		 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	oseaserver_message_ok_answer (channel, msg_no, "enum_values successfully edited",
				OSEACOMM_CODE_OK, result, NULL);

	return TRUE;
}

gboolean os_customer_enum_values_list (OseaCommXmlServiceData * data,
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
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values WHERE id >= %d",
			 initial);
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,enum_type,enum_order_in_type,enum_value FROM os_customer_enum_values WHERE id >= %d LIMIT %d",
			 initial, row_number);
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available enum_valuess", OSEACOMM_CODE_OK,
				response, NULL);

	return TRUE;

}
