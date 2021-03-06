//  ASPL Fact Server Services Update: 
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

#include <unistd.h>
#include "oseaserver_services.h"
#include <coyote/coyote.h>
#include <afdal/afdal.h>
#include "oseaserver_afkeys.h"

#define LOG_DOMAIN "oseaserver_services"


gboolean active_wait;
gboolean kernel_ok_response;

static GList * services_provided = NULL;

AfgsServicesProvided services_provided_by_oseaserver [] = {
	{"set_connection_key",   "", oseaserver_afkeys_set_connection_key, FALSE, NULL, NULL, NULL},
	{NULL, NULL, NULL, FALSE, NULL, NULL}
};

/**
 * oseaserver_services_set:
 * @services: 
 * 
 * This function set up all available services for the actual server
 * 
 **/
void                   oseaserver_services_set (AfgsServicesProvided * services)
{
	gint iterator;
	AfgsServicesProvided * service = NULL;

	g_return_if_fail (services);

	iterator = 0;
	while ((services[iterator].service_name != NULL)) {
		service = g_new0 (AfgsServicesProvided, 1);
		service->service_name = services[iterator].service_name;
		service->description = services[iterator].description;
		service->service_request_process_function = services[iterator].service_request_process_function;
		service->need_key = services[iterator].need_key;
		service->user_data = services[iterator].user_data;
		service->actual_permission = services[iterator].actual_permission;
		service->dependencies = services[iterator].dependencies;

		services_provided = g_list_append (services_provided, service);		
		iterator ++;
	}

	iterator = 0;
	while ((services_provided_by_oseaserver[iterator].service_name != NULL)) {
		service = g_new0 (AfgsServicesProvided, 1);
		service->service_name = services_provided_by_oseaserver[iterator].service_name;
		service->description = services_provided_by_oseaserver[iterator].description;
		service->service_request_process_function = services_provided_by_oseaserver[iterator].service_request_process_function;
		service->need_key = services_provided_by_oseaserver[iterator].need_key;
		service->user_data = services_provided_by_oseaserver[iterator].user_data;
		service->actual_permission = services_provided_by_oseaserver[iterator].actual_permission;
		service->dependencies = services_provided_by_oseaserver[iterator].dependencies;

		services_provided = g_list_append (services_provided, service);		
		iterator ++;
	}

	return;
}

/**
 * oseaserver_services_get:
 * 
 * This function return the a list with the services set by oseaserver_services_set. 
 *
 * Return value: 
 **/
GList * oseaserver_services_get ()
{
	if (!services_provided) 
		g_critical ("Services not defined");
	
	return services_provided;
}



static  void __oseaserver_services_kernel_response_received (RRChannel * channel, 
				    RRFrame   * frame,
				    GString   * message, 
				    gpointer    data, 
				    gpointer    custom_data)
{
	AfDalNulData  * afdal_data = NULL;
	CoyoteDataSet * dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);

	// Close the channel properly
	afdal_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_NUL_DATA, channel,
								  frame, message, &dataset, NULL,
								  &data, &custom_data);

	if (! afdal_data)
		return;

	if (afdal_data->state == AFDAL_OK)
		kernel_ok_response = TRUE;
	else {
		kernel_ok_response = FALSE;
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,  "Af-kernel response: %s\n", afdal_data->text_response);
	}
	
	active_wait = FALSE;
	
}


/**
 * oseaserver_services_get_dataset:
 * 
 * This convenience function return a dataset which contains all
 * services and its dependencies.
 * 
 * Return value: 
 **/
CoyoteDataSet        * oseaserver_services_get_dataset ()
{

	CoyoteDataSet  * dataset        = NULL;
	GList          * iterator       = NULL;

	g_return_val_if_fail (services_provided, NULL);

	dataset = coyote_dataset_new ();

	iterator = g_list_first (services_provided);

	while (iterator) {

		if ((((AfgsServicesProvided *)(iterator->data))->need_key)
		    && (! ((AfgsServicesProvided *) (iterator->data))->actual_permission))
			// Create permission dataset row
			coyote_dataset_add_nth (dataset, 
						((AfgsServicesProvided *) (iterator->data))->service_name,
						((AfgsServicesProvided *) (iterator->data))->description,
						((AfgsServicesProvided *) (iterator->data))->dependencies,
						NULL);
		
		iterator = iterator->next;
		
		// Add a new row if next service is not NULL.
		if (iterator && 
		    (((AfgsServicesProvided *)(iterator->data))->need_key) &&
		    (! ((AfgsServicesProvided *)(iterator->data))->actual_permission))
			coyote_dataset_new_row (dataset);
		
	}

	g_print ("Returned dataset\n");

	coyote_dataset_print (dataset);
	
	return dataset;

}

