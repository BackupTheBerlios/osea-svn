//
//  LibAfdal: common functions to liboseaclient* level and architectural functions.
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

#include "oseaclient_request.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define LOG_DOMAIN  "AFDAL_REQUEST"


typedef struct {
	gpointer                       user_data;
	CoyoteXmlMessage             * concrect_message;
	AfDalFunc                      user_function;
	CoyoteSimpleCfgCompleteMessage return_function;
	gchar                        * server;
} AfdalRequestUserData;

RRChannel    * oseaclient_request_fake_channel = NULL;

gboolean       oseaclient_request_session_expired_callback_is_active = FALSE;
GList        * oseaclient_request_waiting_for_session_queue = NULL;
GStaticMutex   oseaclient_request_session_expired_callback_is_active_mutex = G_STATIC_MUTEX_INIT;

gboolean oseaclient_request_start_afkey_refresh (gpointer afdal_request_user_data);
gboolean __oseaclient_request_start_afkey_refresh (AfDalNulData *data, gpointer afdal_request_user_data);
gboolean __oseaclient_request_start_afkey_refresh_2 (gpointer afdal_request_user_data);
gboolean __oseaclient_request_start_afkey_refresh_2_process (AfDalNulData *data, gpointer afdal_request_user_data);
gboolean __oseaclient_request_start_afkey_refresh_3 (gpointer afdal_request_user_data);
void     oseaclient_refresh_start_afkey_refresh_return_error (gchar *message, AfdalRequestUserData * request_user_data);
gpointer oseaclient_request_return_from_failed_afkey_refresh (AfDalRequestReturnData type,
							 GString * message, 
							 CoyoteDataSet ** returned_dataset,
							 GList         ** returned_datasets,
							 gpointer * data,
							 gpointer * custom_data);

gboolean oseaclient_request_start_session_refresh (gpointer afdal_request_user_data);
void __oseaclient_request_start_session_refresh (gchar * password, gpointer afdal_request_user_data);
gboolean __oseaclient_request_start_session_refresh_process (AfDalNulData *data, gpointer afdal_request_user_data);
void     oseaclient_request_cancel_session_refresh_process (AfdalRequestUserData * request_user_data);


/**
 * oseaclient_request:
 * @connection: an already created conection to a af-kernel server
 * @return_function: Function to be executed by coyote when an xml message arrives from the other peer.
 * @usr_function: User space function to be executed by liboseaclient when an xml message has been processed.
 * @usr_data: User space data to be passed to @usr_function.
 * @service_name: the service's name we are going to send to.
 * @Varags: A list of elements terminated by NULL, where each couple are the param's names followed by the value. 
 * An example is: "name", name, "description", description, NULL
 * 
 * Convenience function to create a request to a server select by
 * @connection. This function is meant to be used by liboseaclient* library
 * level when they are going to make a request.
 * 
 * Return value: Returns TRUE if request goes ok or FALSE if fails.
 **/
