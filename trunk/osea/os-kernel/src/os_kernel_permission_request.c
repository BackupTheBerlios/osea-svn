//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//  Copyright (C) 2004 David Marín Carreño
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

#include "os_kernel_permission_request.h"
#include <liboseaserver/oseaserver.h>
#include <liboseacomm/oseacomm.h>
#include <string.h>

gboolean os_kernel_permission_user_set     (OseaCommXmlServiceData * data, 
					    gpointer               permission_data, 
					    RRChannel            * channel, 
					    gint                   msg_no)
{
	GList      * values;
	gboolean     result = FALSE;

	// Check for correct params
	values = oseaserver_message_check_params (data, "id", "user_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Sets permission to user 'user_id'
	result = oseaserver_command_execute_non_query ("INSERT INTO kernel_user_have (user_id, permission_id) VALUES (%s,%s)",
						 g_list_nth_data (values,  1),
						 g_list_nth_data (values,  0));
						 
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to set permission to user ",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	oseaserver_message_ok_answer (channel, msg_no, "Permission set to user correctly", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}

gboolean os_kernel_permission_user_unset     (OseaCommXmlServiceData * data, 
					      gpointer               permission_data, 
					      RRChannel            * channel, 
					      gint                   msg_no)
{
	GList     * values;
	gboolean    result = FALSE;

	
	// Check for correct params
	values = oseaserver_message_check_params (data, "id", "user_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Sets permission to user 'user_id'
	result = oseaserver_command_execute_non_query ("DELETE FROM kernel_user_have WHERE user_id = %s and  permission_id = %s",
						 g_list_nth_data (values,  1),
						 g_list_nth_data (values,  0));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to unset permission to user ",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	oseaserver_message_ok_answer (channel, msg_no, "Permission unset successfully", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}

gboolean os_kernel_permission_group_set              (OseaCommXmlServiceData * data, 
						     gpointer               permission_data, 
						     RRChannel            * channel, 
						     gint                   msg_no)
{
	GList     * values;
	gboolean    result = FALSE;

	// Check for correct params
	values = oseaserver_message_check_params (data, "id", "group_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Sets permission to user 'user_id'
	result = oseaserver_command_execute_non_query ("INSERT INTO kernel_group_have (group_id, permission_id) VALUES (%s,%s)",
						 g_list_nth_data (values,  1),
						 g_list_nth_data (values,  0));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to set permission to group ",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	oseaserver_message_ok_answer (channel, msg_no, "Permission set to group correctly", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}

gboolean os_kernel_permission_group_unset            (OseaCommXmlServiceData * data, 
						     gpointer               permission_data, 
						     RRChannel            * channel, 
						     gint                   msg_no)
{
	GList     * values;
	gboolean    result = FALSE;

	
	// Check for correct params
	values = oseaserver_message_check_params (data, "id", "group_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Sets permission to user 'user_id'
	result = oseaserver_command_execute_non_query ("DELETE FROM kernel_group_have WHERE group_id = %s and  permission_id = %s",
						 g_list_nth_data (values,  1),
						 g_list_nth_data (values,  0));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to unset permission to group",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	oseaserver_message_ok_answer (channel, msg_no, "Permission unset successfully", OSEACOMM_CODE_OK, NULL);

	return TRUE;	
}

gboolean os_kernel_permission_list         (OseaCommXmlServiceData * data, 
					    gpointer               permission_data, 
					    RRChannel            * channel, 
					    gint                   msg_no)
{

	GList         * values;
	OseaCommDataSet * temp_dataset, * temp_dataset2, * result;
	gint            initial, row_number, rows, i, j, id;
	gchar         * aux;
	GString       * depend_string;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_permission", "row_number", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	
	// Convert initial and row_number to interger
	initial = strtol (g_list_nth_data (values, 0), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for initial parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for row_number parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	// Check out request information
	if (row_number == 0)
		temp_dataset = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                              FROM kernel_permission p, kernel_server k \
                                                              WHERE p.id >= %d AND k.id = p.server_id \
                                                              ORDER BY k.name, p.name", 
								  initial);
	else
		temp_dataset = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                              FROM kernel_permission p, kernel_server k \
                                                              WHERE p.id >= %d AND k.id = p.server_id \
                                                              ORDER BY k.name, p.name LIMIT %d",
							      initial, row_number);

	rows = oseacomm_dataset_get_height (temp_dataset);


	result = oseacomm_dataset_new ();

	for (i=0; i < rows; i++) {
		id = atoi (oseacomm_dataset_get (temp_dataset, i, 0));

		temp_dataset2 = oseaserver_command_execute_single_query ("SELECT p.name FROM kernel_permission p, kernel_depends d \
                                                                    WHERE d.id_permission = %d AND d.id_depends = p.id",
								   id);
		depend_string = g_string_new ("");

		for (j=0; j < oseacomm_dataset_get_height (temp_dataset2); j++) {
			if (j != 0)
				g_string_append_printf (depend_string, ";");

			g_string_append_printf (depend_string, (gchar *) oseacomm_dataset_get (temp_dataset2, j, 0));
		}
		oseacomm_dataset_free (temp_dataset2);

		oseacomm_dataset_add (result, (gchar *) oseacomm_dataset_get (temp_dataset, i, 0));
		oseacomm_dataset_add (result, (gchar *) oseacomm_dataset_get (temp_dataset, i, 1));
		oseacomm_dataset_add (result, (gchar *) oseacomm_dataset_get (temp_dataset, i, 2));
		oseacomm_dataset_add (result, (gchar *) oseacomm_dataset_get (temp_dataset, i, 3));
		oseacomm_dataset_add (result, (gchar *) oseacomm_dataset_get (temp_dataset, i, 4));
		oseacomm_dataset_add (result, depend_string->str);

		if (i != (rows -1))
			oseacomm_dataset_new_row (result);
	
	}

	oseacomm_dataset_free (temp_dataset);


	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available permissions", OSEACOMM_CODE_OK, result, NULL);

	return TRUE;
}

gboolean os_kernel_permission_list_by_user          (OseaCommXmlServiceData * data, 
						     gpointer               permission_data, 
						     RRChannel            * channel, 
						     gint                   msg_no)
{
	GList         * values;
	OseaCommDataSet * response;
	gint            initial, row_number, user_id;
	gchar         * aux;


	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_permission", "row_number", "user_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	
	// Convert initial, row_number and user_id to interger
	initial = strtol (g_list_nth_data (values, 0), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for initial parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for row_number parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	aux = NULL;
	user_id = strtol (g_list_nth_data (values, 2), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for user_id parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}


	// Check out request information
	if (row_number == 0)
		response = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_user_have h \
                                                               WHERE p.id >= %d and h.user_id = %d and h.permission_id = p.id and p.server_id = k.id \
                                                               UNION \
                                                               SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_group_have h, kernel_belongs b \
                                                               WHERE p.id >= %d and %d = b.id_user and b.id_group = h.group_id and h.permission_id = p.id and p.server_id = k.id",
							       initial, user_id, initial, user_id);
	else
		response = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_user_have h \
                                                               WHERE p.id >= %d and h.user_id = %d and h.permission_id = p.id and p.server_id = k.id \
                                                               UNION \
                                                               SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_group_have h, kernel_belongs b \
                                                               WHERE p.id >= %d and %d = b.id_user and b.id_group = h.group_id and h.permission_id = p.id and p.server_id = k.id \
                                                               LIMIT %d",
							       initial, user_id, initial, user_id, row_number);

	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available permissions for request user", OSEACOMM_CODE_OK, response, NULL);

	return TRUE;
}

gboolean os_kernel_permission_list_by_group         (OseaCommXmlServiceData * data, 
						     gpointer               permission_data, 
						     RRChannel            * channel, 
						     gint                   msg_no)
{
	GList         * values;
	OseaCommDataSet * response;
	gint            initial, row_number, group_id;
	gchar         * aux;


	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_permission", "row_number", "group_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	
	// Convert initial, row_number and group_id to interger
	initial = strtol (g_list_nth_data (values, 0), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for initial parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for row_number parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	aux = NULL;
	group_id = strtol (g_list_nth_data (values, 2), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for group_id parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}


	// Check out request information
	if (row_number == 0)
		response = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_group_have h \
                                                               WHERE p.id >= %d and h.group_id = %d and h.permission_id = p.id \
                                                               ORDER BY k.name, p.name",
							      initial, group_id);
	else
		response = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_group_have h \
                                                               WHERE p.id >= %d and h.group_id = %d and h.permission_id = p.id \
                                                               ORDER BY k.name, p.name LIMIT %s",
							      initial, group_id, row_number);

	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available permissions for requested group", OSEACOMM_CODE_OK, response, NULL);

	return TRUE;
}


gboolean os_kernel_permission_actual_list_by_user          (OseaCommXmlServiceData * data, 
							    gpointer               permission_data, 
							    RRChannel            * channel, 
							    gint                   msg_no)
{
	GList         * values;
	OseaCommDataSet * response;
	gint            initial, row_number, user_id;
	gchar         * aux;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_permission", "row_number", "user_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	
	// Convert initial, row_number and user_id to interger
	initial = strtol (g_list_nth_data (values, 0), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for initial parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for row_number parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	aux = NULL;
	user_id = strtol (g_list_nth_data (values, 2), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for user_id parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}


	// Check out request information
	if (row_number == 0)
		response = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_user_have h \
                                                               WHERE p.id >= %d and h.user_id = %d and h.permission_id = p.id and p.server_id = k.id",
							       initial, user_id, initial, user_id);
	else
		response = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_user_have h \
                                                               WHERE p.id >= %d and h.user_id = %d and h.permission_id = p.id and p.server_id = k.id",
							       initial, user_id, initial, user_id, row_number);

	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available permissions for request user", OSEACOMM_CODE_OK, response, NULL);

	return TRUE;
}

gboolean os_kernel_permission_from_group_list_by_user          (OseaCommXmlServiceData * data, 
								gpointer               permission_data, 
								RRChannel            * channel, 
								gint                   msg_no)
{
	GList         * values;
	OseaCommDataSet * response;
	gint            initial, row_number, user_id;
	gchar         * aux;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial_permission", "row_number", "user_id", NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	
	// Convert initial, row_number and user_id to interger
	initial = strtol (g_list_nth_data (values, 0), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for initial parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	aux = NULL;
	row_number = strtol (g_list_nth_data (values, 1), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for row_number parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	aux = NULL;
	user_id = strtol (g_list_nth_data (values, 2), &aux, 10);
	if (strlen (aux) != 0) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid format for user_id parameter",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}


	// Check out request information
	if (row_number == 0)
		response = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_group_have h, kernel_belongs b \
                                                               WHERE p.id >= %d and %d = b.id_user and b.id_group = h.group_id and h.permission_id = p.id and p.server_id = k.id",
							       initial, user_id, initial, user_id);
	else
		response = oseaserver_command_execute_single_query ("SELECT p.id, p.name, p.description, k.name, k.version \
                                                               FROM kernel_permission p, kernel_server k, kernel_group_have h, kernel_belongs b \
                                                               WHERE p.id >= %d and %d = b.id_user and b.id_group = h.group_id and h.permission_id = p.id and p.server_id = k.id \
                                                               LIMIT %d",
							       initial, user_id, initial, user_id, row_number);

	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available permissions for request user", OSEACOMM_CODE_OK, response, NULL);

	return TRUE;
}


