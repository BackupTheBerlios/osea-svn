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

#include "os_kernel_group_request.h"
#include <afgs/afgs.h>
#include <coyote/coyote.h>

gboolean os_kernel_group_list    (CoyoteXmlServiceData * data, 
				 gpointer               group_data, 
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
		response = afgs_command_execute_single_query ("SELECT id,name,description FROM kernel_group WHERE id >= %d", 
							      initial);
	else
		response = afgs_command_execute_single_query ("SELECT id,name,description FROM kernel_group WHERE id >= %d LIMIT %d", 
							      initial, row_number);

	// Send a response
	afgs_message_ok_answer (channel, msg_no, "Available groups", COYOTE_CODE_OK, response, NULL);

	return TRUE;
}


gboolean os_kernel_group_new     (CoyoteXmlServiceData * data, 
				 gpointer               group_data, 
				 RRChannel            * channel, 
				 gint                   msg_no)
{
	GList  * values;
	CoyoteDataSet  * group_id;
	gboolean         result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "name", "description", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Creates the new group
	result = afgs_command_execute_non_query ("INSERT INTO kernel_group (name, description) VALUES ('%s', '%s')",
						 g_list_nth_data (values,  0),
						 g_list_nth_data (values,  1));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to create new group",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}

	// Check out the newly create group identifier
	group_id = afgs_command_execute_single_query ("SELECT id FROM kernel_group WHERE name = '%s'", 
						      g_list_nth_data (values,  0));

	afgs_message_ok_answer (channel, msg_no, "Group created", COYOTE_CODE_OK, group_id, NULL);

	return TRUE;
}

gboolean os_kernel_group_edit    (CoyoteXmlServiceData * data, 
				 gpointer               group_data, 
				 RRChannel            * channel, 
				 gint                   msg_no)
{
	GList  * values;
	gboolean         result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "id", "new_name", "new_description", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Update group values
	result = afgs_command_execute_non_query ("UPDATE kernel_group SET name = '%s', description = '%s' WHERE id = %s",
						 g_list_nth_data (values,  1),
						 g_list_nth_data (values,  2),
						 g_list_nth_data (values,  0));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to update group data",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	
	afgs_message_ok_answer (channel, msg_no, "Group updated", COYOTE_CODE_OK, NULL);

	return TRUE;
}

gboolean os_kernel_group_remove  (CoyoteXmlServiceData * data,
				 gpointer               group_data, 
				 RRChannel            * channel, 
				 gint                   msg_no)
{
	GList  * values;
	gboolean         result = FALSE;

	// Check for correct params
	values = afgs_message_check_params (data, "id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Delete all users at this group
	result = afgs_command_execute_non_query ("DELETE FROM kernel_belongs WHERE id_group = %s",
						 g_list_nth_data (values, 0));
	// Delete group
	result = afgs_command_execute_non_query ("DELETE FROM kernel_group WHERE id = %s",
						 g_list_nth_data (values,  0));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to delete group",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	
	afgs_message_ok_answer (channel, msg_no, "Group deleted", COYOTE_CODE_OK, NULL);

	return TRUE;
}

gboolean os_kernel_group_add_user  (CoyoteXmlServiceData * data,
				    gpointer               group_data, 
				    RRChannel            * channel, 
				    gint                   msg_no)
{
	GList  * values;
	gboolean         result = FALSE;


	// Check for correct params
	values = afgs_message_check_params (data, "group_id", "user_id",NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Delete group
	result = afgs_command_execute_non_query ("INSERT INTO kernel_belongs (id_user, id_group) VALUES (%s, %s)",
						 g_list_nth_data (values,  1),
						 g_list_nth_data (values,  0));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to add user to group",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	
	afgs_message_ok_answer (channel, msg_no, "User added to group", COYOTE_CODE_OK, NULL);

	return TRUE;
}

gboolean os_kernel_group_remove_user  (CoyoteXmlServiceData * data,
				       gpointer               group_data, 
				       RRChannel            * channel, 
				       gint                   msg_no)
{
	GList  * values;
	gboolean         result = FALSE;

	
	// Check for correct params
	values = afgs_message_check_params (data, "group_id", "user_id",NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Delete group
	result = afgs_command_execute_non_query ("DELETE FROM kernel_belongs WHERE id_user = %s and  id_group = %s",
						 g_list_nth_data (values,  1),
						 g_list_nth_data (values,  0));
	if (!result) {
		afgs_message_error_answer (channel, msg_no, "Unable to remove user from group",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	
	afgs_message_ok_answer (channel, msg_no, "User removed from group", COYOTE_CODE_OK, NULL);

	return TRUE;
}

gboolean os_kernel_group_list_user  (CoyoteXmlServiceData * data,
				     gpointer               group_data, 
				     RRChannel            * channel, 
				     gint                   msg_no)
{
	GList * values;
	CoyoteDataSet * response;
	gint            initial, row_number;
	gchar         * aux;


	// Check for correct params
	values = afgs_message_check_params (data, "initial", "row_number", "group_id", NULL);
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
		response = afgs_command_execute_single_query ("SELECT u.id, u.nick, u.description \
                                                               FROM kernel_user u, kernel_belongs b \
                                                               WHERE u.id = b.id_user and id_group = %s and u.id >= %d", 
							      g_list_nth_data (values, 2),
							      initial);
	else
		response = afgs_command_execute_single_query ("SELECT u.id, u.nick, u.description \
                                                               FROM kernel_user u, kernel_belongs b \
                                                               WHERE u.id = b.id_user and id_group = %s and u.id >= %d LIMIT %d", 
							      g_list_nth_data (values,  2),
							      initial, row_number, NULL);

	
	// Send a response
	afgs_message_ok_answer (channel, msg_no, "Available users at this group", COYOTE_CODE_OK, response, NULL);

	return TRUE;	
}
