//
//  LibOseaClient: common functions to liboseaclient* level and architectural functions.
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


#include "oseaclient_session.h"
#define LOG_DOMAIN "oseaclient_session"


typedef struct __OseaClientSessionPrivateServer {
	gchar        * name;
	gchar        * host;
	gchar        * port;
	gchar        * af_key;
	RRConnection * connection;
} OseaClientSessionPrivateServer;


typedef struct __OseaClientSession {
	gchar      * session_id;
	OseaClientList  * server_info;
	gchar      * user;
} OseaClientSession;

static OseaClientSession         session = { NULL, NULL, NULL };
static OseaClientSessionServer * server_myself = NULL;
static gboolean             oseacomm_initialized = FALSE;
static GList              * active_servers = NULL;

void __oseaclient_session_server_destroy (gpointer data)
{
	OseaClientSessionPrivateServer *server = (OseaClientSessionPrivateServer *) data;
	GError *error;
	
	g_free (server->name);
	g_free (server->host);
	g_free (server->port);

	if (server->af_key)
		g_free (server->af_key);

	if (server->connection) {
		if (!rr_connection_disconnect (server->connection, &error)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "couldn't disconnect: %s\n",  error->message);
			g_error_free (error);
		}
		server->connection = NULL;
		g_free (server);
		
	}	
}


gchar * __oseaclient_session_extract_session_id (OseaCommDataSet * dataset)
{
	g_return_val_if_fail (dataset, NULL);

	return g_strdup (oseacomm_dataset_get (dataset, 0, 0));
}

OseaClientList * __oseaclient_session_extract_server_info (OseaCommDataSet * data, RRConnection *kernel_connection)
{
	OseaClientSessionPrivateServer * server;
	OseaClientList                 * result;
	gint i;

	g_return_val_if_fail (data, NULL);
	
	result = oseaclient_list_new_full ((GCompareFunc) g_strcasecmp, NULL, __oseaclient_session_server_destroy);

	for (i=0; i < oseacomm_dataset_get_height (data); i++) {
		server = g_new0 (OseaClientSessionPrivateServer,1);

		server->name = g_strdup (oseacomm_dataset_get (data, i, 0));
		
		if (! g_strcasecmp ("os-kernel", server->name))
			server->connection = kernel_connection;

		server->host = g_strdup (oseacomm_dataset_get (data, i, 1));
		server->port = g_strdup (oseacomm_dataset_get (data, i, 2));
		server->af_key = g_strdup (oseacomm_dataset_get (data, i, 3));

		oseaclient_list_insert (result, server->name, server);

	}

	return result;
}


static  void __oseaclient_session_login_process (RRChannel * channel, 
					    RRFrame   * frame,
					    GString   * message, 
					    gpointer    data, 
					    gpointer    custom_data)
{
	OseaClientMultiData  * oseaclient_multidata = NULL;
	OseaClientNulData    * oseaclient_data = NULL;
	GList           * list = NULL;
	OseaCommDataSet   * session_id_dataset = NULL;
	OseaCommDataSet   * server_info_dataset = NULL;
	
	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_multidata = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_MULTI_DATA, channel,
								       frame, message, NULL, &list, &data, &custom_data);

	g_return_if_fail (oseaclient_multidata);
	
	oseaclient_data = g_new0 (OseaClientNulData, 1);
	oseaclient_data->state = oseaclient_multidata->state;
	oseaclient_data->text_response = g_strdup (oseaclient_multidata->text_response);
	oseaclient_multi_free (oseaclient_multidata, FALSE);

	// If everything was OK, we must process the list of datasets
	// for extracting session_id and server_info



	if (oseaclient_data->state == OSEACLIENT_OK) {

		session_id_dataset = (OseaCommDataSet *) g_list_nth_data (list, 0);
		server_info_dataset = (OseaCommDataSet *) g_list_nth_data (list, 1);
		
		session.session_id  = __oseaclient_session_extract_session_id (session_id_dataset);
		session.server_info = __oseaclient_session_extract_server_info (server_info_dataset, channel->connection);
		
		oseacomm_dataset_free (session_id_dataset);
		oseacomm_dataset_free (server_info_dataset);

		g_list_free (list);
	} else {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Login failed: not retrieving session info: %s", oseaclient_data->text_response);
	}


	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_NUL_DATA, data, custom_data, oseaclient_data);

	return;
}



