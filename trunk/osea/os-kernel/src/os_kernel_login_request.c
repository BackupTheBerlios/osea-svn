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


#include <glib.h>
#include <liboseacomm/oseacomm.h>
#include <unistd.h>
#include <time.h>
#include <liboseaserver/oseaserver.h>
#include "os_kernel_keys.h"
#include "os_kernel_login_request.h"
#include "os_kernel_register_request.h"
#include "os_kernel_registry.h"
#include "os_kernel_crypto.h"
#include "os_kernel_session.h"



gboolean os_kernel_login_check (gchar * user, gchar * password) 
{
	OseaCommDataSet * dataset;
	gchar         * db_password;
	GString       * password_ciphered;

	g_return_val_if_fail (user, FALSE);
	g_return_val_if_fail (password, FALSE);

	dataset = oseaserver_command_execute_single_query ("SELECT passwd FROM kernel_user WHERE nick = '%s'",
						      user, NULL);

	if (oseacomm_dataset_get_height (dataset) == 0)
		return FALSE;
	
	db_password = (gchar *) oseacomm_dataset_get (dataset, 0, 0);

	
	password_ciphered = os_kernel_crypto_md5_sum (password);
	
	if (g_strcasecmp (password_ciphered->str, db_password)) {
		g_string_free (password_ciphered, TRUE);
		oseacomm_dataset_free (dataset);
		return FALSE;
	}
	
	g_string_free (password_ciphered, TRUE);
	oseacomm_dataset_free (dataset);
	
	return TRUE;
}

gboolean os_kernel_login_request (OseaCommXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	GList          * values;
	OseaCommDataSet  * session_id_dataset;
	OseaCommDataSet  * servers_dataset;
	OseaCommDataSet  * user_id_dataset;
	RRConnection   * rr_connection;
	gchar          * session_id = NULL;
	gchar          * af_key = NULL;
	

	// Check for correct params
	values = oseaserver_message_check_params (data, "user","password", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Check for login
	if (!os_kernel_login_check ((gchar *) g_list_nth_data (values, 0), 
				    (gchar *) g_list_nth_data (values, 1))) {
		oseaserver_log_write ("Login failed for user %s", g_list_nth_data (values, 0));
		oseaserver_message_error_answer (channel, msg_no, "Login failed",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
	oseaserver_log_write ("Login ok for user %s", g_list_nth_data (values, 0));
	
	// Login correct, create server table
	
	servers_dataset = os_kernel_registry_generate_session_info_table (g_list_nth_data (values, 0),
									  &af_key);
	
	// Set connection af-key for this user.

	rr_connection = rr_channel_get_connection (channel);

	oseaserver_afkeys_set_connection_key_simple (rr_connection, 
					       oseaserver_afkeys_parse_and_check (af_key));
	

	// Create session_id dataset

	user_id_dataset = oseaserver_command_execute_single_query ("select id from kernel_user where nick = '%s'",
						      (gchar *) g_list_nth_data (values, 0));

	session_id = os_kernel_session_new (atoi (oseacomm_dataset_get (user_id_dataset, 0, 0)), TRUE);
	session_id_dataset = oseacomm_dataset_new ();

	oseacomm_dataset_add (session_id_dataset, session_id);

	

	// Send all generated data to user

	oseaserver_message_ok_answer (channel, msg_no, "Login ok", OSEACOMM_CODE_OK, 
				session_id_dataset, servers_dataset, NULL);	

	return TRUE;
	
}


