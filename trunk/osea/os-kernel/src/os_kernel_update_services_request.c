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

#include "os_kernel_update_services_request.h"
#include <liboseaserver/oseaserver.h>
#include <liboseacomm/oseacomm.h>

gboolean os_kernel_update_services    (OseaCommXmlServiceData * data, 
				       gpointer               user_data, 
				       RRChannel            * channel, 
				       gint                   msg_no)
{

	GList          * values;
	OseaCommDataSet  * dataset; 
	OseaCommDataSet  * permissions;
	gchar         ** splited_string;
	gchar          * permission_string;
	gboolean         response;
	gint             iterator;
	gint             iterator2;
	gint             height;

	
	// Check params
	values = oseaserver_message_check_params (data, "server_name", "description", "version", "permissions", NULL);
	if (!values) {
		
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}



	// Check if the server is already installed
	dataset = oseaserver_command_execute_single_query ("SELECT * FROM kernel_server WHERE name = '%s'", 
						     g_list_nth_data (values, 0));

	if (oseacomm_dataset_get_height (dataset) == 0) {
		// Server not installed. Install it.
		response = oseaserver_command_execute_non_query ("INSERT INTO kernel_server (name, description, version) VALUES ('%s', '%s', %s)",
							   g_list_nth_data (values, 0),
							   g_list_nth_data (values, 1),
							   g_list_nth_data (values, 2));
		if (!response) {
			oseaserver_message_error_answer (channel, msg_no, "Unable to install server services",
						   OSEACOMM_CODE_ERROR);
			return FALSE;
		}
	}
	oseacomm_dataset_free (dataset);
	

	// We are going to check if update_services request is
	// executed to install all services or to update already existing services.
	// Right now we only support install mode
	dataset = oseaserver_command_execute_single_query ("SELECT * \
                                                      FROM kernel_permission p, kernel_server s \
                                                      WHERE p.server_id = s.id and s.name = '%s'",
						     g_list_nth_data (values, 0));

	if (oseacomm_dataset_get_height (dataset) == 0) {
		// Install mode
		oseacomm_dataset_free (dataset);
		
		// First we are going to insert all permissions
		g_print ("Recieved permissions\n");
		permissions = g_list_nth_data (values, 3);

		oseacomm_dataset_print (permissions);

		height = oseacomm_dataset_get_height (permissions);

		for (iterator = 0; iterator < height; iterator++) {
			response = oseaserver_command_execute_non_query ("INSERT INTO kernel_permission (name, description, server_id) VALUES \
                                                                    ('%s', '%s', (SELECT id FROM kernel_server WHERE name = '%s'))",
								   oseacomm_dataset_get (permissions, iterator, 0),
								   oseacomm_dataset_get (permissions, iterator, 1),
								   g_list_nth_data (values, 0));
			if (!response) {
				oseaserver_message_error_answer (channel, msg_no, "Unable to install server services",
							   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
				return FALSE;
			}
		}
		
		// Then we are going to insert all dependencies
		for (iterator = 0; iterator < height; iterator++) {

			// Check for NULL permission_string 
			permission_string = (gchar *) oseacomm_dataset_get (permissions, iterator, 2);

			if (!(permission_string && *permission_string)) 
				continue;

			splited_string = g_strsplit (permission_string, ",", 0);
			
			iterator2 = 0;

			while (splited_string[iterator2] != NULL) {
				
				response = oseaserver_command_execute_non_query ("INSERT INTO kernel_depends (id_permission, id_depends) VALUES ( \
                                                                            (SELECT id FROM kernel_permission WHERE name = '%s'), \
                                                                            (SELECT id FROM kernel_permission WHERE name = '%s'))",
									   oseacomm_dataset_get (permissions, iterator, 0),
									   splited_string[iterator2]);
				if (!response) {
					oseaserver_message_error_answer (channel, msg_no, "Unable to install service dependencies",
								   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
					return FALSE;
				}

				iterator2++;
			}
			g_strfreev (splited_string);
		}
		
	}else {
		// Update mode, not implemented
		oseacomm_dataset_free (dataset);

		// Clue: you have to check
		//     1) permissions that were removed
		//     2) permissions that are new
                //     3) permission dependencies that are new
                //     4) permission dependencies that were removed
	}
						  
	
	
	
	oseaserver_message_ok_answer (channel, msg_no, "Services updated", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}