/**
 * oseaclient_session_login:
 * @usr: login user
 * @passwd: password associated
 * @kernel_hostname: hostname where os-kernel lives
 * @kernel_port: port where os-kernel is listening
 * @usr_function: function to be executed when the server response arrives.
 * @usr_data: user data passed to user_function.
 * 
 * This function tries to login to the os-kernel defined by @kernel_hostname and @kernel_port, using login @usr and password @passwd. 
 * When server's response arrives @usr_function will be
 * called with the given @usr_data.
 * 
 * Return value: TRUE if login request is sent or FALSE if not.
 **/
gboolean        oseaclient_session_login   (gchar * usr,
				       gchar * passwd,
				       gchar * kernel_hostname,
				       gchar * kernel_port,
				       OseaClientNulFunc usr_function,
				       gpointer usr_data)
{
	RRConnection * connection;
	GError       * error;

	g_return_val_if_fail (usr, FALSE);
	g_return_val_if_fail (passwd, FALSE);
	g_return_val_if_fail (kernel_hostname, FALSE);
	g_return_val_if_fail (kernel_port, FALSE);

	// Initialize oseacomm 
	if ((! oseacomm_initialized) && (!oseacomm_init ( NULL, NULL, &error))) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Error: unable to initialize oseacomm: %s\n", error->message);
		return FALSE;
	}

	connection = oseacomm_connection_new (kernel_hostname, kernel_port, TYPE_OSEACOMM_SIMPLE);

	if (! connection) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't connect to os-kernel in %s:%s", 
		       kernel_hostname, kernel_port);
		return FALSE;
	}
	
	if (session.user)
		g_free (session.user);
	session.user = g_strdup (usr);

	return oseaclient_request (connection, __oseaclient_session_login_process, 
			      (OseaClientFunc) usr_function, usr_data,
			      "login", 
			      "user", OSEACOMM_XML_ARG_STRING, usr, 
			      "password", OSEACOMM_XML_ARG_STRING, passwd, NULL);
	
}


gboolean __oseaclient_session_destroy_session_info (gpointer data)
{
	oseaclient_list_destroy (session.server_info);
	session.server_info = NULL;

	g_free (session.session_id);
	session.session_id = NULL;

	g_free (session.user);
	session.user = NULL;

	return FALSE;
}

void __oseaclient_session_logout_process (RRChannel * channel, 
				     RRFrame * frame,
				     GString * message, 
				     gpointer data, 
				     gpointer custom_data)
{
	OseaClientNulData     * oseaclient_data = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_data = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_NUL_DATA,
								  channel,
								  frame,
								  message,
								  NULL, 
								  NULL, &data, &custom_data);

	if (! oseaclient_data)
		return;

	// Now we program a event for destroying all session info, and activate it
	
	oseaclient_event_source_launch (__oseaclient_session_destroy_session_info, NULL);

	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_NUL_DATA, data, custom_data, oseaclient_data);


	return;
}


/**
 * oseaclient_session_logout:
 * @usr_function: function to be executed when the server response arrives.
 * @usr_data: user data passed to user_function.
 * 
 * Function used to get logged out from the system
 * 
 * Return value: TRUE if the query was executed, or FALSE if it wasn't
 **/
