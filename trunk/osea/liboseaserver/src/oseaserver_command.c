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

#include "oseaserver.h"
#include <stdio.h>
#include <stdarg.h>

#define LOG_DOMAIN "oseaserver-command"

GdaConnection * connection = NULL;

OseaCommDataSet * oseaserver_command_get_dataset          (GdaDataModel *  datamodel);

/**
 * oseaserver_command_execute_non_query:
 * @cmd_string: printf posix format string sql command.
 * @Varargs: Arguments for cmd_string
 * 
 * Execute a sql query which doesn't get any data such as UPDATE or
 * DELETE sentences. This function can be used as follow:
 *
 *   oseaserver_command_execute_non_query (connection, "DELETE * FROM anywhere");
 *
 * But also, as follow:
 * 
 *   oseaserver_command_execute_non_query (connection, "DELETE * FROM anywhere WHERE attr = '%s'", value);
 * 
 * Return value: TRUE if query was succesfully executed, otherwise return FALSE.
 **/
gboolean oseaserver_command_execute_non_query (gchar * cmd_string, ...)
{
	gchar          * cmd_result = NULL;
	GdaCommand     * command    = NULL;
	gboolean         result = TRUE;
	gint             error_code;
	va_list          args;

	g_return_val_if_fail (cmd_string, FALSE);

	// Create a database connection
	if (!connection) {
		connection = oseaserver_database_new_connection ();
		if (!connection) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Unable to create a database connection");
			return FALSE;
		}	
	}
	
	// Create sql query to database
	va_start (args, cmd_string);       
	cmd_result = g_strdup_vprintf (cmd_string, args);	
	va_end (args);

	command = gda_command_new ((const gchar *) cmd_result,
				   GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	
	error_code = gda_connection_execute_non_query (connection, command, NULL);

	if (error_code == -1) {
		oseaserver_log_write ("Unable to execute: \n%s\n", cmd_result);		
		result = FALSE;
	}
	
	gda_command_free (command);
	g_free (cmd_result);
	
	return result;
}



/**
 * oseaserver_command_execute_single_query:
 * @cmd_string: printf posix format string sql command.
 * @Varargs: Arguments for cmd_string
 *
 * Execute a sql query which gets data such as SELECT sentence. This function can be used as follow: 
 *
 *   oseaserver_command_execute_single_query ("SELECT * FROM anywhere");
 *
 * But also, as follow:
 * 
 *   oseaserver_command_execute_single_query ("SELECT * FROM anywhere WHERE attr = '%s'", value);
 * Return value: a dataset which contains the query data or NULL if something fails.
 **/
OseaCommDataSet *  oseaserver_command_execute_single_query (gchar * cmd_string, ...)
{
	gchar         * cmd_result = NULL;
	GdaCommand    * command    = NULL;
	GdaDataModel  * datamodel  = NULL;
	OseaCommDataSet * dataset    = NULL;
	va_list         args;

	g_return_val_if_fail (cmd_string, FALSE);

	// Create a database connection
	if (!connection) {
		connection = oseaserver_database_new_connection ();
		if (!connection) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Unable to create a database connection");
			return NULL;
		}	
	}

	// Create sql query to database
	va_start (args, cmd_string);

	cmd_result = g_strdup_vprintf (cmd_string, args);

	va_end (args);

	command = gda_command_new ((const gchar *) cmd_result,
				   GDA_COMMAND_TYPE_SQL, GDA_COMMAND_OPTION_STOP_ON_ERRORS);
	
	datamodel = gda_connection_execute_single_command (connection, command, NULL);

	gda_command_free (command);

	if (!datamodel) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Unable to execute: \n%s\n",
		       cmd_result);

		g_free (cmd_result);
		return NULL;		
	}

	dataset = oseaserver_command_get_dataset (datamodel);

	g_free (cmd_result);
	g_object_unref (datamodel);		


	return dataset;
}

/**
 * oseaserver_command_get_dataset:
 * @datamodel: A GdaDataModel to extract data from.
 * 
 * Convenienve function to translate a given @datamodel into a
 * OseaCommDataSet structure.
 * 
 * Return value: A translated OseaCommDataSet or NULL if fails.
 **/
OseaCommDataSet * oseaserver_command_get_dataset          (GdaDataModel *  datamodel)
{
	gint rows, columns, i, j;
	OseaCommDataSet * result;
	GdaValue      * value     = NULL;
	gchar         * str_value = NULL;

	g_return_val_if_fail (datamodel, NULL);

	rows = gda_data_model_get_n_rows (datamodel);
	columns = gda_data_model_get_n_columns (datamodel);
	result = oseacomm_dataset_new ();

	for (i = 0; i < rows; i++) {
		for (j = 0; j < columns; j ++) {
			
			value = gda_value_copy ( gda_data_model_get_value_at (datamodel, j, i) );
			str_value = gda_value_stringify (value);

			oseacomm_dataset_add (result, str_value);
			
			g_free (str_value);

			if (value->type != GDA_VALUE_TYPE_NUMERIC)
				gda_value_free (value);
			else {
				#warning We must remove this if when gda fix its gda_value_copy bug
				
			}
				
		}
		
		if (i == (rows - 1))
			break;
		
		oseacomm_dataset_new_row (result);
	}

	return result;
}


void        oseaserver_command_close_connection         ()
{
	if (connection)
		gda_connection_close (connection);
	return;
}