gboolean   oseaclient_request                     (RRConnection * connection,
					      CoyoteSimpleCfgCompleteMessage return_function,
					      AfDalFunc usr_function, 
					      gpointer usr_data,
					      gchar * service_name,
					      ...)
{
	va_list args;
	GError * error = NULL;
	
	CoyoteSimple    * channel = NULL;
	CoyoteSimpleCfg * config = NULL;
	RRMessage       * rr_message = NULL;
	
	CoyoteXmlObject  * abstract_message = NULL;
	CoyoteXmlMessage * concrect_message = NULL;
	
	AfdalRequestUserData * request_user_data = NULL;

	gchar                * server_name = NULL;
	gint                   try_number = 0;

	va_start (args, service_name);
	
	request_user_data = g_new0 (AfdalRequestUserData, 1);
	request_user_data->user_data = usr_data;
	request_user_data->return_function = return_function;
	request_user_data->user_function = usr_function;

	if (! oseaclient_request_fake_channel)
		oseaclient_request_fake_channel = g_new0 (RRChannel, 1);

	// Create a config object to tell to our profile to execute
	// the following function when a complete response arrives
	config = coyote_simple_cfg_new ();
	coyote_simple_cfg_set_complete_message (config, 
						return_function,
						(gpointer) usr_function, 
						request_user_data);
	
	// Create a channel using the passed connection
	channel = coyote_simple_start (connection, config, &error);
	while ((!channel) && (try_number < 2)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't start a channel");

		server_name = oseaclient_session_get_server_name (connection, &error);
		connection = oseaclient_session_new_connection (server_name, &error);

		channel = coyote_simple_start (connection, config, &error);
		try_number ++;
	}

	if (!channel) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't start a channel");
		return FALSE;
	}
	
	// Create the message using coyote_xml
	abstract_message = coyote_xml_new_object ();	
	if (!args) 
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "args param is null");
	
	coyote_xml_add_vrequest_service (abstract_message, service_name, args);
	concrect_message = coyote_xml_build_message (abstract_message);
	request_user_data->concrect_message = coyote_xml_build_message (abstract_message);
	coyote_xml_destroy_object (abstract_message);

	
	// Create a road runner message 
	rr_message = rr_message_static_new (RR_FRAME_TYPE_MSG,
					    concrect_message->content,
					    concrect_message->len,
					    TRUE);
		
	// Send the message
	if (!rr_channel_send_message (RR_CHANNEL (channel), rr_message, &error)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Error while sending message: %s", error->message);
		return FALSE;
	}
	
	// Destroy unused objects.
	coyote_xml_destroy_message (concrect_message);

	va_end (args);
	
	return TRUE;		
}


/**
 * oseaclient_request_close_and_return_initial_data:
 * @type: Type of oseaclient user space data: AFDAL_REQUEST_DATA,
 * AFDAL_REQUEST_SIMPLE_DATA, AFDAL_REQUEST_NUL_DATA 
 * @channel: channel where @message was recieved.
 * @frame: recieved frame
 * @message: 
 * @returned_dataset: recieved dataset from coyote_xml level. This out
 * variable is set to the recieved dataset when @type is set to
 * AFDAL_REQUEST_DATA. 
 * 
 * Convenience function meant to be used to write process request
 * functions more quickly and maintainable.
 * This function trusts in incoming message to be process as @type
 * variable says, so is @type is set to AFDAL_REQUEST_DATA, function
 * will expect to find a coyote_dataset from coyote level and will
 * return a AfdalData pointer to the caller.
 * If this sound to you very dificult, please see
 * oseaclient_request_process_data/simple_data/nul_data to get an clear
 * idea on what I am saying to you.
 * 
 * Return value: Return an AfDalData, AfDalSimpleData, AfDalNulData
 * according to @type value.
 **/
