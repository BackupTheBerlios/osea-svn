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

#include "af_customer_inet_data.h"
#include <afgs/afgs.h>
#include <coyote/coyote.h>

gboolean af_customer_inet_data_new (CoyoteXmlServiceData * data,
				    gpointer user_data,
				    RRChannel * channel,
				    gint msg_no)
{
	GList              *values;
	CoyoteDataSet      *inet_data_id;
	gboolean            result = FALSE;

	// Check for correct params
	values = afgs_message_check_params (data, "inet_data", "customer_id", "enum_values_id",
					    NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Creates the new inet_data
	result = afgs_command_execute_non_query
		("INSERT INTO af_customer_inet_data (inet_data,customer_id,enum_values_id) VALUES ('%s', %s, %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to create new inet_data",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	// Check out the previously created inet_data identifier
	inet_data_id =
		afgs_command_execute_single_query
		("SELECT id FROM af_customer_inet_data WHERE inet_data = '%s' AND customer_id = %s AND enum_values_id = %s",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2));
	afgs_message_ok_answer (channel, msg_no, "inet_data created", COYOTE_CODE_OK,
				inet_data_id, NULL);

	return TRUE;
}

gboolean af_customer_inet_data_remove (CoyoteXmlServiceData * data,
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
	// Delete inet_data
	result = afgs_command_execute_non_query
		("DELETE FROM af_customer_inet_data WHERE id = %s", g_list_nth_data (values, 0));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to delete inet_data",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}

	afgs_message_ok_answer (channel, msg_no, "inet_data deleted", COYOTE_CODE_OK, NULL);

	return TRUE;
}

gboolean af_customer_inet_data_edit (CoyoteXmlServiceData * data,
				     gpointer user_data,
				     RRChannel * channel,
				     gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "inet_data_id", "new_inet_data",
					    "new_customer_id", "new_enum_values_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Update inet_data values
	result = afgs_command_execute_non_query
		("UPDATE af_customer_inet_data SET inet_data = '%s' , customer_id = %s , enum_values_id = %s WHERE id = %s",
		 g_list_nth_data (values, 1), g_list_nth_data (values, 2),
		 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	afgs_message_ok_answer (channel, msg_no, "inet_data successfully edited", COYOTE_CODE_OK,
				result, NULL);

	return TRUE;
}

gboolean af_customer_inet_data_list (CoyoteXmlServiceData * data,
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
			("SELECT id,inet_data,customer_id,enum_values_id FROM af_customer_inet_data WHERE id >= %d",
			 initial);
	else
		response =
			afgs_command_execute_single_query
			("SELECT id,inet_data,customer_id,enum_values_id FROM af_customer_inet_data WHERE id >= %d LIMIT %d",
			 initial, row_number);
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "Available inet_datas", COYOTE_CODE_OK, response,
				NULL);

	return TRUE;

}
