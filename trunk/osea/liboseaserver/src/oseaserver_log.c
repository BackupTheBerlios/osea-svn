//  ASPL Fact Log facilities: 
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

#include "oseaserver_log.h"
#include <glib.h>
#include <stdlib.h>
#include <time.h>

GIOChannel * log_file_opened = NULL;
gboolean     check_for_log   = FALSE;
gboolean     log_file_active = FALSE;

/**
 * oseaserver_log_write:
 * @log_msg: message with printf format
 * @Varargs: elements to be inserted following printf format
 * 
 * This function will send a message to the configured log file. This
 * function also uses 'log active' and 'log file' config values at
 * server.cfg config file. First value, 'log active', activates log
 * functionality. 
 * 
 **/
void oseaserver_log_write (gchar * log_msg, ...)
{
	va_list args;

	va_start (args, log_msg);

	oseaserver_log_vwrite (log_msg, args);

	va_end (args);
}


void oseaserver_log_vwrite (gchar * log_msg, va_list args)
{
	gchar      * log_file  = NULL;
	gchar      * message   = NULL;
	gchar      * time_char = NULL;
	gchar      * value     = NULL;
	GError     * error     = NULL;
	time_t       time_stamp;
	struct tm  * time_data = NULL;


	g_return_if_fail (log_msg);

	// Check for log active flag
	if (!check_for_log) {

		check_for_log = TRUE;
		value = oseaserver_config_get (NULL, "log active");
		
		if (value && *value && !g_strcasecmp (value, "yes"))
			log_file_active = TRUE;
	}
	
	if (!log_file_active)
		return;


	// Check for already opened log file
	if (log_file_opened  == NULL) {
		// Get log file config value
		log_file = oseaserver_config_get (NULL, "log file");
		if (!log_file) {
			g_critical ("Unable to get config value log file: %s\n", log_file);
			return;
		}

		if ((log_file_opened = g_io_channel_new_file (log_file, "a",  &error)) < 0) {
			g_critical ("Unable to open log file: %s, error: %s", log_file, error->message);
			g_error_free (error);
			log_file_opened = NULL;
			return;
		}
	}
	

	message = g_strdup_vprintf (log_msg, args);

	time (&time_stamp);
	
	time_data = gmtime (&time_stamp);
	

	time_char = g_strdup_printf ("%02d/%d -- %02d:%02d:%02d -- ",
				     time_data->tm_mon,
				     time_data->tm_mday, 
				     time_data->tm_hour, 
				     time_data->tm_min, 
				     time_data->tm_sec);

	if (g_io_channel_write_chars (log_file_opened, time_char, -1, NULL, &error) == G_IO_STATUS_ERROR) {
		g_critical ("Unable to write message to log file: %s", error->message);
		g_error_free (error);
	}

	if (g_io_channel_write_chars (log_file_opened, message, -1, NULL, &error) == G_IO_STATUS_ERROR) {
		g_critical ("Unable to write message to log file: %s", error->message);
		g_error_free (error);
	}
		
	
	if (g_io_channel_write_chars (log_file_opened, "\n", -1, NULL, &error) == G_IO_STATUS_ERROR) {
		g_critical ("Unable to write message to log file: %s", error->message);
		g_error_free (error);
	}

	if (g_io_channel_flush (log_file_opened, &error) == G_IO_STATUS_ERROR) {
		g_critical ("Unable to write message to log file: %s", error->message);
		g_error_free (error);
	}

	g_free (message);
	
	return;	
}

void __oseaserver_log_fake_log_handler (const gchar *log_domain,
		       GLogLevelFlags log_level,
		       const gchar *message,
		       gpointer user_data)
{
	return;
}

void oseaserver_log_check_and_disable_glog () 
{
	
	if (! getenv("AF_DEBUG")) {
		g_log_set_handler (NULL, G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("COYOTE_CONNECTION", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("AFDAL_REQUEST", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("oseaclient_session", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("COYOTE_SIMPLE", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("COYOTE_XML", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("oseaserver_database", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("af-kernel_register_request", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("oseaserver_main", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("GLib", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
		g_log_set_handler ("oseaserver_afkeys", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
   		g_log_set_handler ("oseaserver_reg", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, __oseaserver_log_fake_log_handler, NULL);
	}
	
	return;
}
