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

#include "os_kernel_keys.h"
#include "os_kernel_session.h"
#include <liboseaserver/oseaserver.h>
#include <time.h>

gchar *  os_kernel_keys_generate (gchar * user, gchar * satellite_server)
{
	OseaCommDataSet * dataset         = NULL;
	GString       * result_key      = NULL;
	time_t          time_stamp;
	gint            i,height;



	g_return_val_if_fail (user, NULL);
	g_return_val_if_fail (satellite_server, NULL);

	// Check out all user's permission
	dataset = oseaserver_command_execute_single_query ("SELECT p.name  \
                                                      FROM kernel_user u, kernel_server s, kernel_permission p, kernel_user_have uh  \
                                                      WHERE u.id = uh.user_id and u.nick = '%s' and uh.permission_id = p.id and s.name = '%s' and s.id = p.server_id \
                                                      UNION \
                                                      SELECT p.name  \
                                                      FROM kernel_user u, kernel_server s, kernel_permission p, kernel_belongs b, kernel_group_have gh \
                                                      WHERE p.id = gh.permission_id and gh.group_id = b.id_group and b.id_user = u.id and u.nick = '%s' and s.name = '%s' and s.id = p.server_id",
						      user, satellite_server, user, satellite_server);


	// Create a key
	
	result_key = g_string_new  (NULL);

	time_stamp = time (NULL);

	g_string_printf (result_key, "%s;%s;%ld;", user, satellite_server, time_stamp);
	
	height = oseacomm_dataset_get_height (dataset);
	for (i = 0; i < height; i++) {
		if ((i + 1) == height)
			g_string_append_printf (result_key, "%s", oseacomm_dataset_get (dataset, i, 0));
		else
			g_string_append_printf (result_key, "%s,", oseacomm_dataset_get (dataset, i, 0));
		
	}
	
	oseacomm_dataset_free (dataset);
	
	return result_key->str;
}


gboolean os_kernel_keys_request (OseaCommXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	GList          * values;
	OseaCommDataSet  * af_key_dataset;
	RRConnection   * rr_connection;
	gchar          * new_af_key = NULL;
	OseaServerAfKey      * channel_af_key = NULL;
	

	// Check for correct params
	values = oseaserver_message_check_params (data, "session_id","server_name", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	
 	// Creating new af-key

 	rr_connection = rr_channel_get_connection (channel); 

	channel_af_key = oseaserver_afkeys_get_connection_key  (rr_connection);

	if (! channel_af_key) {
		/* Although there's no permissions for this service, you must
		   set an af-key before using it (you need to login before). */
		oseaserver_log_write ("Error. Trying to refresh an af-key before logging in.");
		oseaserver_message_error_answer (channel, msg_no, 
					   "You must log in before trying to refresh af-keys.",
					   OSEACOMM_CODE_ERROR);		
		return FALSE;
	}


	// Check if session is current
	if (! os_kernel_session_is_current (g_list_nth_data (values, 0))) {
		oseaserver_log_write ("Refresh af-key. Permission denied: session %s expired.",
				g_list_nth_data (values, 0));
		oseaserver_message_error_answer (channel, msg_no, "Session expired",
					   OSEACOMM_CODE_SESSION_EXPIRED);
		return FALSE;
	}

 	oseaserver_log_write ("Refreshing af-key for session_id %s and server %s", g_list_nth_data (values, 0),
			g_list_nth_data (values, 1)); 

	// Generating new afkey

	new_af_key = os_kernel_keys_generate (channel_af_key->user, g_list_nth_data (values, 1));

 	af_key_dataset = oseacomm_dataset_new (); 

 	oseacomm_dataset_add (af_key_dataset, new_af_key); 

	oseaserver_message_ok_answer (channel, msg_no, "Af-key refreshed", OSEACOMM_CODE_OK,
				af_key_dataset, NULL);
	
	return TRUE;
	
}