gpointer   oseaclient_request_close_and_return_initial_data (AfDalRequestReturnData type,
							RRChannel * channel,
							RRFrame * frame,
							GString * message, 
							CoyoteDataSet ** returned_dataset,
							GList         ** returned_datasets,
							gpointer * data,
							gpointer * custom_data)
{
	GError * error = NULL;
	AfDalData        * user_data = NULL;
	AfDalSimpleData  * user_simple_data = NULL;
	AfDalNulData     * user_nul_data = NULL;
	AfDalMultiData   * user_multi_data = NULL;
	CoyoteXmlMessage     * coyote_message = NULL;
	CoyoteXmlServiceData * xml_data = NULL;
	RRConnection         * connection = NULL;

	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) (*custom_data);	
	
	if (channel == oseaclient_request_fake_channel)
		return oseaclient_request_return_from_failed_afkey_refresh (type, message, returned_dataset,
								       returned_datasets, data, custom_data);

	connection = rr_channel_get_connection (channel);
	(*custom_data) = request_user_data->user_data;

	// Selects apropiate user level data to return
	switch (type) {
	case AFDAL_REQUEST_MULTI_DATA:
		user_multi_data = g_new0 (AfDalMultiData, 1);
		break;
	case AFDAL_REQUEST_DATA:
		user_data = g_new0 (AfDalData, 1);
		break;
	case AFDAL_REQUEST_SIMPLE_DATA:
		user_simple_data = g_new0 (AfDalSimpleData, 1);
		break;
	case AFDAL_REQUEST_NUL_DATA:
		user_nul_data = g_new0 (AfDalNulData, 1);
		break;
	}
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "closing channel");


	// Close incoming message's channel
	if (!coyote_simple_close (COYOTE_SIMPLE (channel), &error)) 
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Error while closing the channel: %s", error->message);
	else
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel closed");

	// validate message	
	coyote_message = g_new0 (CoyoteXmlMessage, 1);
	coyote_message->content = message->str;
	coyote_message->len = message->len;		

	if (!coyote_xml_validate_message (coyote_message)) {

		coyote_xml_destroy_message (request_user_data->concrect_message);
		g_free (request_user_data);
				
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Validation failed, we are goint to die: %s", 
		       coyote_message->content);
		g_free (coyote_message);

		switch (type) {
		case AFDAL_REQUEST_MULTI_DATA:
			user_multi_data->state = AFDAL_ERROR;
			user_multi_data->text_response = g_strdup ("Validation failed: we are going to die");			
			return user_multi_data;
		case AFDAL_REQUEST_DATA:
			user_data->state = AFDAL_ERROR;
			user_data->text_response = g_strdup ("Validation failed: we are going to die");
			return user_data;
		case AFDAL_REQUEST_SIMPLE_DATA:
			user_simple_data->state = AFDAL_ERROR;
			user_simple_data->text_response = g_strdup ("Validation failed: we are going to die");
			return user_simple_data;
		case AFDAL_REQUEST_NUL_DATA:
			user_nul_data->state = AFDAL_ERROR;
			user_nul_data->text_response = g_strdup ("Validation failed: we are going to die");
			return user_nul_data;
		}
		return NULL;
	}
	
	// Parse incoming message to get an apropiate structure
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Parsing XML message");
	xml_data = coyote_xml_parse_message (coyote_message);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "XML message parsed");
	g_free (coyote_message);
			
	if (!xml_data) {

		coyote_xml_destroy_message (request_user_data->concrect_message);
		g_free (request_user_data);
				
		switch (type) {
		case AFDAL_REQUEST_MULTI_DATA:
			user_multi_data->state = AFDAL_ERROR;
			user_multi_data->text_response = g_strdup ("Failed to parse message: we are going to die");
			return user_multi_data;

		case AFDAL_REQUEST_DATA:
			user_data->state = AFDAL_ERROR;
			user_data->text_response = g_strdup ("Failed to parse message: we are going to die");
			return user_data;

		case AFDAL_REQUEST_SIMPLE_DATA:
			user_simple_data->state = AFDAL_ERROR;
			user_simple_data->text_response = g_strdup ("Failed to parse message: we are going to die");
			return user_simple_data;

		case AFDAL_REQUEST_NUL_DATA:
			user_nul_data->state = AFDAL_ERROR;
			user_nul_data->text_response = g_strdup ("Failed to parse message: we are going to die");
			return user_nul_data;
		}
		return NULL;
	}
	
	if (frame->type ==  RR_FRAME_TYPE_RPY) {

		coyote_xml_destroy_message (request_user_data->concrect_message);
		g_free (request_user_data);
				
		switch (type) {
		case AFDAL_REQUEST_MULTI_DATA:
			if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_SERVICES_UPDATE_NEEDED)
				user_multi_data->state = AFDAL_UPDATE_SERVICES_NEEDED;
			else
				user_multi_data->state = AFDAL_OK;
			user_multi_data->text_response = g_strdup (xml_data->status_message);
			if (returned_datasets) 
				(*returned_datasets) = xml_data->item_list;
			return user_multi_data;
		case AFDAL_REQUEST_DATA:
			if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_SERVICES_UPDATE_NEEDED)
				user_data->state = AFDAL_UPDATE_SERVICES_NEEDED;
			else
				user_data->state = AFDAL_OK;
			user_data->text_response = g_strdup (xml_data->status_message);
			if (returned_dataset)
				(*returned_dataset) = xml_data->item_list->data;
			return user_data;
			
		case AFDAL_REQUEST_SIMPLE_DATA:
			if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_SERVICES_UPDATE_NEEDED)
				user_simple_data->state = AFDAL_UPDATE_SERVICES_NEEDED;
			else
				user_simple_data->state = AFDAL_OK;
			user_simple_data->id = atoi(coyote_dataset_get((xml_data->item_list->data),0,0));
			return user_simple_data;

		case AFDAL_REQUEST_NUL_DATA:
			if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_SERVICES_UPDATE_NEEDED)
				user_nul_data->state = AFDAL_UPDATE_SERVICES_NEEDED;
			else
				user_nul_data->state = AFDAL_OK;
			return user_nul_data;
		}
		return NULL;

		
	}else {
		
		// frame_type == RR_FRAME_TYPE_ERR

		if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_KEY_EXPIRED) {			
			request_user_data->server = oseaclient_session_get_server_name (connection, NULL);
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Refreshing af key...");
			oseaclient_event_source_launch (afdal_request_start_afkey_refresh, request_user_data);
			return NULL;
		}

		coyote_xml_destroy_message (request_user_data->concrect_message);
		g_free (request_user_data->server);
		g_free (request_user_data);
				
		switch (type) {

		case AFDAL_REQUEST_MULTI_DATA:
			if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_SESSION_EXPIRED)
				user_multi_data->state = AFDAL_SESSION_EXPIRED;
			else
				user_multi_data->state = AFDAL_ERROR;
			user_multi_data->text_response = g_strdup (xml_data->status_message);
			return user_multi_data;
		case AFDAL_REQUEST_DATA:
			if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_SESSION_EXPIRED)
				user_data->state = AFDAL_SESSION_EXPIRED;
			else
				user_data->state = AFDAL_ERROR;
			user_data->text_response = g_strdup (xml_data->status_message);
			return user_data;
		case AFDAL_REQUEST_SIMPLE_DATA:
			if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_SESSION_EXPIRED)
				user_simple_data->state = AFDAL_SESSION_EXPIRED;
			else
				user_simple_data->state = AFDAL_ERROR;
			user_simple_data->text_response = g_strdup (xml_data->status_message);
			return user_simple_data;
		case AFDAL_REQUEST_NUL_DATA:
			if (coyote_code_get_type (xml_data->status) == COYOTE_CODE_SESSION_EXPIRED)
				user_nul_data->state = AFDAL_SESSION_EXPIRED;
			else
				user_nul_data->state = AFDAL_ERROR;
			user_nul_data->text_response = g_strdup (xml_data->status_message);
			return user_nul_data;
		}
		return NULL;
	}
	
	return NULL;
}



