//  ASPL Fact Server Database Connection: 
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

#include <libgda/libgda.h>
#include <config.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <coyote/coyote.h>
#include "oseaserver_config.h"
#include "oseaserver_database.h"

#define LOG_DOMAIN "oseaserver_database"

static GdaClient   * db_client = NULL;
static const gchar * gda_config_name = NULL;


gboolean create_libgda_directory ()
{
	gchar *dir_name = NULL;
	gchar *home = getenv("HOME");

	g_return_val_if_fail (home, FALSE);

	dir_name = g_build_filename (home, ".libgda", NULL);

	if (!g_file_test (dir_name, (G_FILE_TEST_EXISTS | 
				      G_FILE_TEST_IS_DIR))) {
		
		if (mkdir (dir_name, 0770) < 0) {
			g_printerr ("Error: couldn't create %s.", dir_name);
			g_free (dir_name);
			return FALSE;
		}
	}

	g_free (dir_name);
	
	return TRUE;
}


gboolean oseaserver_database_create_gda_config (const gchar *connection_description)
{
	GString *string = NULL;

	gchar * server = oseaserver_config_get (NULL, "database server");
	gchar * port   = oseaserver_config_get (NULL, "database port");
	gchar * db     = oseaserver_config_get (NULL, "database name");

	if (!server) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'database server' key in config file");
		return FALSE;
	}

	if (!port) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'database port' key in config file");
		return FALSE;
	}

	if (!db) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'database name' key in config file");
		return FALSE;
	}


	if (!create_libgda_directory ()) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't create ~/.libgda directory, neccesary for database connection.");
		return FALSE;
	}


	string = g_string_new ("");

	if (strlen (port) != 0)
		g_string_sprintf (string, "HOST=%s;PORT=%s;DATABASE=%s", server, port, db);	
	else
		g_string_sprintf (string, "HOST=%s;DATABASE=%s", server, db);

	gda_config_save_data_source     (gda_config_name,
					 "PostgreSQL",
					 string->str,
					 connection_description,
					 NULL, NULL);

	g_string_free (string, TRUE);

	return TRUE;

}


void print_notification (GdaClient *client, GdaConnection *cnc, GdaClientEvent event, GdaParameterList *params)
{
	GdaParameter *param = NULL;
	GdaError *error = NULL;
		
	switch (event) {
	case GDA_CLIENT_EVENT_INVALID:
		break;
	case GDA_CLIENT_EVENT_ERROR:
		param = gda_parameter_list_find (params, "error");
 		error = GDA_ERROR (gda_value_get_gobject (param->value)); 
 		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
 		       "\n\tError in GDA Client: %s", gda_error_get_description (error)); 
		break;
	case GDA_CLIENT_EVENT_CONNECTION_OPENED:
 		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
 		       "Connected to database");
		break;
	case GDA_CLIENT_EVENT_CONNECTION_CLOSED:
 		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
 		       "Disconnected from database");
		break;
	case GDA_CLIENT_EVENT_TRANSACTION_STARTED:
		break;
	case GDA_CLIENT_EVENT_TRANSACTION_COMMITTED:
		break;
	case GDA_CLIENT_EVENT_TRANSACTION_CANCELLED:
		break;
	default:
		break;
	}
}


/**
 * oseaserver_database_init:
 * @connection_name: connection short name 
 * @connection_description: connection description
 * @argc: Argument needed by gda initialization
 * @argv: Argument needed by gda initialization
 * 
 * Tries to initialize gda and make some enviroment checks such as
 * connect to the configured database and then disconnect to
 * figure out if it is alive.
 * This function must be called after a call to oseaserver_config_load.
 * 
 * Return value: TRUE is everything goes OK or FALSE in other case.
 **/
gboolean oseaserver_database_init (const gchar *connection_name, 
			     const gchar *connection_description, 
			     gint argc,
			     gchar **argv) 
{
	GdaConnection *cnc;
	gchar *user = NULL;
	gchar *password = NULL;
        
	
	g_return_val_if_fail (connection_name, FALSE);

	gda_config_name = connection_name;

	user = oseaserver_config_get (NULL, "database user");
	if (!user) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'database user' key in config file");
		return FALSE;
	}

	password = oseaserver_config_get (NULL, "database password");

	if (!password) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'database password' key in config file");
		return FALSE;
	}

	gda_init (PACKAGE, VERSION, argc, argv);

	if (!oseaserver_database_create_gda_config (connection_description))
		return FALSE;

	db_client = gda_client_new ();

	if (!db_client) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't create database client");
		return FALSE;
	}

	g_signal_connect (db_client, "event-notification", (GCallback) print_notification, NULL);
	
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       "Connecting to database. User='%s'. Password='%s'", user, password);

	cnc = gda_client_open_connection (db_client, gda_config_name, user, password, 0);

	if (!cnc) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't make connection to database.");
		return FALSE;
	}

	if (!gda_connection_close (cnc) ) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't close connection to database");
		return FALSE;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Disconnected from database");
		
	return TRUE;
}


/**
 * oseaserver_database_new_connection:
 * 
 * Creates a new connection to the configured database. This function
 * must be called after a call to oseaserver_database_init.
 * 
 * 
 * Return value: GdaConnection or NULL
 **/
GdaConnection *oseaserver_database_new_connection ()
{
	gchar *user = oseaserver_config_get (NULL, "database user");
	gchar *password = oseaserver_config_get (NULL, "database password");

	if (!user) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'database user' key in config file");
		return FALSE;
	}
	
	if (!password) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Couldn't find 'database password' key in config file");
		return FALSE;
	}
	
	return gda_client_open_connection (db_client, gda_config_name, user, password, 0);
}


/**
 * oseaserver_database_quit:
 * 
 * Shutdown all create connection to the configured database.
 * 
 * Return value: TRUE or FALSE if something fails.
 **/
gboolean oseaserver_database_quit ()
{
	gda_client_close_all_connections (db_client);

	g_object_unref(G_OBJECT (db_client));

	db_client = NULL;
	
	return TRUE;
}
