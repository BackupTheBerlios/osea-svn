//  Copyright (C) 2003  Advanced Software Production Line, S.L.
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
#include <librr/rr.h>
#include <coyote/coyote.h>
#include <time.h>
#include "oseaserver.h"

#define LOG_DOMAIN "oseaserver_main"

static gboolean is_accepting_connections = FALSE;

gint oseaserver_main_update_services_option = 0;
gint oseaserver_main_version_option = 0;


struct poptOption  __oseaserver_main_no_option[] = {POPT_TABLEEND};

struct poptOption * oseaserver_main_no_extra_options ()
{
	return __oseaserver_main_no_option;
}

void oseaserver_main_complete_message (RRChannel * channel,
				 RRFrame * frame,
				 GString * message,
				 gpointer user_data,
				 gpointer custom_data)
{
	CoyoteXmlMessage * xml_message = NULL;
	CoyoteXmlServiceData * data;

	g_return_if_fail (channel);
	g_return_if_fail (message);

	switch (frame->type) {
	case RR_FRAME_TYPE_MSG:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "It's a MSG frame.");
		// obtain the instance config
		
		xml_message = g_new (CoyoteXmlMessage, 1);
		xml_message->content = message->str;
		xml_message->len = message->len;

		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Validating...");

		// validate message

		if (!coyote_xml_validate_message (xml_message)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Validation failed");
			oseaserver_message_error_answer (channel, frame->msgno, "XML Validation failed",
						   COYOTE_CODE_XML_VALIDATION_PROBLEM);
			return;
		} else 	
			// parse message
			if (!(data = coyote_xml_parse_message (xml_message))) {
				g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Message parsing failed");
				oseaserver_message_error_answer (channel, frame->msgno, "XML parse failed", 
							   COYOTE_CODE_XML_PARSE_PROBLEM);
				return;
			} else {				
				g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Trying to execute the parsed service");
				oseaserver_services_process (data, channel, frame->msgno);
				return;
			}
		
		break;
		
	case RR_FRAME_TYPE_RPY:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "It's a RPY frame.");
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Shouldn't receive a RPY frame here");
		// Server should not receive RPY messages through a listening channel, so 
		// we do nothing.
		break;

	case RR_FRAME_TYPE_ANS:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "It's a RPY frame.");
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Shouldn't receive a RPY frame here");
		// Server should not receive ANS messages through a listening channel, so 
		// we do nothing.
		break;

	case RR_FRAME_TYPE_ERR:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "It's a ERR frame.");
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Shouldn't receive a ERR frame here");
		// Server should not receive ERR messages through a listening channel, so 
		// we do nothing.
		break;
	case RR_FRAME_TYPE_UNKNOWN:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "It's a Unknown frame.");
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Doing nothing. ");
		break;
	case RR_FRAME_TYPE_NUL:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "It's a Nul frame.");
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Doing nothing. ");
		break;
	}

	return;
}

gboolean oseaserver_main_close_indication   (RRChannel *channel, gint code,
				       const gchar *xml_lang, 
				       const gchar *diagnostic,
				       GError **error)
{
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Received a close indication.");


	// We must return TRUE if the channel can be closed, FALSE if it cannot be closed.

	return TRUE;
}

void     oseaserver_main_close_confirmation   (RRChannel *channel, gint code,
					 const gchar *xml_lang, 
					 const gchar *diagnostic)
{
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Recieved a close confirmation.");
	return;
}


gboolean oseaserver_main_server_init (RRChannel *channel,  const gchar *piggyback, GError **error)
{
	if (! is_accepting_connections) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Server not allowing connections: not registered yet.");
		return FALSE;
	}

	// initialize the config for the particular instance of the channel

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Server init");
	channel->instance_config = coyote_simple_cfg_new ();

	rr_channel_set_aggregate (channel, FALSE);

	return TRUE;
}

/**
 * oseaserver_main_set_is_accepting_connections:
 * @data: 
 * @user_data: 
 * 
 * 
 * 
 * Return value: 
 **/
gboolean oseaserver_main_set_is_accepting_connections (AfDalNulData * data, gpointer user_data)
{
	
	is_accepting_connections = data->state;
	
	if (data->state == AFDAL_OK) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Server will allow connections.");
		oseaserver_log_write ("Register ok, accepting connections");
	}else {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Server won't allow connections.");
		g_print ("Register failed, maybe you need to install or update your server with --update-services. Exiting\n");
		oseaserver_log_write ("Register failed. Exiting.");
		exit (-1);
	}

	return TRUE;
}


void oseaserver_main_print_version_info (gchar *server_name, gchar *server_version, glong server_compilation_date)
{
	g_print ("%s version %s, compiled %s", server_name, server_version,
		 ctime (&server_compilation_date));
	oseaserver_print_version_info ();
	afdal_print_version_info ();
}



/**
 * oseaserver_main_run_server:
 * @server_name: Name of the server to be launched.
 * @server_description: Description of the server to be launched.
 * @argc: Pointer to argc (por parsing and modifying argc)
 * @argv: Pointer to argv (por parsing and modifying argv)
 * @config_file: file where server must read
 * @accepted_keys: array of keys accepted for the config file. 
 * @services_provided: array of services provided by this server 
 * @services_version: version of this version
 * @check_function: function this function will execute after initializing database and configuration files
 * 
 * Starts a new AF server with given parameters.
 **/
