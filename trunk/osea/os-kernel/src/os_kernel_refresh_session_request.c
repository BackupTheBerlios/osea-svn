//  Af-Kernel server: 
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


#include <glib.h>
#include <liboseacomm/oseacomm.h>
#include <unistd.h>
#include <time.h>
#include <liboseaserver/oseaserver.h>
#include "os_kernel_keys.h"
#include "os_kernel_login_request.h"
#include "os_kernel_register_request.h"
#include "os_kernel_crypto.h"
#include "os_kernel_session.h"


gboolean os_kernel_refresh_session_request (OseaCommXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	GList          * values;
	OseaCommDataSet  * session_id_dataset;
	gchar          * session_id = NULL;
	

	// Check for correct params
	values = oseaserver_message_check_params (data, "user","password", "session_id", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Check for login
	if (!os_kernel_login_check ((gchar *) g_list_nth_data (values, 0), 
				    (gchar *) g_list_nth_data (values, 1))) {
		oseaserver_log_write ("Refresh session failed for user %s", g_list_nth_data (values, 0));
		oseaserver_message_error_answer (channel, msg_no, "Login failed",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}

	if (! os_kernel_session_belongs ((gchar *) g_list_nth_data (values, 2),
					 (gchar *) g_list_nth_data (values, 0))) {
		oseaserver_log_write ("Refresh session failed for user %s", g_list_nth_data (values, 0));
		oseaserver_message_error_answer (channel, msg_no, "Session not owned by user",
					   OSEACOMM_CODE_ERROR);
		return FALSE;

	}

	oseaserver_log_write ("Refresh session for user %s", g_list_nth_data (values, 0));
	
	// Create session_id dataset
	session_id = os_kernel_session_new ((gchar *) g_list_nth_data (values, 0), TRUE);
	os_kernel_session_remove ((gchar *) g_list_nth_data (values, 2));
	
	session_id_dataset = oseacomm_dataset_new ();

	oseacomm_dataset_add (session_id_dataset, session_id);
	

	// Send all generated data to user

	oseaserver_message_ok_answer (channel, msg_no, "Session refresh ok", OSEACOMM_CODE_OK, 
				session_id_dataset,  NULL);
	
	return TRUE;
	
}


