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

#include "oseaserver_keys.h"
#include <stdlib.h>
#include <string.h>
#include <glib-object.h>

#define LOG_DOMAIN "oseaserver_afkeys"

/**
 * oseaserver_afkeys_parse_and_check:
 * @af_key: key to be parsed
 * 
 * This function try to parse the given @af_key.
 * 
 * Return value: A parsed af-key or NULL if something fails.
 **/
OseaServerAfKey * oseaserver_afkeys_parse_and_check      (gchar     * af_key)
{
	OseaServerAfKey  * result_key;
	gchar      ** splited_string;
	gchar      ** argv_permission;
	gint          int_time_stamp;
	gchar       * aux_string;
	gint          iterator;

	g_return_val_if_fail (af_key, NULL);

	// Get all fields contained at af_key.

	splited_string = g_strsplit (af_key, ";", 4);
	
	// Check af-key sintax
	g_return_val_if_fail (splited_string[0], NULL);
	g_return_val_if_fail (splited_string[1], NULL);
	g_return_val_if_fail (splited_string[2], NULL);
	g_return_val_if_fail (splited_string[3], NULL);
	
	// All fields seems to be good

	// Get af-key time_stamp
	int_time_stamp = strtol (splited_string[2], &aux_string, 10);
	if (strlen (aux_string) != 0) {
		g_strfreev (splited_string);
		return FALSE;
	}
	
	// Get af-key user and associated server
	result_key = g_new0 (OseaServerAfKey, 1);

	result_key->user = g_strdup (splited_string[0]);
	result_key->satellite_server = g_strdup (splited_string[1]);
	result_key->time_stamp = int_time_stamp;
	
	// Get permission list
	argv_permission = g_strsplit (splited_string[3],",", 0);

	for (iterator = 0; argv_permission[iterator]; iterator++) {
		result_key->permission_list = g_list_append (result_key->permission_list, 
							     g_strdup(argv_permission[iterator]));
	}
	
	
	// Free memory
	g_strfreev (splited_string);
	g_strfreev (argv_permission);

	return result_key; 
}

/**
 * oseaserver_afkeys_destroy:
 * @af_key: 
 * @free_permission_list: 
 * 
 * Frees all memory allocated by @af_key.
 **/
void         oseaserver_afkeys_destroy (OseaServerAfKey * af_key, 
				  gboolean     free_permission_list)
{

	g_return_if_fail (af_key);

	g_free (af_key->user);
	g_free (af_key->satellite_server);

	if (free_permission_list) {
		for (af_key->permission_list = g_list_first (af_key->permission_list);
		     af_key->permission_list;
		     af_key->permission_list = g_list_next (af_key->permission_list)) {
			g_free ((gchar *)af_key->permission_list->data);
		}
		
		g_list_free (af_key->permission_list);
	}
	
	g_free (af_key);

	return;
	
}
static void __oseaserver_afkeys_set_connection_key_free (gpointer data) 
{
	g_free (data);

	return;
}

/**
 * oseaserver_afkeys_set_connection_key_simple:
 * @connection: where the key is going to be set.
 * @af_key: an af-key to be set
 * 
 * This function set up the connection key to be used to check if an
 * incoming petition can be served.
 * 
 **/
void        oseaserver_afkeys_set_connection_key_simple (RRConnection *connection,
						   OseaServerAfKey    *af_key)
{
 	GObject * object;
  	
 	g_return_if_fail (connection);
 	g_return_if_fail (af_key);
 	
 	
 	object = G_OBJECT (connection);
  
 	g_object_set_qdata_full (object, 
  				 g_quark_from_static_string ("af-key"),
  				 af_key,
  				 __oseaserver_afkeys_set_connection_key_free);
  	
  
 	return;
}

/**
 * oseaserver_afkeys_set_connection_key:
 * @data: 
 * @user_data: 
 * @channel: 
 * @msg_no: 
 * 
 * This is the server callback to be executed when an incoming
 * set_connection_key petition is recieved. 
 * 
 * Return value: 
 **/
