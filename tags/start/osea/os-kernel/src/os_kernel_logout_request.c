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
#include <time.h>
#include <afgs/afgs.h>
#include "os_kernel_logout_request.h"
#include "os_kernel_session.h"

gboolean os_kernel_logout_request (CoyoteXmlServiceData *data, gpointer user_data, RRChannel * channel, gint msg_no)
{
	GList * values;

	values = afgs_message_check_params (data, "session_id", NULL);

	if (! values) {
		// Params seems to be incorrect
		afgs_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   COYOTE_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	// Remove session_id from session_module
	os_kernel_session_remove ((gchar *) g_list_nth_data (values, 0));

	// Params seems to be incorrect
	afgs_message_ok_answer (channel, msg_no, "Logout successful", COYOTE_CODE_OK, NULL);

	return TRUE;
}