gboolean oseaclient_request_start_afkey_refresh (gpointer afdal_request_user_data)
{
	/* Ask for a af-key refresh, calling step 2 for processing it */
	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) oseaclient_request_user_data;	

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Asking af-kernel for new af-key...");
	      
	if (! oseaclient_session_refresh_key (request_user_data->server,
				   __oseaclient_request_start_afkey_refresh,
				   oseaclient_request_user_data))
		oseaclient_refresh_start_afkey_refresh_return_error ("Couldn't refresh key: not found valid connection to af-kernel",
								request_user_data);

	return FALSE;
}

gboolean __oseaclient_request_start_afkey_refresh (AfDalNulData *data, gpointer afdal_request_user_data)
{
	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) oseaclient_request_user_data;	

	if (data->state == AFDAL_ERROR) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't refresh af-key");
		g_free (request_user_data->server);
		g_free (request_user_data);
		oseaclient_refresh_start_afkey_refresh_return_error ("Couldn't refresh key.",
								request_user_data);
		return FALSE;
	}

	if (data->state == AFDAL_SESSION_EXPIRED) {
		g_static_mutex_lock (& oseaclient_request_session_expired_callback_is_active_mutex);

		if (oseaclient_request_session_expired_callback_is_active) {
			oseaclient_request_waiting_for_session_queue = g_list_append (afdal_request_waiting_for_session_queue,
										 request_user_data);
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Password dialog already shown. Putting action in queue.");
		} else {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Session expired. Asking password to user...");
			oseaclient_request_session_expired_callback_is_active = TRUE;
			oseaclient_event_source_launch (afdal_request_start_session_refresh, request_user_data);
		}
		g_static_mutex_unlock (& oseaclient_request_session_expired_callback_is_active_mutex);
		oseaclient_nul_free (data);
		return FALSE;
	}
			
	oseaclient_nul_free (data);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Af-Kernel gave us the new key");
	oseaclient_event_source_launch (__afdal_request_start_afkey_refresh_2, request_user_data);
	return FALSE;
}