/**
 * oseaserver_services_update:
 * @server_name: 
 * @version_number: 
 * 
 * This function allow to a server to install or update all its available servers.
 * 
 * Return value: 
 **/
		
gboolean oseaserver_services_update (const gchar *server_name, const gchar *server_description, gint version_number)
{
	RRConnection * connection;
	GError       * error;
	gchar        * kernel_hostname = oseaserver_config_get (NULL, "kernel server");
	gchar        * kernel_port = oseaserver_config_get (NULL, "kernel port");

	// Initialize coyote 
	if (!coyote_init ( NULL, NULL, &error)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Error: unable to initialize coyote: %s\n", error->message);
		return FALSE;
	}
	// Establish connection
	connection = coyote_connection_new (kernel_hostname, kernel_port, TYPE_COYOTE_SIMPLE);
	g_free (kernel_hostname);
	g_free (kernel_port);
	
	if (!connection) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't connect to af-kernel in %s:%s", 
		       kernel_hostname, kernel_port);
		return FALSE;
	}

	// Build message
	active_wait = TRUE;
	
	if (!afdal_request (connection, __oseaserver_services_kernel_response_received,
			    NULL, NULL,
			    "update_services", 
			    "server_name", COYOTE_XML_ARG_STRING, server_name, 
			    "description", COYOTE_XML_ARG_STRING, server_description, 
			    "version", COYOTE_XML_ARG_STRING, g_strdup_printf("%d",version_number), 
			    "permissions", COYOTE_XML_ARG_DATASET, oseaserver_services_get_dataset (), 
			    NULL))
		return FALSE;
	
		
	while (active_wait) {
		// active wait
		sleep (1);
	}
	
	return kernel_ok_response;

}


gboolean              oseaserver_services_process (CoyoteXmlServiceData * data, RRChannel * channel, gint msg_no)
{
	GList       * iterator;
	gchar       * host_name;
	AfgsAfKey   * af_key;

	// obtain service type
	switch (data->type) {
	case COYOTE_XML_SERVICE_REQUEST:
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "It's a request, name: %s\n", data->name);
		
		for (iterator = g_list_first (services_provided); iterator; iterator = iterator->next) {
			if (!g_strcasecmp (data->name, ((AfgsServicesProvided *) (iterator->data))->service_name)) {

				// Check for af_key
				if (((AfgsServicesProvided *) (iterator->data))->need_key) {
					if (!((AfgsServicesProvided *) (iterator->data))->actual_permission) {
						if (!oseaserver_afkeys_check_service_permission (channel, 
											   ((AfgsServicesProvided *) (iterator->data))->service_name, 
											   msg_no))
							return FALSE;
					} else {
						if (!oseaserver_afkeys_check_service_permission 
						    (channel, 
						     ((AfgsServicesProvided *) (iterator->data))->actual_permission, 
						     msg_no))
							return FALSE;
					}
				}
				// Log client ip, user and service name into the log.
				host_name = coyote_connection_get_client_name (channel);
				af_key = oseaserver_afkeys_get_connection_key (rr_channel_get_connection (channel));
				if (af_key) {

					oseaserver_log_write ("recieved petition '%s' from: %s user: %s", 
							data->name, host_name, af_key->user);
				}else {
					// its a public service, no way to figure out who is connecting
					oseaserver_log_write ("recieved petition '%s' from: %s user: no user, public service",
							data->name, host_name);
				}

				free (host_name);

				return ((AfgsServicesProvided *) (iterator->data))->service_request_process_function 
					(data, 		 
					 ((AfgsServicesProvided *) (iterator->data))->user_data, 
					 channel, msg_no);
			}
		}

		
		
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Not recognized service request %s. Generating error request...", data->name);

		oseaserver_message_error_answer (channel, msg_no, "Critical error: the service is not provided by this server",
					   COYOTE_CODE_UNKNOWN_SERVICE);
		return TRUE;

	case COYOTE_XML_SERVICE_RESPONSE:
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "It's a response\n");		
		return TRUE;

	default:
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Message type not recognized\n");
		return FALSE;
	}
}
