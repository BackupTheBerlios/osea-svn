//  af_customer: Daemon for the server-side
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

#include "af_customer_customer.h"
#include <afgs/afgs.h>
#include <coyote/coyote.h>
gboolean af_customer_customer_address_set (CoyoteXmlServiceData * data,
					   gpointer customer_data,
					   RRChannel * channel,
					   gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "address_id", "", "customer_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean af_customer_customer_address_update_all (CoyoteXmlServiceData * data,
						  gpointer customer_data,
						  RRChannel * channel,
						  gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "address_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean af_customer_customer_address_list (CoyoteXmlServiceData * data,
					    gpointer customer_data,
					    RRChannel * channel,
					    gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "initial_customer", "max_row_number",
					    "address_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Check out request information
	if (!g_strcasecmp ("0", g_list_nth_data (values, 1)))
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer, af_customer_located WHERE id = customer_id AND address_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer, af_customer_located WHERE id = customer_id AND address_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "customer elements which has address",
				COYOTE_CODE_OK, response, NULL);

	return TRUE;



}

gboolean af_customer_customer_address_remove (CoyoteXmlServiceData * data,
					      gpointer customer_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "address_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add address to customer 
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_located (customer_id, address_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add address to customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_message_ok_answer (channel, msg_no, "address added to customer", COYOTE_CODE_OK,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_address_add (CoyoteXmlServiceData * data,
					   gpointer customer_data,
					   RRChannel * channel,
					   gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "address_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add address to customer 
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_located (customer_id, address_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add address to customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_message_ok_answer (channel, msg_no, "address added to customer", COYOTE_CODE_OK,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_telephone_set (CoyoteXmlServiceData * data,
					     gpointer customer_data,
					     RRChannel * channel,
					     gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "telephone_id", "", "customer_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean af_customer_customer_telephone_update_all (CoyoteXmlServiceData * data,
						    gpointer customer_data,
						    RRChannel * channel,
						    gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "telephone_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean af_customer_customer_telephone_list (CoyoteXmlServiceData * data,
					      gpointer customer_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "initial_customer", "max_row_number",
					    "telephone_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Check out request information
	if (!g_strcasecmp ("0", g_list_nth_data (values, 1)))
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer, af_customer_has_phone WHERE id = customer_id AND telephone_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer, af_customer_has_phone WHERE id = customer_id AND telephone_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "customer elements which has telephone",
				COYOTE_CODE_OK, response, NULL);

	return TRUE;



}

gboolean af_customer_customer_telephone_remove (CoyoteXmlServiceData * data,
						gpointer customer_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "telephone_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add telephone to customer 
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_has_phone (customer_id, telephone_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add telephone to customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_message_ok_answer (channel, msg_no, "telephone added to customer", COYOTE_CODE_OK,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_telephone_add (CoyoteXmlServiceData * data,
					     gpointer customer_data,
					     RRChannel * channel,
					     gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "telephone_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add telephone to customer 
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_has_phone (customer_id, telephone_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add telephone to customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_message_ok_answer (channel, msg_no, "telephone added to customer", COYOTE_CODE_OK,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_inet_data_set (CoyoteXmlServiceData * data,
					     gpointer customer_data,
					     RRChannel * channel,
					     gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "inet_data_id", "", "customer_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean af_customer_customer_inet_data_update_all (CoyoteXmlServiceData * data,
						    gpointer customer_data,
						    RRChannel * channel,
						    gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "inet_data_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean af_customer_customer_inet_data_list (CoyoteXmlServiceData * data,
					      gpointer customer_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "initial_customer", "max_row_number",
					    "inet_data_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Check out request information
	if (!g_strcasecmp ("0", g_list_nth_data (values, 1)))
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer, af_customer_has_web_data WHERE id = customer_id AND inet_data_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer, af_customer_has_web_data WHERE id = customer_id AND inet_data_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "customer elements which has inet_data",
				COYOTE_CODE_OK, response, NULL);

	return TRUE;



}

gboolean af_customer_customer_inet_data_remove (CoyoteXmlServiceData * data,
						gpointer customer_data,
						RRChannel * channel,
						gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "inet_data_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add inet_data to customer 
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_has_web_data (customer_id, inet_data_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add inet_data to customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_message_ok_answer (channel, msg_no, "inet_data added to customer", COYOTE_CODE_OK,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_inet_data_add (CoyoteXmlServiceData * data,
					     gpointer customer_data,
					     RRChannel * channel,
					     gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "inet_data_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add inet_data to customer 
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_has_web_data (customer_id, inet_data_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add inet_data to customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_message_ok_answer (channel, msg_no, "inet_data added to customer", COYOTE_CODE_OK,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_contact_set (CoyoteXmlServiceData * data,
					   gpointer customer_data,
					   RRChannel * channel,
					   gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "contact_id", "", "customer_setof", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean af_customer_customer_contact_update_all (CoyoteXmlServiceData * data,
						  gpointer customer_data,
						  RRChannel * channel,
						  gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "contact_id", "value_to_set",
					    NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	return TRUE;
}

gboolean af_customer_customer_contact_list (CoyoteXmlServiceData * data,
					    gpointer customer_data,
					    RRChannel * channel,
					    gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;

	// Check for correct params
	values = afgs_message_check_params (data, "initial_customer", "max_row_number",
					    "contact_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Check out request information
	if (!g_strcasecmp ("0", g_list_nth_data (values, 1)))
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer, af_customer_contact_with WHERE id = customer_id AND contact_id = %s AND id >= %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	else
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer, af_customer_contact_with WHERE id = customer_id AND contact_id = %s AND id >= %s LIMIT %s",
			 g_list_nth_data (values, 3), g_list_nth_data (values, 0),
			 g_list_nth_data (values, 1));
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "customer elements which has contact",
				COYOTE_CODE_OK, response, NULL);

	return TRUE;



}

gboolean af_customer_customer_contact_remove (CoyoteXmlServiceData * data,
					      gpointer customer_data,
					      RRChannel * channel,
					      gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "contact_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add contact to customer 
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_contact_with (customer_id, contact_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add contact to customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_message_ok_answer (channel, msg_no, "contact added to customer", COYOTE_CODE_OK,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_contact_add (CoyoteXmlServiceData * data,
					   gpointer customer_data,
					   RRChannel * channel,
					   gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "contact_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Add contact to customer 
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_contact_with (customer_id, contact_id) VALUES (%s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add contact to customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_message_ok_answer (channel, msg_no, "contact added to customer", COYOTE_CODE_OK,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_new (CoyoteXmlServiceData * data,
				   gpointer user_data,
				   RRChannel * channel,
				   gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *customer_id;
	gboolean            result = FALSE;

	// Check for correct params
	values = afgs_message_check_params (data, "legal_id", "name", "surname", "salesman",
					    "vat_customer_type", "customer_ref", "enum_values_id",
					    NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Creates the new customer
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_customer (legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id) VALUES ('%s', '%s', '%s', '%s', %s, '%s', %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2), g_list_nth_data (values, 3),
		 g_list_nth_data (values, 4), g_list_nth_data (values, 5),
		 g_list_nth_data (values, 6));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to create new customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	// Check out the previously created customer identifier
	customer_id =
		afgs_command_execute_single_query
		("SELECT id FROM af_customer_customer WHERE legal_id = '%s' AND name = '%s' AND surname = '%s' AND salesman = '%s' AND vat_customer_type = %s AND customer_ref = '%s' AND enum_values_id = %s",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2), g_list_nth_data (values, 3),
		 g_list_nth_data (values, 4), g_list_nth_data (values, 5),
		 g_list_nth_data (values, 6));
	afgs_message_ok_answer (channel, msg_no, "customer created", COYOTE_CODE_OK, customer_id,
				NULL);

	return TRUE;
}

gboolean af_customer_customer_remove (CoyoteXmlServiceData * data,
				      gpointer user_data,
				      RRChannel * channel,
				      gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;

	// Check for correct params 
	values = afgs_message_check_params (data, "id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Delete customer
	result = afgs_command_execute_non_query ("DELETE FROM af_customer_customer WHERE id = %s",
						 g_list_nth_data (values, 0));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to delete customer",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}

	afgs_message_ok_answer (channel, msg_no, "customer deleted", COYOTE_CODE_OK, NULL);

	return TRUE;
}

gboolean af_customer_customer_edit (CoyoteXmlServiceData * data,
				    gpointer user_data,
				    RRChannel * channel,
				    gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "customer_id", "new_legal_id", "new_name",
					    "new_surname", "new_salesman",
					    "new_vat_customer_type", "new_customer_ref",
					    "new_enum_values_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Update customer values
	result = afgs_command_execute_non_query
		("UPDATE af_customer_customer SET legal_id = '%s' , name = '%s' , surname = '%s' , salesman = '%s' , vat_customer_type = %s , customer_ref = '%s' , enum_values_id = %s WHERE id = %s",
		 g_list_nth_data (values, 1), g_list_nth_data (values, 2),
		 g_list_nth_data (values, 3), g_list_nth_data (values, 4),
		 g_list_nth_data (values, 5), g_list_nth_data (values, 6),
		 g_list_nth_data (values, 7), g_list_nth_data (values, 0));
	afgs_message_ok_answer (channel, msg_no, "customer successfully edited", COYOTE_CODE_OK,
				result, NULL);

	return TRUE;
}

gboolean af_customer_customer_list (CoyoteXmlServiceData * data,
				    gpointer user_data,
				    RRChannel * channel,
				    gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *response;
	gint                initial, row_number;
	gchar              *aux;

	// Check for correct params
	values = afgs_message_check_params (data, "initial", "row_number", NULL);
	if (!values) {

		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Convert initial and row_number to interger
	initial = strtol (g_list_nth_data (values, 0), &aux, 10);
	if (strlen (aux) != 0) {
		afgs_message_error_answer (channel, msg_no,
					   "Invalid format for initial parameter",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}

	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		afgs_message_error_answer (channel, msg_no,
					   "Invalid format for row_number parameter",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	// Check out request information
	if (row_number == 0)
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer WHERE id >= %d",
			 initial);
	else
		response =
			afgs_command_execute_single_query
			("SELECT id,legal_id,name,surname,salesman,vat_customer_type,customer_ref,enum_values_id FROM af_customer_customer WHERE id >= %d LIMIT %d",
			 initial, row_number);
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "Available customers", COYOTE_CODE_OK, response,
				NULL);

	return TRUE;

}