gboolean __oseaclient_request_start_afkey_refresh_2 (gpointer afdal_request_user_data)
{
	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) oseaclient_request_user_data;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sending new af-key to server...");

	if (! oseaclient_session_send_afkey (request_user_data->server,
					__oseaclient_request_start_afkey_refresh_2_process,
					request_user_data))
		oseaclient_refresh_start_afkey_refresh_return_error ("Couldn't refresh key: not found valid connection to af-kernel",
								request_user_data);		
	return FALSE;
}


gboolean __oseaclient_request_start_afkey_refresh_2_process (AfDalNulData *data, gpointer afdal_request_user_data)
{
	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) oseaclient_request_user_data;	

	if (data->state == AFDAL_ERROR) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Server couldn't get new af-key");
		g_free (request_user_data->server);
		g_free (request_user_data);
		oseaclient_refresh_start_afkey_refresh_return_error ("Couldn't refresh key.",
								request_user_data);
		return FALSE;
	}

	oseaclient_nul_free (data);

	oseaclient_event_source_launch (__afdal_request_start_afkey_refresh_3, request_user_data);

	return FALSE;
}


gboolean __oseaclient_request_start_afkey_refresh_3 (gpointer afdal_request_user_data)
{
	/* Sending original petition, with original callback function */
	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) oseaclient_request_user_data;
	CoyoteSimple    * channel = NULL;
	CoyoteSimpleCfg * config = NULL;
	RRMessage       * rr_message = NULL;
	GError          * error = NULL;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "New key sent to server. Now proceeding with repeating actual petition...");
	
	// Create a config object to tell to our profile to execute
	// the following function when a complete response arrives
	config = coyote_simple_cfg_new ();
	coyote_simple_cfg_set_complete_message (config, 
						request_user_data->return_function,
						(gpointer) request_user_data->user_function, 
						request_user_data);
	
	// Create a channel using the passed connection
	channel = coyote_simple_start (oseaclient_session_get_connection (request_user_data->server, FALSE), 
				       config, &error);
	if (!channel) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't start a channel");
		return FALSE;
	}
	
	// Create a road runner message 
	rr_message = rr_message_static_new (RR_FRAME_TYPE_MSG,
					    g_strdup (request_user_data->concrect_message->content),
					    request_user_data->concrect_message->len,
					    TRUE);
		
	// Send the message
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Repeating message: %s", request_user_data->concrect_message->content);
	if (!rr_channel_send_message (RR_CHANNEL (channel), rr_message, &error)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Error while sending message: %s", error->message);
		return FALSE;
	}
	
	return FALSE;
}

void oseaclient_refresh_start_afkey_refresh_return_error (gchar *message, AfdalRequestUserData * request_user_data)
{
	GString              * string = g_string_new (message);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Returning error from refresh key/session");
	 
	request_user_data->return_function (oseaclient_request_fake_channel,
					    NULL, string, request_user_data->user_function,
					    request_user_data);
					    
	return;
}

gpointer   oseaclient_request_return_from_failed_afkey_refresh (AfDalRequestReturnData type,
							   GString * message, 
							   CoyoteDataSet ** returned_dataset,
							   GList         ** returned_datasets,
							   gpointer * data,
							   gpointer * custom_data)
{
	AfDalData        * user_data = NULL;
	AfDalSimpleData  * user_simple_data = NULL;
	AfDalNulData     * user_nul_data = NULL;
	AfDalMultiData   * user_multi_data = NULL;

	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) (*custom_data);	
	
	(*custom_data) = request_user_data->user_data;

	// Selects apropiate user level data to return
	switch (type) {
	case AFDAL_REQUEST_MULTI_DATA:
		user_multi_data = g_new0 (AfDalMultiData, 1);
		break;
	case AFDAL_REQUEST_DATA:
		user_data = g_new0 (AfDalData, 1);
		break;
	case AFDAL_REQUEST_SIMPLE_DATA:
		user_simple_data = g_new0 (AfDalSimpleData, 1);
		break;
	case AFDAL_REQUEST_NUL_DATA:
		user_nul_data = g_new0 (AfDalNulData, 1);
		break;
	}
	
	coyote_xml_destroy_message (request_user_data->concrect_message);
				
	switch (type) {
	case AFDAL_REQUEST_MULTI_DATA:
		user_multi_data->state = AFDAL_ERROR;
		user_multi_data->text_response = g_strdup (message->str);			
		return user_multi_data;
	case AFDAL_REQUEST_DATA:
		user_data->state = AFDAL_ERROR;
		user_data->text_response = g_strdup (message->str);
		return user_data;
	case AFDAL_REQUEST_SIMPLE_DATA:
		user_simple_data->state = AFDAL_ERROR;
		user_simple_data->text_response = g_strdup (message->str);
		return user_simple_data;
	case AFDAL_REQUEST_NUL_DATA:
		user_nul_data->state = AFDAL_ERROR;
		user_nul_data->text_response = g_strdup (message->str);
		return user_nul_data;
	}

	g_free (request_user_data->server);
	g_free (request_user_data);

	return NULL;
}