gboolean    oseaserver_afkeys_set_connection_key   (OseaCommXmlServiceData * data,
					      gpointer               user_data,
					      RRChannel            * channel,
					      gint                   msg_no)
{
	GList        * values;
	RRConnection * connection = rr_channel_get_connection (channel);
	OseaServerAfKey    * af_key = NULL;
	

	// Check for correct params
	values = oseaserver_message_check_params (data, "af-key", NULL);
	if (!values) {
		// Params seems to be incorrect
		oseaserver_message_error_answer (channel, msg_no, "Incorrect parameters form", 
					   OSEACOMM_CODE_XML_INCORRECT_PARAMETER);
		return FALSE;
	}

	af_key = oseaserver_afkeys_parse_and_check ((gchar *) g_list_nth_data (values, 0));

	if (! af_key) {
		oseaserver_message_error_answer (channel, msg_no, "Invalid key",
					   OSEACOMM_CODE_ERROR);
		return FALSE;
	}
			
	g_object_set_qdata_full (G_OBJECT(connection), 
				 g_quark_from_static_string ("af-key"),
				 af_key,
				 __oseaserver_afkeys_set_connection_key_free);
	
	oseaserver_message_ok_answer (channel, msg_no, "Key established", OSEACOMM_CODE_OK, NULL);

	return TRUE;
}

/**
 * oseaserver_afkeys_get_connection_key:
 * @connection: 
 * 
 * Gets the connection key set.
 * 
 * Return value: An OseaServerAfkey or NULL if something fails.
 **/
OseaServerAfKey    * oseaserver_afkeys_get_connection_key   (RRConnection * connection)
{
	GObject * object;

	g_return_val_if_fail (connection, NULL);

	object = G_OBJECT (connection);
	
	return g_object_get_qdata (object, g_quark_from_static_string ("af-key"));
}


/**
 * oseaserver_afkeys_check_service_permission:
 * @channel: 
 * @service_name: service name to be checked
 * @msg_no: 
 * 
 * This function is used by liboseaserver to check if a service petition can
 * be served by checking af-key set on this channel.
 * 
 * Return value: 
 **/
gboolean     oseaserver_afkeys_check_service_permission (RRChannel * channel, gchar * service_name, gint msg_no)
{
	RRConnection * connection;
	OseaServerAfKey    * af_key   = NULL;
	GList        * list_aux = NULL;
	time_t         time_stamp;
	
	connection = rr_channel_get_connection (channel);

	af_key = oseaserver_afkeys_get_connection_key (connection);
	
	if (!af_key) {
		oseaserver_message_error_answer (channel, msg_no, "AfKey unset for this connection",
					   OSEACOMM_CODE_KEY_REQUIRED);
		return FALSE;
	}
	
	// Now we must check if session is valid (or if the time stamp has expired)

	time_stamp = time (NULL);

	if (! oseaserver_config_get (NULL, "afkey expiration")) {
		oseaserver_message_error_answer (channel, msg_no, "Your af-key has expired. You must refresh your af-key.",
					   OSEACOMM_CODE_KEY_EXPIRED);
		return FALSE;
	}

	if (time_stamp > (af_key->time_stamp + atol (oseaserver_config_get (NULL, "afkey expiration")))) {
		// Session has expired. Asking for a new af_key
		oseaserver_message_error_answer (channel, msg_no, "Your af-key has expired. You must refresh your af-key.",
					   OSEACOMM_CODE_KEY_EXPIRED);
		return FALSE;
	}
	
	for (list_aux = g_list_first (af_key->permission_list); list_aux; list_aux = g_list_next (list_aux)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Checking for %s with %s\n", service_name, (gchar *) list_aux->data);
		if (!g_strcasecmp ((gchar *) list_aux->data, service_name))  
			return TRUE;
	}
	
	oseaserver_message_error_answer (channel, msg_no, "You don't have enough permissions",
				   OSEACOMM_CODE_INSUFFICIENT_PERMISSIONS);
	
	return FALSE;
	
}
