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
#include "os_kernel_crypto.h"
#include "os_kernel_session.h"


gboolean os_kernel_refresh_session_request (CoyoteXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	GList          * values;
	CoyoteDataSet  * session_id_dataset;
	gchar          * session_id = NULL;
	

	// Check for correct params
	values = afgs_message_check_params (data, "user","password", "session_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Check for login
	if (!os_kernel_login_check ((gchar *) g_list_nth_data (values, 0), 
				    (gchar *) g_list_nth_data (values, 1))) {
		afgs_log_write ("Refresh session failed for user %s", g_list_nth_data (values, 0));
		afgs_message_error_answer (channel, msg_no, "Login failed",
					   COYOTE_CODE_ERROR);
		return FALSE;
	}

	if (! os_kernel_session_belongs ((gchar *) g_list_nth_data (values, 2),
					 (gchar *) g_list_nth_data (values, 0))) {
		afgs_log_write ("Refresh session failed for user %s", g_list_nth_data (values, 0));
		afgs_message_error_answer (channel, msg_no, "Session not owned by user",
					   COYOTE_CODE_ERROR);
		return FALSE;

	}

	afgs_log_write ("Refresh session for user %s", g_list_nth_data (values, 0));
	
	// Create session_id dataset
	session_id = os_kernel_session_new ((gchar *) g_list_nth_data (values, 0), TRUE);
	os_kernel_session_remove ((gchar *) g_list_nth_data (values, 2));
	
	session_id_dataset = coyote_dataset_new ();

	coyote_dataset_add (session_id_dataset, session_id);
	

	// Send all generated data to user

	afgs_message_ok_answer (channel, msg_no, "Session refresh ok", COYOTE_CODE_OK, 
				session_id_dataset,  NULL);
	
	return TRUE;
	
}


