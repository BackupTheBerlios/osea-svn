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

#include <liboseaserver/oseaserver.h>
#include <glib.h>
#include "os_kernel_session.h"
#include "os_kernel_crypto.h"

void     __os_kernel_session_add (gchar * session_id, gint user, gboolean expire)
{
	gboolean   result = FALSE;
	gchar    * expire_str = NULL;

	if (expire)
		expire_str = "TRUE";
	else
		expire_str = "FALSE";

	result = oseaserver_command_execute_non_query ("INSERT INTO Kernel_Session \
                 (id, user_id, stime, expire) VALUES \
                 ('%s', %d, current_timestamp, '%s');", session_id, user, expire_str);

	if (! result)
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't insert a new session into database");

}


void     __os_kernel_session_delete_non_expiring_sessions (gint user)
{
	oseaserver_command_execute_non_query ("DELETE FROM kernel_session WHERE \
              user_id = %d AND expire = 'FALSE'", user);

	return;
}


gchar *  os_kernel_session_new (gint user, gboolean expire)
{
	gchar * string = NULL;
	gchar * session_id = NULL;
	GTimeVal * time = g_new0 (GTimeVal, 1);
	GString  * md5 = NULL;

	g_get_current_time (time);
	
	string = g_strdup_printf ("%d-%ld", user, time->tv_sec);
	
	md5 = os_kernel_crypto_md5_sum (string);

	session_id = md5->str;
	g_string_free (md5, FALSE);
	
	
       	if (! expire)
		__os_kernel_session_delete_non_expiring_sessions (user);
	
	__os_kernel_session_add (session_id, user, expire);

	return session_id;
}



void     os_kernel_session_remove (gchar *id)
{
	oseaserver_command_execute_non_query ("DELETE FROM kernel_session WHERE \
              id = '%s'", id);
}


gboolean os_kernel_session_is_current (gchar *id)
{
	gchar               * session_expiration;
	glong                 session_exp;
	gboolean              result;
	OseaCommDataSet       * dataset;



	session_expiration = oseaserver_config_get (NULL, "session expiration");
	if (! session_expiration)
		return FALSE;

	session_exp = atol (session_expiration);
	g_free (session_expiration);

	oseaserver_command_execute_non_query ("DELETE FROM Kernel_Session WHERE stime < CURRENT_TIMESTAMP - INTERVAL '%ld seconds' AND expire = 'TRUE'", session_exp);

	oseaserver_command_execute_non_query ("UPDATE Kernel_Session SET stime = current_timestamp WHERE id='%s'", id);

	dataset = oseaserver_command_execute_single_query ("SELECT id FROM kernel_session WHERE id = '%s'",
						      id);	

	if (oseacomm_dataset_get_height (dataset) < 1)
		result = FALSE;
	else
		result = TRUE;

	oseacomm_dataset_free (dataset);

	return result;
	
}


gboolean os_kernel_session_refresh (gchar *id)
{
	gboolean result = oseaserver_command_execute_non_query ("UPDATE Kernel_Session SET stime = current_timestamp WHERE id='%s'", id);

	return result;
}