gboolean        oseaclient_session_logout  (OseaClientNulFunc usr_function,
				       gpointer usr_data)
{
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);

	if (! connection)
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't find connection for os-kernel");

	return oseaclient_request (connection, __oseaclient_session_logout_process, 
			      (OseaClientFunc) usr_function, usr_data,
			      "logout", 
			      "session_id", OSEACOMM_XML_ARG_STRING, session.session_id, 
			      NULL);
	
}





static  void __oseaclient_session_refresh_key_process (RRChannel * channel, 
						  RRFrame   * frame,
						  GString   * message, 
						  gpointer    data, 
						  gpointer    custom_data)
{
	OseaClientNulData              * oseaclient_nul_data = NULL;
	OseaClientData                 * oseaclient_data = NULL;
	OseaCommDataSet             * af_key_dataset = NULL;
	gchar                     * new_af_key = NULL;
	OseaClientAfKey                * key = NULL;
	OseaClientSessionPrivateServer * server = NULL;
	gchar                     * tmp;
	
	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_data = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_DATA, channel,
								  frame, message, &af_key_dataset, 
								  NULL, &data, &custom_data);

	if (! oseaclient_data)
		return;

	oseaclient_nul_data = g_new0 (OseaClientNulData, 1);
	oseaclient_nul_data->state = oseaclient_data->state;
	oseaclient_nul_data->text_response = g_strdup (oseaclient_data->text_response);
	oseaclient_data_free (oseaclient_data, FALSE);
	
	// If everything was OK, we must process new af_key and set it to corresponding server
	
	if (oseaclient_nul_data->state == OSEACLIENT_OK) {
		new_af_key = g_strdup (oseacomm_dataset_get (af_key_dataset, 0, 0));
		key =  ___oseaclient_afkeys_parse (new_af_key);
		
		server = oseaclient_list_lookup (session.server_info, key->satellite_server);

		tmp = server->af_key;
		server->af_key = new_af_key;
		g_free (tmp);
		
 		oseacomm_dataset_free (af_key_dataset); 

	} else {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Key refresh failed: %s", oseaclient_nul_data->text_response);
	}


	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_NUL_DATA, data, custom_data, oseaclient_nul_data);

	return;
}







/**
 * oseaclient_session_refresh_key:
 * @server: server whose af-key we must refresh
 * @usr_function: function to be executed when the server response arrives.
 * @usr_data: user data passed to user_function. 
 * 
 * Refresh the af-key for the given @server. When server's response
 * arrives @usr_function will be called with the given @usr_data.
 * 
 * Return value: TRUE if the query was executed, or FALSE if it wasn't
 **/
gboolean        oseaclient_session_refresh_key          (gchar * server,
						    OseaClientNulFunc usr_function,
						    gpointer usr_data)
{
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);

	if (! connection)
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't find connection for os-kernel");

	return oseaclient_request (connection, __oseaclient_session_refresh_key_process, 
			      (OseaClientFunc) usr_function, usr_data,
			      "refresh_key", 
			      "session_id", OSEACOMM_XML_ARG_STRING, session.session_id,
			      "server_name", OSEACOMM_XML_ARG_STRING, server,
			      NULL);
}