gboolean oseaclient_request_start_session_refresh (gpointer afdal_request_user_data)
{
	/* Ask for a af-key refresh, calling step 2 for processing it */
	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) oseaclient_request_user_data;	

	if (! oseaclient_password_get (__afdal_request_start_session_refresh, request_user_data))
		oseaclient_refresh_start_afkey_refresh_return_error ("Couldn't refresh session. Callback not defined.",
								request_user_data);
	      
	return FALSE;
}




void __oseaclient_request_start_session_refresh (gchar * password, gpointer afdal_request_user_data)
{
	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) oseaclient_request_user_data;	

	if (!password) {
		/* User has cancelled session*/
		oseaclient_request_cancel_session_refresh_process (request_user_data);
		return;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "User gave me a password. Trying to get a new session id.");

	if (! oseaclient_session_refresh_session (password,
					     __oseaclient_request_start_session_refresh_process,
					     oseaclient_request_user_data))
		oseaclient_refresh_start_afkey_refresh_return_error ("Couldn't refresh key: not found valid connection to af-kernel",
								request_user_data);


	return;
}

gboolean __oseaclient_request_start_session_refresh_process (AfDalNulData *data, gpointer afdal_request_user_data)
{
	AfdalRequestUserData * request_user_data = (AfdalRequestUserData *) oseaclient_request_user_data;	
	GList                * cursor = g_list_first (oseaclient_request_waiting_for_session_queue);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "OK. Got new session ID.");

	oseaclient_nul_free (data);

	g_static_mutex_lock (& oseaclient_request_session_expired_callback_is_active_mutex);
	oseaclient_request_session_expired_callback_is_active = FALSE;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Relaunching actual petition ");

	oseaclient_event_source_launch (__afdal_request_start_afkey_refresh_3, request_user_data);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Actual petition relaunched");

	while (cursor) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Launching deferred operation.");
		oseaclient_event_source_launch (__afdal_request_start_afkey_refresh_3, cursor->data);
		cursor = cursor->next;
	}

	g_list_free (oseaclient_request_waiting_for_session_queue);
	oseaclient_request_waiting_for_session_queue = NULL;
	
	g_static_mutex_unlock (& oseaclient_request_session_expired_callback_is_active_mutex);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Exiting __oseaclient_request_start_session_refresh_process");

	return FALSE;
}

void     oseaclient_request_cancel_session_refresh_process (AfdalRequestUserData * request_user_data)
{
	AfdalRequestUserData * rud = NULL;
	GList                * cursor = NULL;

	g_static_mutex_lock (& oseaclient_request_session_expired_callback_is_active_mutex);
	oseaclient_request_session_expired_callback_is_active = FALSE;

	g_free (request_user_data->server);
	coyote_xml_destroy_message (request_user_data->concrect_message);
	g_free (request_user_data);

	while (cursor) {
		rud = (AfdalRequestUserData *) cursor->data;
		g_free (rud->server);
		coyote_xml_destroy_message (rud->concrect_message);
		g_free (rud);
		cursor = cursor->next;
	}

	g_list_free (oseaclient_request_waiting_for_session_queue);
	oseaclient_request_waiting_for_session_queue = NULL;
	
	g_static_mutex_unlock (& oseaclient_request_session_expired_callback_is_active_mutex);

	return;
}



