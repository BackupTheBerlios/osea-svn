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

#include "af_customer_telephone.h"
#include <afgs/afgs.h>
#include <coyote/coyote.h>

gboolean af_customer_telephone_new (CoyoteXmlServiceData * data,
				    gpointer user_data,
				    RRChannel * channel,
				    gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *telephone_id;
	gboolean            result = FALSE;

	// Check for correct params
	values = afgs_message_check_params (data, "phone", "description", "customer_id",
					    "enum_values_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Creates the new telephone
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_telephone (phone,description,customer_id,enum_values_id) VALUES ('%s', '%s', %s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2), g_list_nth_data (values, 3));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to create new telephone",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	// Check out the previously created telephone identifier
	telephone_id =
		afgs_command_execute_single_query
		("SELECT id FROM af_customer_telephone WHERE phone = '%s' AND description = '%s' AND customer_id = %s AND enum_values_id = %s",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2), g_list_nth_data (values, 3));
	afgs_message_ok_answer (channel, msg_no, "telephone created", COYOTE_CODE_OK,
				telephone_id, NULL);

	return TRUE;
}

gboolean af_customer_telephone_remove (CoyoteXmlServiceData * data,
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
	// Delete telephone
	result = afgs_command_execute_non_query
		("DELETE FROM af_customer_telephone WHERE id = %s", g_list_nth_data (values, 0));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to delete telephone",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}

	afgs_message_ok_answer (channel, msg_no, "telephone deleted", COYOTE_CODE_OK, NULL);

	return TRUE;
}

gboolean af_customer_telephone_edit (CoyoteXmlServiceData * data,
				     gpointer user_data,
				     RRChannel * channel,
				     gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "telephone_id", "new_phone", "new_description",
					    "new_customer_id", "new_enum_values_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Update telephone values
	result = afgs_command_execute_non_query
		("UPDATE af_customer_telephone SET phone = '%s' , description = '%s' , customer_id = %s , enum_values_id = %s WHERE id = %s",
		 g_list_nth_data (values, 1), g_list_nth_data (values, 2),
		 g_list_nth_data (values, 3), g_list_nth_data (values, 4),
		 g_list_nth_data (values, 0));
	afgs_message_ok_answer (channel, msg_no, "telephone successfully edited", COYOTE_CODE_OK,
				result, NULL);

	return TRUE;
}

gboolean af_customer_telephone_list (CoyoteXmlServiceData * data,
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
			("SELECT id,phone,description,customer_id,enum_values_id FROM af_customer_telephone WHERE id >= %d",
			 initial);
	else
		response =
			afgs_command_execute_single_query
			("SELECT id,phone,description,customer_id,enum_values_id FROM af_customer_telephone WHERE id >= %d LIMIT %d",
			 initial, row_number);
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "Available telephones", COYOTE_CODE_OK, response,
				NULL);

	return TRUE;

}