static  void __oseaclient_session_refresh_session_process (RRChannel * channel, 
						      RRFrame   * frame,
						      GString   * message, 
						      gpointer    data, 
						      gpointer    custom_data)
{
	OseaClientMultiData  * oseaclient_multidata = NULL;
	OseaClientNulData    * oseaclient_nul_data = NULL;
	GList           * list = NULL;
	OseaCommDataSet   * session_id_dataset = NULL;
	OseaCommDataSet   * server_info_dataset = NULL;
	
	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_multidata = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_MULTI_DATA, channel,
								       frame, message, NULL, &list, &data, &custom_data);


	if (! oseaclient_multidata)
		return;

	oseaclient_nul_data = g_new0 (OseaClientNulData, 1);
	oseaclient_nul_data->state = oseaclient_multidata->state;
	oseaclient_nul_data->text_response = g_strdup (oseaclient_multidata->text_response);
	oseaclient_multi_free (oseaclient_multidata, FALSE);

	// If everything was OK, we must process the list of datasets
	// for extracting session_id and server_info

	if (oseaclient_nul_data->state == OSEACLIENT_OK) {

		session_id_dataset = (OseaCommDataSet *) g_list_nth_data (list, 0);
		server_info_dataset = (OseaCommDataSet *) g_list_nth_data (list, 1);

		g_free (session.session_id);
		session.session_id  = __oseaclient_session_extract_session_id (session_id_dataset);

		#warning We should destroy old session.server_info
		/*oseaclient_list_destroy (session.server_info); */
		session.server_info = __oseaclient_session_extract_server_info (server_info_dataset, channel->connection);
		
		oseacomm_dataset_free (session_id_dataset);
		oseacomm_dataset_free (server_info_dataset);

		g_list_free (list);
	} else {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Session refresh failed: %s", oseaclient_nul_data->text_response);
	}


	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_NUL_DATA, data, custom_data, oseaclient_nul_data);

	return;

}


#warning DOCUMENT ME
gboolean        oseaclient_session_refresh_session   (gchar * passwd,
						 OseaClientNulFunc usr_function,
						 gpointer usr_data)
{
	RRConnection * connection;

	g_return_val_if_fail (passwd, FALSE);

	connection = oseaclient_session_get_connection ("os-kernel", NULL);

	if (! connection) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't connect to os-kernel.");
		return FALSE;
	}
	
	return oseaclient_request (connection, __oseaclient_session_refresh_session_process, 
			      (OseaClientFunc) usr_function, usr_data,
			      "login", 
			      "user", OSEACOMM_XML_ARG_STRING, session.user, 
			      "password", OSEACOMM_XML_ARG_STRING, passwd, 
			      NULL);
	
}






static  void __oseaclient_session_register_process (RRChannel * channel, 
					       RRFrame   * frame,
					       GString   * message, 
					       gpointer    data, 
					       gpointer    custom_data)
{

	OseaClientMultiData  * oseaclient_multidata = NULL;
	OseaClientNulData    * oseaclient_data = NULL;
	GList           * list = NULL;
	OseaCommDataSet   * session_id_dataset = NULL;
	OseaCommDataSet   * server_info_dataset = NULL;	

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_multidata = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_MULTI_DATA, channel,
								       frame, message, NULL, &list, &data, &custom_data);

	if (! oseaclient_multidata)
		return;

	oseaclient_data = g_new0 (OseaClientNulData, 1);
	oseaclient_data->state = oseaclient_multidata->state;
	oseaclient_data->text_response = g_strdup (oseaclient_multidata->text_response);
	oseaclient_multi_free (oseaclient_multidata, FALSE);

	// If everything was OK, we must process the list of datasets
	// for extracting session_id and server_info

	switch (oseaclient_data->state) {
	case OSEACLIENT_OK:
		session_id_dataset = (OseaCommDataSet *) g_list_nth_data (list, 0);
		server_info_dataset = (OseaCommDataSet *) g_list_nth_data (list, 1);
		
		session.session_id  = __oseaclient_session_extract_session_id (session_id_dataset);
		session.server_info = __oseaclient_session_extract_server_info (server_info_dataset, channel->connection);
		
		oseacomm_dataset_free (session_id_dataset);
		oseacomm_dataset_free (server_info_dataset);

		g_list_free (list);

		break;
	case OSEACLIENT_ERROR:
	case OSEACLIENT_UPDATE_SERVICES_NEEDED:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Register failed: not retrieving session info: %s", 
		       oseaclient_data->text_response);
		break;
	default:
		break;
	}


	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_NUL_DATA, data, custom_data, oseaclient_data);

	return;
}

