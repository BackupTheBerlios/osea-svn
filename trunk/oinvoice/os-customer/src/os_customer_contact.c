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

#include "os_customer_contact.h"
#include <liboseaserver/oseaserver.h>
#include <liboseacomm/oseacomm.h>

gboolean os_customer_contact_new (OseaCommXmlServiceData * data,
				  gpointer user_data,
				  RRChannel * channel,
				  gint msg_no)
{
	GList              *values;
	OseaCommDataSet      *contact_id;
	gboolean            result = FALSE;

	// Check for correct params
	values = oseaserver_message_check_params (data, "name", "position", "customer_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Creates the new contact
	result = oseaserver_command_execute_non_query
		("INSERT INTO os_customer_contact (name,position,customer_id) VALUES ('%s', '%s', %s)",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to create new contact",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	// Check out the previously created contact identifier
	contact_id =
		oseaserver_command_execute_single_query
		("SELECT id FROM os_customer_contact WHERE name = '%s' AND position = '%s' AND customer_id = %s",
		 g_list_nth_data (values, 0), g_list_nth_data (values, 1),
		 g_list_nth_data (values, 2));
	oseaserver_message_ok_answer (channel, msg_no, "contact created", OSEACOMM_CODE_OK, contact_id,
				NULL);

	return TRUE;
}

gboolean os_customer_contact_remove (OseaCommXmlServiceData * data,
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
	// Delete contact
	result = oseaserver_command_execute_non_query ("DELETE FROM os_customer_contact WHERE id = %s",
						 g_list_nth_data (values, 0));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to delete contact",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	oseaserver_message_ok_answer (channel, msg_no, "contact deleted", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}

gboolean os_customer_contact_edit (OseaCommXmlServiceData * data,
				   gpointer user_data,
				   RRChannel * channel,
				   gint msg_no)
{
	GList              *values;
	gboolean            result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "contact_id", "new_name", "new_position",
					    "new_customer_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	// Update contact values
	result = oseaserver_command_execute_non_query
		("UPDATE os_customer_contact SET name = '%s' , position = '%s' , customer_id = %s WHERE id = %s",
		 g_list_nth_data (values, 1), g_list_nth_data (values, 2),
		 g_list_nth_data (values, 3), g_list_nth_data (values, 0));
	oseaserver_message_ok_answer (channel, msg_no, "contact successfully edited", OSEACOMM_CODE_OK,
				result, NULL);

	return TRUE;
}

gboolean os_customer_contact_list (OseaCommXmlServiceData * data,
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
			("SELECT id,name,position,customer_id FROM os_customer_contact WHERE id >= %d",
			 initial);
	else
		response =
			oseaserver_command_execute_single_query
			("SELECT id,name,position,customer_id FROM os_customer_contact WHERE id >= %d LIMIT %d",
			 initial, row_number);
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available contacts", OSEACOMM_CODE_OK, response,
				NULL);

	return TRUE;

}
