//  Af-Kernel server: 
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <liboseacomm/oseacomm.h>
#include <liboseaserver/oseaserver.h>
#include "os_kernel_register_request.h"
#include "os_kernel_registry.h"
#include "os_kernel_login_request.h"
#include "os_kernel_session.h"

#define LOG_DOMAIN "af-kernel_register_request"


gboolean os_kernel_register_check_version (RRChannel * channel, gint msg_no, gchar * server_name, gchar * server_version) 
{
	OseaCommDataSet * dataset;

	g_return_val_if_fail (server_name, FALSE);
	g_return_val_if_fail (server_version, FALSE);
	
	dataset = oseaserver_command_execute_single_query ("SELECT name, version \
                                                      FROM kernel_server WHERE name = '%s'",
						      server_name);
	if (oseacomm_dataset_get_height (dataset) == 0) {
		// So, this server didn't make a registering petition never.
		return FALSE;
	}

	if (!g_strcasecmp (server_version, oseacomm_dataset_get (dataset, 0, 1))) {
		// Same version, register ok
		return TRUE;
	}
	
	return FALSE;
	
	
}

gboolean os_kernel_register_request (OseaCommXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	GList          * values;
	OseaCommDataSet  * session_id_dataset;
	OseaCommDataSet  * server_id_dataset;
	OseaCommDataSet  * servers_dataset;
	gchar          * session_id = NULL;
	gchar          * af_key = NULL;
	RRConnection   * rr_connection = NULL;

	g_return_val_if_fail (data, FALSE);

	// Check for correct params
	values = oseaserver_message_check_params (data, "name", "version", "host", "port", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form",
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}
	
	// Check for satellite server version
	if (!os_kernel_register_check_version (channel, msg_no,
					       g_list_nth_data (values, 0), 
					       g_list_nth_data (values, 1))) {
		oseaserver_message_error_answer (channel, msg_no, "Service update needed, register failed", 
					   OSEACOMM_CODE_SERVICES_UPDATE_NEEDED);
		return FALSE;
	}
	


	if (!os_kernel_registry_insert (g_list_nth_data (values, 0), 
					g_list_nth_data (values, 2),
					g_list_nth_data (values, 3)) ) {
		oseaserver_message_error_answer (channel, msg_no, "Register failed", OSEACOMM_CODE_ERROR);
		return FALSE;
	} 

	// Register correct, create server table

	servers_dataset =  os_kernel_registry_generate_session_info_table (g_list_nth_data (values, 0),
									  &af_key);
	
	// Set connection af-key for this user.
	rr_connection = rr_channel_get_connection (channel);

	oseaserver_afkeys_set_connection_key_simple (rr_connection, 
					oseaserver_afkeys_parse_and_check (af_key));

	
	// Create session_id dataset
	server_id_dataset = oseaserver_command_execute_single_query ("select id from kernel_server where name = '%s'",
						      (gchar *) g_list_nth_data (values, 0));

	session_id = os_kernel_session_new ( atoi(oseacomm_dataset_get (server_id_dataset, 0, 0)), FALSE);
	session_id_dataset = oseacomm_dataset_new ();

	oseacomm_dataset_add (session_id_dataset, session_id);

	// Send all generated data to user
	oseaserver_message_ok_answer (channel, msg_no, "Register ok", OSEACOMM_CODE_OK, 
				session_id_dataset, servers_dataset, NULL);
	
	return TRUE;	
}



/* gboolean os_kernel_register_list_request (OseaCommXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no) */
/* { */
/* 	gboolean           register_is_valid = FALSE; */
/* 	OseaCommDataSet    * data_set = NULL; */


/* 	g_return_val_if_fail (data, FALSE); */

/* 	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "We are in register_list_request"); */
	
/* 	g_static_mutex_lock(&mutex_active_servers); */
/* 	if (os_kernel_active_servers) { */
		
/* 		data_set = oseacomm_dataset_new (); */
		
/* 		g_tree_foreach ((GTree *) os_kernel_active_servers,  */
/* 				(GTraverseFunc) os_kernel_register_build_active_servers,  */
/* 				data_set); */

/* 		register_is_valid = TRUE; */
/* 	} */

/* 	g_static_mutex_unlock(&mutex_active_servers); */

/* 	if (register_is_valid) */
/* 		oseaserver_message_ok_answer (channel, msg_no, "Register ok", OSEACOMM_CODE_OK, data_set, NULL); */
/* 	else */
/* 		oseaserver_message_error_answer (channel, msg_no, "Register failed", OSEACOMM_CODE_ERROR); */
	

/* 	return TRUE; */
	
/* } */

/* gboolean os_kernel_register_build_active_servers (gchar * key,  AfKernelActiveServer * value, gpointer data)  */
/* { */
/* 	OseaCommDataSet * data_set = (CoyoteDataSet *) data; */
/* 	gint i, j; */

/* 	if (oseacomm_dataset_get_height (data_set)) { */
/* 		oseacomm_dataset_new_row (data_set);  */
/* 	} */

/* 	oseacomm_dataset_add_nth (data_set, value->name, value->host, value->port, NULL); */

/* 	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "build active servers tree: h[%d] d[%d]\n",  */
/* 	       oseacomm_dataset_get_height (data_set), */
/* 	       oseacomm_dataset_get_width (data_set)); */
	
/* 	for (i = 0; i < oseacomm_dataset_get_height (data_set); i++) { */
/* 		for (j = 0; j < oseacomm_dataset_get_width (data_set); j++) { */
/* 			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG,  */
/* 			       " [%s] ", oseacomm_dataset_get (data_set, i, j)); */
/* 		} */
			
/* 		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "\n"); */
/* 	} */

/* 	return FALSE; */
/* } */



gboolean os_kernel_unregister_request (OseaCommXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	GList            * values = NULL;
	
	g_return_val_if_fail (data, FALSE);

	// Check for correct params
	values = oseaserver_message_check_params (data, "name", "host","port",  NULL);
	if (!values) {

		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}


	// Check for correct af-key to unregister server.

	
	// Once checked, unregister server
	if ( os_kernel_registry_remove (g_list_nth_data (values, 0)) ) {
		oseaserver_message_ok_answer (channel, msg_no, "Unregister ok", OSEACOMM_CODE_OK, NULL);
		return TRUE;
	} 

	// Unable to unregister server
	oseaserver_message_error_answer (channel, msg_no, "Unable to unregister server", OSEACOMM_CODE_ERROR);
	
	return TRUE;
}