/**
 * oseaclient_session_register:
 * @name: server name
 * @version_number: server version
 * @host: server's listening host
 * @port: server's listening port
 * @kernel_hostname: hostname where os-kernel lives
 * @kernel_port: port where os-kernel is listening
 * @usr_function: function to be executed when the server response arrives
 * @usr_data: user data passed to user_function
 * 
 * This function tries to register the given server (@name and @version_number) 
 * in the os-kernel defined by @kernel_hostname and @kernel_port.  When server's 
 * response arrives @usr_function will be called the given @usr_data.
 * 
 * Return value: TRUE if register request is sent or FALSE if not.
 **/
gboolean        oseaclient_session_register (gchar * name, 
					gint    version_number,
					gchar * host, 
					gchar * port,
					gchar * kernel_hostname,
					gchar * kernel_port,
					OseaClientNulFunc usr_function, 
					gpointer usr_data)
{
	RRConnection * connection;
	GError       * error;
	gchar        * version_string;
	gboolean       result;	

	version_string = g_strdup_printf ("%d", version_number);

	g_return_val_if_fail (name, FALSE);
	g_return_val_if_fail (version_string, FALSE);
	g_return_val_if_fail (host, FALSE);
	g_return_val_if_fail (port, FALSE);
	g_return_val_if_fail (kernel_hostname, FALSE);
	g_return_val_if_fail (kernel_port, FALSE);

	if (server_myself)
		oseaclient_session_server_free (server_myself);

	server_myself = g_new0 (OseaClientSessionServer, 1);
	server_myself->name = name;
	server_myself->host = host;
	server_myself->port = port;

	// Initialize oseacomm 
	if ((! oseacomm_initialized) && (!oseacomm_init ( NULL, NULL, &error))) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Error: unable to initialize oseacomm: %s\n", error->message);
		return FALSE;
	}

	connection = oseacomm_connection_new (kernel_hostname, kernel_port, TYPE_OSEACOMM_SIMPLE);

	if (! connection) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't connect to os-kernel in %s:%s", 
		       kernel_hostname, kernel_port);
		return FALSE;
	}

	result = oseaclient_request (connection, __oseaclient_session_register_process,
				(OseaClientFunc) usr_function, usr_data,
				"register", 
				"name", OSEACOMM_XML_ARG_STRING, name, 
				"version", OSEACOMM_XML_ARG_STRING, version_string,
				"host", OSEACOMM_XML_ARG_STRING, host, 
				"port", OSEACOMM_XML_ARG_STRING, port, 
				NULL);

	g_free (version_string);
	return result;

}



void __oseaclient_session_unregister_process (RRChannel * channel, 
					 RRFrame * frame,
					 GString * message, 
					 gpointer data, 
					 gpointer custom_data)
{
	OseaClientNulData     * oseaclient_data = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_data = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_NUL_DATA,
								  channel,
								  frame,
								  message,
								  NULL, 
								  NULL, &data, &custom_data);

	if (! oseaclient_data)
		return;

	// Now we program a event for destroying all session info, and activate it
	
	oseaclient_event_source_launch ( __oseaclient_session_destroy_session_info, NULL);


	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_NUL_DATA, data, custom_data, oseaclient_data);


	return;
}


/**
 * oseaclient_session_unregister:
 * @usr_function: function to be executed when the server response arrives.
 * @usr_data: user data passed to user_function.
 * 
 * Function used to get unregistered from the system
 * 
 * Return value: TRUE if the query was executed, or FALSE if it wasn't
 **/
gboolean        oseaclient_session_unregister  (OseaClientNulFunc usr_function,
					   gpointer usr_data)
{
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-kernel", NULL);

	if (! connection)
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't find connection for os-kernel");

	return oseaclient_request (connection, __oseaclient_session_unregister_process, 
			      (OseaClientFunc) usr_function, usr_data,
			      "unregister", 
			      "name", OSEACOMM_XML_ARG_STRING, server_myself->name,
			      "host", OSEACOMM_XML_ARG_STRING, server_myself->host,
			      "port", OSEACOMM_XML_ARG_STRING, server_myself->port,
			      NULL);
	
}