/**
 * oseaclient_request_call_user_function:
 * @type:  * @type: Type of oseaclient user space data: AFDAL_REQUEST_DATA,
 * AFDAL_REQUEST_SIMPLE_DATA, AFDAL_REQUEST_NUL_DATA 
 * @handler: user space function to be executed
 * @user_data: user space data to be passed to @handler.
 * @oseaclient_data: afdal level data to be passed to @handler.
 * 
 * Convenience function to make life easy to those who are writing
 * liboseaclient* function. It's more dificult to explain what does this
 * function do, so please see
 * @oseaclient_request_process_data/simple_data/nul_data function.
 **/
void oseaclient_request_call_user_function (AfDalRequestReturnData type, gpointer handler,
				       gpointer user_data, gpointer oseaclient_data)
{
	AfDalDataFunc   data_handler;
	AfDalSimpleFunc simple_handler;
	AfDalNulFunc    nul_handler;
	AfDalMultiFunc  multi_handler;
	gboolean        result = FALSE;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Executing user handler");
	switch (type) {
	case AFDAL_REQUEST_DATA:
		data_handler = (AfDalDataFunc) handler;
		result = data_handler (oseaclient_data, user_data);
		break;
	case AFDAL_REQUEST_SIMPLE_DATA:
		simple_handler = (AfDalSimpleFunc) handler;
		result = simple_handler (oseaclient_data, user_data);
		break;
	case AFDAL_REQUEST_NUL_DATA:
		nul_handler = (AfDalNulFunc) handler;
		result = nul_handler (oseaclient_data, user_data);
		break;
	case AFDAL_REQUEST_MULTI_DATA:
		multi_handler = (AfDalMultiFunc) handler;
		result = multi_handler (oseaclient_data, user_data);
		break;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "User handler executed");	
	return;
}

/**
 * oseaclient_request_process_data:
 * @channel: 
 * @frame: 
 * @message: 
 * @data: 
 * @custom_data: 
 * 
 * Generic process function to handle incoming messages with
 * data. This function is meant to be used by liboseaclient* programmers to
 * create a more maintainable library. This functions is an example
 * on how to create process functions for incomming message with data,
 * so you don't have to use it directly.
 **/
 void       oseaclient_request_process_data        (RRChannel * channel, 
						     RRFrame * frame,
						     GString * message, 
						     gpointer data, 
						     gpointer custom_data)
{

	return;
}

/**
 * oseaclient_request_process_simple_data:
 * @channel: 
 * @frame: 
 * @message: 
 * @data: 
 * @custom_data: 
 * 
 * Generic process function to handle incoming messages with an unique
 * data, which is usually an identifier. This function is meant to be
 * used by liboseaclient* programmers to create a more maintainable library.
 **/
 void       oseaclient_request_process_simple_data (RRChannel * channel, 
						     RRFrame * frame,
						     GString * message, 
						     gpointer data, 
						     gpointer custom_data)
{
	AfDalSimpleData  * oseaclient_data = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_SIMPLE_DATA,
								  channel,
								  frame,
								  message,
								  NULL, NULL,
								  &data, &custom_data);
	if (! oseaclient_data)
		return;

	// Call to user defined callback.
	oseaclient_request_call_user_function (AFDAL_REQUEST_SIMPLE_DATA, data, custom_data, afdal_data);

	return;
}



/**
 * oseaclient_request_process_nul_data:
 * @channel: channel where @message was recieved.
 * @frame: recieved frame
 * @message: 
 * @data: 
 * @custom_data: 
 * 
 * Generic process function to handle incoming messages with no 
 * data. This function is meant to be used by liboseaclient* programmers to
 * create a more maintainable library.
 **/
 void       oseaclient_request_process_nul_data    (RRChannel * channel, 
					       RRFrame * frame,
					       GString * message, 
					       gpointer data, 
					       gpointer custom_data)
{
	AfDalNulData         * oseaclient_data = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_data = afdal_request_close_and_return_initial_data (AFDAL_REQUEST_NUL_DATA,
								  channel,
								  frame,
								  message,
								  NULL, NULL,
								  &data, &custom_data);

	if (! oseaclient_data)
		return;

	// Call to user defined callback.
	oseaclient_request_call_user_function (AFDAL_REQUEST_NUL_DATA, data, custom_data, afdal_data);

	return;
}



