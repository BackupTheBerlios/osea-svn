//  Aspl-rule server: Daemon for server side for rule report system
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

#include "os_kernel_user_request.h"
#include "os_kernel_login_request.h"
#include "os_kernel_crypto.h"
#include <liboseaserver/oseaserver.h>
#include <liboseacomm/oseacomm.h>
#include <string.h>

gboolean os_kernel_user_list    (OseaCommXmlServiceData * data, 
				 gpointer               user_data, 
				 RRChannel            * channel, 
				 gint                   msg_no)
{
	GList         * values;
	OseaCommDataSet * response;
	gint            initial, row_number;
	gchar         * aux;

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
		response = oseaserver_command_execute_single_query ("SELECT id,nick,description FROM kernel_user WHERE id >= %d", 
							      initial);
	else
		response = oseaserver_command_execute_single_query ("SELECT id,nick,description FROM kernel_user WHERE id >= %d LIMIT %d", 
							      initial, row_number);
	
	
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "Available users", OSEACOMM_CODE_OK, response, NULL);

	return TRUE;
}


gboolean os_kernel_user_new     (OseaCommXmlServiceData * data, 
				 gpointer               user_data, 
				 RRChannel            * channel, 
				 gint                   msg_no)
{
	GList          * values;
	OseaCommDataSet  * user_id;
	gboolean         result = FALSE;
	GString        * ciphered_password;



	// Check for correct params
	values = oseaserver_message_check_params (data, "nick", "password", "description", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Creates the new user
	ciphered_password = os_kernel_crypto_md5_sum (g_list_nth_data (values, 1));

	result = oseaserver_command_execute_non_query ("INSERT INTO kernel_user (nick, passwd, description) VALUES ('%s', '%s', '%s')",
						 g_list_nth_data (values,  0),
						 ciphered_password->str,
						 g_list_nth_data (values,  2));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to create new user",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	// Check out the newly created user identifier
	user_id = oseaserver_command_execute_single_query ("SELECT id FROM kernel_user WHERE nick = '%s'", 
						     g_list_nth_data (values,  0));

	oseaserver_message_ok_answer (channel, msg_no, "User created", OSEACOMM_CODE_OK, user_id, NULL);

	return TRUE;
}

gboolean os_kernel_user_edit    (OseaCommXmlServiceData * data, 
				 gpointer               user_data, 
				 RRChannel            * channel, 
				 gint                   msg_no)
{
	GList    * values;
	gboolean   result = FALSE;
	GString  * ciphered_password;


	// Check for correct params
	values = oseaserver_message_check_params (data, "id", "new_nick", "new_password", "new_description", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Edit user
	
	if (strlen (g_list_nth_data(values, 2)) > 0) {	
		ciphered_password = os_kernel_crypto_md5_sum (g_list_nth_data (values, 2));
		result = oseaserver_command_execute_non_query ("UPDATE kernel_user SET nick = '%s', passwd = '%s', description = '%s' WHERE id = %s",
							 g_list_nth_data (values,  1),
							 ciphered_password->str,
							 g_list_nth_data (values,  3),
							 g_list_nth_data (values,  0));
	} else
		result = oseaserver_command_execute_non_query ("UPDATE kernel_user SET nick = '%s', description = '%s' WHERE id = %s",
							 g_list_nth_data (values,  1),
							 g_list_nth_data (values,  3),
							 g_list_nth_data (values,  0));

	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to update user data",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	oseaserver_message_ok_answer (channel, msg_no, "User updated", OSEACOMM_CODE_OK, NULL);
	
	return TRUE;
}

gboolean os_kernel_user_remove  (OseaCommXmlServiceData * data,
				 gpointer               user_data, 
				 RRChannel            * channel, 
				 gint                   msg_no)
{
	GList     * values;
	gboolean    result = FALSE;


	// Check for correct params
	values = oseaserver_message_check_params (data, "id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Check if user wants to remove himself

	

	// We must remove all relations the user have with permission table and group table
	oseaserver_command_execute_non_query ("DELETE FROM kernel_belongs WHERE id_user = %s",
					g_list_nth_data (values,  0));

	oseaserver_command_execute_non_query ("DELETE FROM kernel_user_have WHERE user_id = %s",
					g_list_nth_data (values,  0));

	// Delete user
	result = oseaserver_command_execute_non_query ("DELETE FROM kernel_user WHERE id = %s",
						 g_list_nth_data (values,  0));
	if (!result) {
		oseaserver_message_error_answer (channel, msg_no, "Unable to delete user",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	
	oseaserver_message_ok_answer (channel, msg_no, "User deleted", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}

gboolean os_kernel_user_list_group  (OseaCommXmlServiceData * data,
				     gpointer               group_data, 
				     RRChannel            * channel, 
				     gint                   msg_no)
{
	GList * values;
	OseaCommDataSet * response;
	gint            initial, row_number;
	gchar         * aux;

	// Check for correct params
	values = oseaserver_message_check_params (data, "initial", "row_number", "user_id", NULL);
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
		response = oseaserver_command_execute_single_query ("SELECT g.id, g.name, g.description \
                                                               FROM kernel_group g, kernel_belongs b \
                                                               WHERE g.id = b.id_group and b.id_user = %s and g.id >= %d", 
							      g_list_nth_data (values, 2),
							      initial);
	else
		response = oseaserver_command_execute_single_query ("SELECT g.id, g.name, g.description \
                                                               FROM kernel_group g, kernel_belongs b \
                                                               WHERE g.id = b.id_group and b.id_user = %s and g.id >= %d LIMIT %d", 
							      g_list_nth_data (values,  2),
							      initial, row_number, NULL);

	
	// Send a response
	oseaserver_message_ok_answer (channel, msg_no, "This user belongs to the following groups", OSEACOMM_CODE_OK, response, NULL);

	return TRUE;	
}