/**
 * oseaclient_session_server_exists:
 * @server_name: name of the server
 * 
 * This function returns if a server is registered against the kernel
 * we have connected to.
 * 
 * Return value: TRUE if the server exists, FALSE otherwise
 **/
gboolean        oseaclient_session_server_exists (gchar *server_name)
{
	g_return_val_if_fail (session.server_info, FALSE);
	
	if (oseaclient_list_lookup (session.server_info, server_name))
		return TRUE;
	else
		return FALSE;
}


gboolean __oseaclient_session_extract_active_servers (gpointer key, gpointer value, gpointer data)
{
	OseaClientSessionPrivateServer       * server = (OseaClientSessionPrivateServer *) value;
	OseaClientSessionServer              * pserv  = g_new0 (OseaClientSessionServer, 1);

	

	if (server->name) {
		pserv->name = g_strdup (server->name);
		pserv->host = g_strdup (server->host);
		pserv->port = g_strdup (server->port);
	
		active_servers = g_list_append (active_servers, pserv);
	}
	return FALSE;
}


/**
 * oseaclient_session_active_servers:
 * @: 
 * 
 * Returns the list of the active servers in the kernel when we logged in
 * 
 * Return value: a GList of OseaClientSessionServer
 **/

GList         * oseaclient_session_active_servers ()
{

	g_return_val_if_fail (session.server_info, NULL);

	active_servers = NULL;
	
	oseaclient_list_foreach (session.server_info, __oseaclient_session_extract_active_servers, NULL);

	return active_servers;
}


/**
 * oseaclient_session_server_free:
 * @pserv: 
 * 
 * Frees the memory used by @pserv.
 **/
void            oseaclient_session_server_free (OseaClientSessionServer *pserv)
{
	g_return_if_fail (pserv);

	if (pserv->name)
		g_free (pserv->name);
	if (pserv->host)
		g_free (pserv->host);
	if (pserv->port)
		g_free (pserv->port);
	
	g_free (pserv);
}


/**
 * __oseaclient_session_send_afkey:
 * @server_name: 
 * 
 * 
 * 
 * Return value: 
 **/
gboolean __oseaclient_session_send_afkey (gchar *server_name)
{
	OseaClientSessionPrivateServer  * server = NULL;

	g_return_val_if_fail (session.server_info, FALSE);

	server = oseaclient_list_lookup (session.server_info, server_name);
	g_return_val_if_fail (server, FALSE);
	
	g_return_val_if_fail (server->connection, FALSE);

	return oseaclient_request (server->connection,
			      NULL, NULL, NULL,
			      "set_connection_key",
			      "af-key", OSEACOMM_XML_ARG_STRING, server->af_key,
			      NULL);

}


/**
 * oseaclient_session_send_afkey:
 * @server_name: 
 * @usr_function: 
 * @usr_data: 
 * 
 * This function asks to server @server_name to refresh its af_key
 * 
 * Return value: 
 **/
gboolean        oseaclient_session_send_afkey          (gchar * server_name,
						   OseaClientNulFunc usr_function,
						   gpointer usr_data)
{
	OseaClientSessionPrivateServer  * server = NULL;

	server = oseaclient_list_lookup (session.server_info, server_name);
	g_return_val_if_fail (server, FALSE);	

	g_return_val_if_fail (server->connection, FALSE);

	return oseaclient_request (server->connection, oseaclient_request_process_nul_data, 
			      (OseaClientFunc) usr_function, usr_data,
			      "set_connection_key", 
			      "af-key", OSEACOMM_XML_ARG_STRING, server->af_key,
			      NULL);
}


/**
 * oseaclient_session_get_connection:
 * @server_name: 
 * @error: 
 * 
 * This function should be used only from liboseaclient* libraries for getting connections to the needed af-servers
 * 
 * Return value: The connection to the server, or NULL if the server 
 **/
