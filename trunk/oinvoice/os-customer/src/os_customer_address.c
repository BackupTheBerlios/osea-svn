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

#include "af_customer_address.h"
#include <afgs/afgs.h>
#include <coyote/coyote.h>

gboolean af_customer_address_new (CoyoteXmlServiceData * data,
				  gpointer user_data,
				  RRChannel * channel,
				  gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *address_id;
	gboolean            result = FALSE;

	// Check for correct params
	values = afgs_message_check_params (data, "address", "city", "state", "zip_code",
					    "country", "customer_id", "enum_values_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Creates the new address
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_address (address,city,state,zip_code,country,customer_id,enum_values_id) VALUES ('%s', '%s', '%s', '%s', '%s', %s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2), g_list_nth_data (values, 3),
		 g_list_nth_data (values, 4), g_list_nth_data (values, 5),
		 g_list_nth_data (values, 6));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to create new address",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	// Check out the previously created address identifier
	address_id =
		afgs_command_execute_single_query
		("SELECT id FROM af_customer_address WHERE address = '%s' AND city = '%s' AND state = '%s' AND zip_code = '%s' AND country = '%s' AND customer_id = %s AND enum_values_id = %s",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2), g_list_nth_data (values, 3),
		 g_list_nth_data (values, 4), g_list_nth_data (values, 5),
		 g_list_nth_data (values, 6));
	afgs_message_ok_answer (channel, msg_no, "address created", COYOTE_CODE_OK, address_id,
				NULL);

	return TRUE;
}

gboolean af_customer_address_remove (CoyoteXmlServiceData * data,
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
	// Delete address
	result = afgs_command_execute_non_query ("DELETE FROM af_customer_address WHERE id = %s",
						 g_list_nth_data (values, 0));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to delete address",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}

	afgs_message_ok_answer (channel, msg_no, "address deleted", COYOTE_CODE_OK, NULL);

	return TRUE;
}

gboolean af_customer_address_edit (CoyoteXmlServiceData * data,
				   gpointer user_data,
				   RRChannel * channel,
				   gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "address_id", "new_address", "new_city",
					    "new_state", "new_zip_code", "new_country",
					    "new_customer_id", "new_enum_values_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Update address values
	result = afgs_command_execute_non_query
		("UPDATE af_customer_address SET address = '%s' , city = '%s' , state = '%s' , zip_code = '%s' , country = '%s' , customer_id = %s , enum_values_id = %s WHERE id = %s",
		 g_list_nth_data (values, 1), g_list_nth_data (values, 2),
		 g_list_nth_data (values, 3), g_list_nth_data (values, 4),
		 g_list_nth_data (values, 5), g_list_nth_data (values, 6),
		 g_list_nth_data (values, 7), g_list_nth_data (values, 0));
	afgs_message_ok_answer (channel, msg_no, "address successfully edited", COYOTE_CODE_OK,
				result, NULL);

	return TRUE;
}

gboolean af_customer_address_list (CoyoteXmlServiceData * data,
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
			("SELECT id,address,city,state,zip_code,country,customer_id,enum_values_id FROM af_customer_address WHERE id >= %d",
			 initial);
	else
		response =
			afgs_command_execute_single_query
			("SELECT id,address,city,state,zip_code,country,customer_id,enum_values_id FROM af_customer_address WHERE id >= %d LIMIT %d",
			 initial, row_number);
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "Available addresss", COYOTE_CODE_OK, response,
				NULL);

	return TRUE;

}