void oseaserver_main_run_server (gchar                * server_name,
			   gchar                * server_description,
			   gchar                * server_version,
			   glong                  server_compilation_date,
			   gint                 * argc, 
			   gchar               ** argv[], 
			   gchar                * config_file,
			   gchar                * accepted_keys [], 
			   AfgsServicesProvided   services_provided [],
			   gint                   services_version,
			   AfgsMainCallback       check_function,
			   struct poptOption    * server_extra_popt_option
			   )
{
	poptContext         ctx;
	AfgsConfiguration * afcfg;
	GError            * error = NULL;
	gchar             * port;
	CoyoteSimpleCfg   * config;
	RRProfileRegistry * beep_profile;
	RRListener        * server;
	struct poptOption   oseaserver_main_popoptions[] = { 
		{ "update-services", 'u', POPT_ARG_NONE, &oseaserver_main_update_services_option, 0, "Send a update-services message to af-kernel for refreshing permission data", NULL},
		{ "version", 'v', POPT_ARG_NONE, &oseaserver_main_version_option, 0, "Show version info", NULL},
//		{ NULL, '\0', POPT_ARG_INCLUDE_TABLE, server_extra_popt_option, 0, "Specific server options", NULL},
		POPT_AUTOHELP
		POPT_TABLEEND };
	struct poptOption   oseaserver_main_kernel_popoptions[] = { 
		{ "version", 'v', POPT_ARG_NONE, &oseaserver_main_version_option, 0, "Show version info", NULL},
		{ NULL, '\0', POPT_ARG_INCLUDE_TABLE, server_extra_popt_option, 0, "Specific server options", NULL},
		POPT_AUTOHELP
		POPT_TABLEEND
	};

	if (! strcmp ("af-kernel", server_name)) {
		is_accepting_connections = TRUE;
		ctx = poptGetContext (server_name, *argc, (const gchar **) *argv, oseaserver_main_kernel_popoptions, 0);
	} else
		ctx = poptGetContext (server_name, *argc, (const gchar **) *argv, oseaserver_main_popoptions, 0);

	// Default system handler
	oseaserver_signal_handler_init ();

	poptGetNextOpt(ctx);
	
	// Before anything check for --version flag
	if (oseaserver_main_version_option) {
		oseaserver_main_print_version_info (server_name, server_version, server_compilation_date);
		return;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Starting %s...", server_name);

	// Check for secure and properly config file permission.
	oseaserver_config_check_permissions (config_file);

	afcfg = oseaserver_config_load (config_file, accepted_keys);

	if (!afcfg)
		oseaserver_main_abort ("couldn't load config file");

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "config file loaded");


	oseaserver_log_write ("%s server starting ...", server_name);

	// Initialize afdal
	afdal_init ();

	// Initialize road runner
	if (!coyote_init (argc, argv, &error)) 
		oseaserver_main_abort ("failed to call coyote_init: %s", error->message);
	
	// Initialize database connection
	if (! oseaserver_database_init (server_name, "Database Connection", *argc, *argv))
		oseaserver_main_abort ("couldn't initialize database access");

      	// Tell liboseaserver which services we support
	oseaserver_services_set (services_provided);


	if (strcmp ("af-kernel", server_name)) {
		// Check for --update-services param
		
		if (oseaserver_main_update_services_option) {
			if (! oseaserver_services_update (server_name, server_description, services_version)) {
				oseaserver_main_abort ("unable to update services supported");
				return;
			}
			g_print ("Services updated successfully\n");
			return;
		}
		
		
		// Now we try to register our service in kernel
		if (! oseaserver_reg_register (server_name, services_version, afgs_main_set_is_accepting_connections)) {
			oseaserver_main_abort ("couldn't register our service in kernel");
		}
	} 

	if (check_function)
		check_function ();

	// Create the profile registry that will hold our profiles.
	beep_profile = rr_profile_registry_new ();
	
	// Create a config object
	config = coyote_simple_cfg_new ();
	
	coyote_simple_cfg_set_complete_message (config, oseaserver_main_complete_message, NULL, NULL);	
	coyote_simple_cfg_set_close_indication (config, oseaserver_main_close_indication);	
	coyote_simple_cfg_set_close_confirmation (config, oseaserver_main_close_confirmation);	
	coyote_simple_cfg_set_server_init (config, oseaserver_main_server_init, NULL);
		
	// Add to the supported profiles coyote_simple
	rr_profile_registry_add_profile (beep_profile, TYPE_COYOTE_SIMPLE, config);


        port = oseaserver_config_get (NULL, "listening port");

	if (!port)
		oseaserver_main_abort ("couldn't find 'listening port' key in config file");

	// Start the server 

	server = rr_tcp_listener_new (beep_profile, "0.0.0.0", atoi (port), &error);

	if (!server) {
		oseaserver_main_abort ("while starting the server  %s\n", error->message);
		exit (0);
	}
	
	if (!rr_wait_until_done (&error)) {
		oseaserver_main_abort ("unexpected error %s\n", error->message);
	}
	
	if (!rr_listener_shutdown (server, &error)) {
		oseaserver_main_abort ("shutting down error: %s\n", error->message);
	}
	
	if (!rr_exit (&error))
		oseaserver_main_abort ("unexpected error while exitting from rr_init: %s\n", error->message);
	
	oseaserver_config_destroy (NULL);

	oseaserver_database_quit ();

	return;
}

/**
 * oseaserver_main_abort:
 * @format: 
 * @: 
 * 
 * Convenience function for write an error message in the log file and
 * in the std out. After that, this function will exit.
 **/
void oseaserver_main_abort (gchar * format, ...)
{
	gchar   * error;

	va_list   args;
	va_start (args, format);

	error = g_strdup_printf ("Error: %s\n", format);
	oseaserver_log_vwrite (error, args);
	g_vfprintf (stderr, error, args);
	oseaserver_log_write ("Exiting..");

	g_free (error);
	va_end (args);

	oseaserver_database_quit ();

	exit (-1);
}