#warning FIXME: we must add support for forcing a new connection, and support for deleting an existing connection

RRConnection  * oseaclient_session_get_connection (gchar *server_name, GError **error)
{
	OseaClientSessionPrivateServer  * server = NULL;

	g_return_val_if_fail (session.server_info, NULL);

	server = oseaclient_list_lookup (session.server_info, server_name);
	g_return_val_if_fail (server, NULL);
	
	if (! server->connection) {
		server->connection = oseacomm_connection_new (server->host, 
							    server->port, 
							    TYPE_OSEACOMM_SIMPLE);

		if (! __oseaclient_session_send_afkey (server_name))
			return FALSE;
		
	}

	return server->connection;
}

void     oseaclient_session_delete_connection (gchar *server_name)
{
	OseaClientSessionPrivateServer  * server = NULL;

	g_return_if_fail (session.server_info);

	server = oseaclient_list_lookup (session.server_info, server_name);
	g_return_if_fail (server);
	
	if (server->connection) {
		rr_connection_disconnect (server->connection, NULL);
		server->connection = NULL;
	} 

	return;
}

RRConnection  * oseaclient_session_new_connection (gchar *server_name, GError **error)
{
	oseaclient_session_delete_connection (server_name);

	return oseaclient_session_get_connection (server_name, error);
}



gchar  * oseaclient_session_get_server_name (RRConnection *connection, GError **error)
{
	OseaClientSessionPrivateServer  * server = NULL;

	g_return_val_if_fail (session.server_info, NULL);

	oseaclient_list_first (session.server_info);

	do {
		server = (OseaClientSessionPrivateServer *) oseaclient_list_data (session.server_info);

		if (connection == server->connection)
			return g_strdup (server->name);
			
	} while (oseaclient_list_next(session.server_info));


	return NULL;
}


/**
 * oseaclient_session_get_permission_list:
 * @server_name: 
 * 
 * 
 * 
 * Return value: A list with the permissions for the given @server_name that the current user has got.
 **/
GList         * oseaclient_session_get_permission_list (gchar * server_name)
{
	OseaClientSessionPrivateServer * server = NULL;
	OseaClientAfKey                * af_key;
	GList                     * result;

	// Check for correct incomming parameter
	g_return_val_if_fail (server_name, NULL);

	// Search for asked server
	server = oseaclient_list_lookup (session.server_info, server_name);
	
	g_return_val_if_fail (server, NULL);

	// Parse recieved af_key
	af_key = ___oseaclient_afkeys_parse (server->af_key);

	result = af_key->permission_list;

	___oseaclient_afkeys_destroy (af_key, FALSE);
	
	return result;
}


/**
 * oseaclient_session_get_permission:
 * @server_name: 
 * @permission_name: 
 * 
 * Returns if the given permission in the given server is enabled for
 * the current session.
 * 
 * Return value: TRUE if the permission is enabled, FALSE otherwise.
 **/
gboolean        oseaclient_session_get_permission       (gchar * server_name, gchar * permission_name)
{
	OseaClientSessionPrivateServer * server = NULL;
	OseaClientAfKey                * af_key;
	GList                     * permission_list;
	gboolean                    result = FALSE;

	// Check for correct incomming parameter
	g_return_val_if_fail (server_name, FALSE);
	g_return_val_if_fail (permission_name, FALSE);

 	// Search for asked server 
 	server = oseaclient_list_lookup (session.server_info, server_name); 
	
 	g_return_val_if_fail (server, FALSE); 

 	// Parse recieved af_key 
 	af_key = ___oseaclient_afkeys_parse (server->af_key); 

	permission_list = g_list_first (af_key->permission_list);

	while ((permission_list) && (! result)){

		if (! g_strcasecmp (permission_list->data, permission_name))
			result = TRUE;
		
		permission_list = g_list_next(permission_list);
	}

 	___oseaclient_afkeys_destroy (af_key, TRUE); 
	
	return result;

}
