//  ASPL Clm: Command Line Manager for ASPL Fact system
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
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <liboseacomm/oseacomm.h>
#include <popt.h>
#include <time.h>
#include <config.h>
#include <liboseaclient/oseaclient.h>
#include <libaoskernel/aos_kernel.h>
//#include "auto_completion.h"
#include "help.h"
#include "commands.h"
#include "connection.h"


AllowedCommands * commands = NULL;

gint version_option = 0;
struct poptOption popoptions[] = { 
	{ "version", 'v', POPT_ARG_NONE, &version_option, 0, "Show version info", NULL},
	POPT_AUTOHELP
	POPT_TABLEEND
};


void initialize_readline ()
{
//	if (rl_bind_key ('\t', auto_completion_run) != 0) 
//		exit (-1);

	return;
}

void fake_log_handler (const gchar *log_domain,
		       GLogLevelFlags log_level,
		       const gchar *message,
		       gpointer user_data)
{
	return;
}

void initialize_log_handlers ()
{

	if (! getenv("AF_DEBUG")) {
		g_log_set_handler (NULL, G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("COYOTE_CONNECTION", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("OSEACLIENT_REQUEST", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("COYOTE_SIMPLE", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("COYOTE_XML", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
	}

	return;
}

int main (int argc, char *argv[])
{

	gchar            * line = NULL;
	GString          * prompt = NULL;
	CommandCallback    command_handler ;
	GError           * error;
	poptContext        ctx;
	glong              cd = COMPILATION_DATE;
	
	ctx = poptGetContext (PACKAGE, argc, (const gchar **) argv, popoptions, 0);

	poptGetNextOpt(ctx);

	if (version_option) {
		g_print ("ASPL Command Line Manager version %s, compiled %s", VERSION, ctime(&cd));
		oseaclient_print_version_info ();
		aos_kernel_print_version_info ();
		return 0;
	}
		
	oseaclient_init ();
	
	help_initial_help ();

	// Initialize readline.
	initialize_readline ();

	// Initialize autocompletion module.
//	auto_completion_init ();

	// Initialize log_handler
	initialize_log_handlers ();

	// Get all avaliable commands
	commands = commands_get ();

	

	while (TRUE) {
		// Create current prompt
		if (connection_get ()) {
			prompt = g_string_new (NULL);
			g_string_sprintf (prompt, "\n[%s] aspl-clm > ", connection_get_server_name ());
		}else
			prompt = g_string_new ("\n[not connected] aspl-clm > ");

		// Wait until read a command
		line = readline (prompt->str);
		
		// Check if command is supported.
		if (!commands_is_allowed (commands, line)) {
			// Command not recognized.
			g_print ("Command '%s' not recognized. Try:  help\n", line);
			g_free (line);
			g_string_free (prompt, TRUE);
			continue;
		}
		
		// It's a recognized command so save it 
		add_history (line);

		// Executed command associated handler
		command_handler = commands_get_callback (commands, line);

		command_handler (line, NULL);

		g_string_free (prompt, TRUE);
	}

	// Exit from road runner
	if (!rr_exit (&error))
		g_error ("rr_exit failed: %s\n", error->message);
	
	return 0;
}
