//  Copyright (C) 2002, 2003  Advanced Software Production Line, S.L.
//
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

#include "os_kernel_server_request.h"
#include <afgs/afgs.h>
#include <string.h>

gboolean os_kernel_server_remove    (CoyoteXmlServiceData * data, 
				     gpointer               user_data, 
				     RRChannel            * channel, 
				     gint                   msg_no)
{
	GList  * values = NULL;
	gboolean result = FALSE;

	// Check for correct params
	values = afgs_message_check_params (data, "id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	result = afgs_command_execute_non_query ("DELETE FROM kernel_server WHERE id = %s",
						 g_list_nth_data (values, 0));

	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to delete requested server",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	
	afgs_message_ok_answer (channel, msg_no, "Server removed", COYOTE_CODE_OK, NULL);
	
	return FALSE;
}


gboolean os_kernel_server_list         (CoyoteXmlServiceData * data, 
					gpointer               permission_data, 
					RRChannel            * channel, 
					gint                   msg_no)
{

	GList         * values;
	CoyoteDataSet * response;
	gint            initial, row_number;
	gchar         * aux;


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
		afgs_message_error_answer (channel, msg_no, "Invalid format for initial parameter",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	
	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		afgs_message_error_answer (channel, msg_no, "Invalid format for row_number parameter",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}

	// Check out request information
	if (row_number == 0)
		response = afgs_command_execute_single_query ("SELECT * FROM kernel_server WHERE id >= %d ORDER BY name", 
							      initial);
	else
		response = afgs_command_execute_single_query ("SELECT * FROM kernel_server WHERE id >= %d ORDER BY name LIMIT %d",
							      initial, row_number);

	// Send a response
	afgs_message_ok_answer (channel, msg_no, "Available servers", COYOTE_CODE_OK, response, NULL);

	return TRUE;
}
