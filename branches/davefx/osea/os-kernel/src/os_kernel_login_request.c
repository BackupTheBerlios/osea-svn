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
#include <coyote/coyote.h>
#include <unistd.h>
#include <time.h>
#include <afgs/afgs.h>
#include "os_kernel_keys.h"
#include "os_kernel_login_request.h"
#include "os_kernel_register_request.h"
#include "os_kernel_registry.h"
#include "os_kernel_crypto.h"
#include "os_kernel_session.h"



gboolean os_kernel_login_check (gchar * user, gchar * password) 
{
	CoyoteDataSet * dataset;
	gchar         * db_password;
	GString       * password_ciphered;

	g_return_val_if_fail (user, FALSE);
	g_return_val_if_fail (password, FALSE);

	dataset = afgs_command_execute_single_query ("SELECT passwd FROM kernel_user WHERE nick = '%s'",
						      user, NULL);

	if (coyote_dataset_get_height (dataset) == 0)
		return FALSE;
	
	db_password = (gchar *) coyote_dataset_get (dataset, 0, 0);

	
	password_ciphered = os_kernel_crypto_md5_sum (password);
	
	if (g_strcasecmp (password_ciphered->str, db_password)) {
		g_string_free (password_ciphered, TRUE);
		coyote_dataset_free (dataset);
		return FALSE;
	}
	
	g_string_free (password_ciphered, TRUE);
	coyote_dataset_free (dataset);
	
	return TRUE;
}

gboolean os_kernel_login_request (CoyoteXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	GList          * values;
	CoyoteDataSet  * session_id_dataset;
	CoyoteDataSet  * servers_dataset;
	CoyoteDataSet  * user_id_dataset;
	RRConnection   * rr_connection;
	gchar          * session_id = NULL;
	gchar          * af_key = NULL;
	

	// Check for correct params
	values = afgs_message_check_params (data, "user","password", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Check for login
	if (!os_kernel_login_check ((gchar *) g_list_nth_data (values, 0), 
				    (gchar *) g_list_nth_data (values, 1))) {
		afgs_log_write ("Login failed for user %s", g_list_nth_data (values, 0));
		afgs_message_error_answer (channel, msg_no, "Login failed",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}
	afgs_log_write ("Login ok for user %s", g_list_nth_data (values, 0));
	
	// Login correct, create server table
	
	servers_dataset = os_kernel_registry_generate_session_info_table (g_list_nth_data (values, 0),
									  &af_key);
	
	// Set connection af-key for this user.

	rr_connection = rr_channel_get_connection (channel);

	afgs_afkeys_set_connection_key_simple (rr_connection, 
					       afgs_afkeys_parse_and_check (af_key));
	

	// Create session_id dataset

	user_id_dataset = afgs_command_execute_single_query ("select id from kernel_user where nick = '%s'",
						      (gchar *) g_list_nth_data (values, 0));

	session_id = os_kernel_session_new (atoi (coyote_dataset_get (user_id_dataset, 0, 0)), TRUE);
	session_id_dataset = coyote_dataset_new ();

	coyote_dataset_add (session_id_dataset, session_id);

	

	// Send all generated data to user

	afgs_message_ok_answer (channel, msg_no, "Login ok", COYOTE_CODE_OK, 
				session_id_dataset, servers_dataset, NULL);	

	return TRUE;
	
}


