//  LibCoyote:  Support library with xml and communication functions.
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
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

#include <librr/rr.h>
#include "oseacomm_simple.h"

#define LOG_DOMAIN "COYOTE_SIMPLE"

static GObjectClass * parent_class = NULL;


typedef struct __CoyoteSimpleData {
	gboolean finish_connection;
	GString * buffer;
} CoyoteSimpleData;


gboolean oseacomm_simple_close_indication  (RRChannel *channel, gint code,
					  const gchar *xml_lang, 
					  const gchar *diagnostic,
					  GError **error);

void oseacomm_simple_close_confirmation  (RRChannel *channel, gint code,
					const gchar *xml_lang, 
					const gchar *diagnostic);

static gboolean oseacomm_simple_frame_available (RRChannel *channel, RRFrame *frame, 
					       GError **error);

gboolean oseacomm_simple_server_init       (RRChannel *channel, 
					  const gchar *piggyback, 
					  GError **error);

static void finalize            (GObject *object);

/**
 * oseacomm_simple_init:
 * @object: a newly created profile object.
 * 
 * This function is used to initialize instance specific variables.
 **/
static void
oseacomm_simple_init (GObject *object, gpointer user_data)
{
	CoyoteSimple * channel  = NULL;
	
	channel = COYOTE_SIMPLE (object);
	
	// Initialize internal oseacomm simple data
	channel->buffer = g_byte_array_new ();
	channel->finish_connection = FALSE;
	return;
}

/**
 * oseacomm_simple_class_init:
 * @klass: Our class stucture.
 * 
 * This function will be called when the profile class is registered
 * as a GType.
 **/
static void
oseacomm_simple_class_init (GObjectClass * class)
{
	RRChannelClass * channel_class = (RRChannelClass *) class;
	
	channel_class->frame_available = oseacomm_simple_frame_available;
 	channel_class->close_indication = oseacomm_simple_close_indication;
	channel_class->close_confirmation = oseacomm_simple_close_confirmation;	
	channel_class->server_init = oseacomm_simple_server_init;
	
	class->finalize = finalize;
	
	/* Save a pointer to our super class function table so that
	 * we can all the "old" functions. 
	 */
	parent_class = g_type_class_peek_parent (class);
	return; 
}

/**
 * finalize:
 * @object: The profile object that will be destroyed.
 * 
 * This function should release all the resources allocated by
 * this profile instance.
 **/
static void
finalize (GObject *object)
{
	/* dealloc stuff here: */
	
	/* When we are done, lets call the destructor from the
	 * parent class to let it do the rest of the job. We
	 * can't touch the object after this call. */
	parent_class->finalize (object);
	
}



/**
 * oseacomm_simple_get_type:
 * 
 * This function will register the profile in the GLib type system.
 * 
 * Return value: the GType identifier.
 **/
GType 
oseacomm_simple_get_type (void)
{
	static GType oseacomm_simple_type = 0;
	if (!oseacomm_simple_type) {
		static GTypeInfo type_info = {
			sizeof (CoyoteSimpleClass),
			NULL,
			NULL,
			(GClassInitFunc) oseacomm_simple_class_init,
			NULL,
			NULL,
			sizeof (CoyoteSimple),
			16,
			(GInstanceInitFunc) oseacomm_simple_init
		};
		oseacomm_simple_type = g_type_register_static (RR_TYPE_CHANNEL, 
							     "CoyoteSimple", 
							     &type_info, 0);

		/* Tell RoadRunner which URI this profile will 
		 * identify itself with */
		rr_channel_set_uri (oseacomm_simple_type, COYOTE_SIMPLE_URI);
	}
	return oseacomm_simple_type;
}

gboolean oseacomm_simple_server_init     (RRChannel *channel, 
					const gchar *piggyback, 
					GError **error)
{
	CoyoteSimpleCfg * configuration = channel->instance_config;
	
	rr_channel_set_aggregate (channel, FALSE);	

	// Check for instance_config
	if ((configuration) && (configuration->server_init_cb)) {
		return configuration->server_init_cb (channel, piggyback, error);
	}

	// Check for global_config
	configuration = channel->global_config;
	if ((configuration) && (configuration->server_init_cb)) {
		return configuration->server_init_cb (channel, piggyback, error);
	}

	return TRUE;
}

gboolean oseacomm_simple_close_indication  (RRChannel *channel, gint code,
					  const gchar *xml_lang, 
					  const gchar *diagnostic,
					  GError **error)
{
	CoyoteSimpleCfg * configuration = channel->instance_config;
	
	// Check for instance_config
	if ((configuration) && (configuration->close_indication_cb)) {
		return configuration->close_indication_cb (channel, code, xml_lang, diagnostic, error);
	}
	
	// Check for global_config
	configuration = channel->global_config;
	if ((configuration) && (configuration->close_indication_cb)) {
		return configuration->close_indication_cb (channel, code, xml_lang, diagnostic, error);
	}
	
	return TRUE;
}

void oseacomm_simple_close_confirmation (RRChannel *channel, gint code,
					   const gchar *xml_lang, 
					   const gchar *diagnostic)
{
	CoyoteSimpleCfg * configuration = channel->instance_config;
	
	// Check for instance_config
	if ((configuration) && (configuration->close_confirmation_cb)) {
		return configuration->close_confirmation_cb (channel, code, xml_lang, diagnostic);
	}
	
	// Check for global_config
	configuration = channel->global_config;
	if ((configuration) && (configuration->close_confirmation_cb)) {
		return configuration->close_confirmation_cb (channel, code, xml_lang, diagnostic);
	}
	
	return;
}


/**
 * oseacomm_simple_frame_available:
 * @channel: The channel (profile) object.
 * @frame: a #RRFrame object received from the other peer.
 * @error: Location to return an error.
 * 
 * This function handles all incoming frames, according to channel
 * configurations. First, it looks for instance configuration, and
 * then for global_configurations.
 *
 * If frame_available_cb callback is set in some of this
 * configurations, this callback is called. If it's not set, and
 * complete_message_cb is set, it manages a buffer aggregating all
 * incoming frames in a message. This message has a size limit: 2KB
 * for MSG and ERR messages, and 10KB for RPY and ANS messages.
 *
 * If neither complete_message_cb nor frame_available_cb are set, it
 * refuses incoming messages.
 * 
 * Return value: 
 **/
static gboolean
oseacomm_simple_frame_available (RRChannel *channel, RRFrame *frame, GError **error)
{
	CoyoteSimple * oseacomm_channel;
	CoyoteSimpleCfg * global_cfg, * instance_cfg;
	GString * string;

	oseacomm_channel = COYOTE_SIMPLE (channel);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Frame available ");

	// Get channel configuration	       		 
	global_cfg = (CoyoteSimpleCfg *) RR_CHANNEL(channel)->global_config;		
	instance_cfg = (CoyoteSimpleCfg *) RR_CHANNEL(channel)->instance_config;

	// Check if were set frame_available_cb (in instance_cfg or in global_cfg)
	// If it was set, execute corresponding callback and return
	
	if (instance_cfg && instance_cfg->frame_available_cb) {
		return instance_cfg->frame_available_cb (channel, frame, error);
	}

	if (global_cfg && global_cfg->frame_available_cb) {
		return global_cfg->frame_available_cb (channel, frame, error);
	}

	// If not, oseacomm_simple must manage the buffer, and execute
	// complete message callback 

	// So, first, we must check if complete_message_cb is set.  If
	// not, return with error (we cannot accept a message we won't
	// know how to process it)

	if (! ( (instance_cfg && instance_cfg->complete_message_cb) || 
		(global_cfg && global_cfg->complete_message_cb)))
		return FALSE;

	// Now we proceed according to the message type...

	switch (frame->type) {

	case RR_FRAME_TYPE_MSG:
	case RR_FRAME_TYPE_ERR:				
		// If buffer length is greater than 2KB, then we will
		// ignore the received data (for security) (if you
		// expect to receive MSG_type or ERR_type message
		// greater than 2KB of data you must then use
		// "frame_available_cb" instead of
		// "complete_message_cb")

		//if (oseacomm_channel->buffer->len < 2048)
		oseacomm_channel->buffer = g_byte_array_append (coyote_channel->buffer, 
							      frame->payload, frame->size);
		break;

	case RR_FRAME_TYPE_RPY:
	case RR_FRAME_TYPE_ANS:
		// If buffer length is greater than 10KB, then we will
		// ignore the received data (for security) (if you
		// expect to receive RPY_type or ANS_type message
		// greater than 10KB of data you must then use
		// "frame_available_cb" instead of
		// "complete_message_cb")

		// if (oseacomm_channel->buffer->len < 10240)
		oseacomm_channel->buffer = g_byte_array_append (coyote_channel->buffer, 
							      frame->payload, frame->size);
		
		break;

	default:
		return FALSE;
	}


	// If this was the last frame, then we call the handler of complete_message
	
	if (!frame->more) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Calling user defined callback for complete message");
		
		// Call to a user defined function.
		string = g_string_new_len (oseacomm_channel->buffer->data, coyote_channel->buffer->len);
		
		if (instance_cfg && instance_cfg->complete_message_cb) 
			instance_cfg->complete_message_cb 
				(channel, frame, string, 
				 instance_cfg->complete_message_data,
				 instance_cfg->complete_message_custom_data);
		else
			global_cfg->complete_message_cb 
				(channel, frame, string,
				 global_cfg->complete_message_data,
				 global_cfg->complete_message_custom_data);
		
		g_byte_array_free (oseacomm_channel->buffer, TRUE);
		oseacomm_channel->buffer = g_byte_array_new ();

		g_string_free (string, TRUE);

		return TRUE;
	}

	return TRUE;
}

/**
 * oseacomm_simple_start:
 * @connection: a #RRConnection object
 * @config: fiil me
 * @error: location to store an error of type RR_ERROR or RR_BEEP_ERROR.
 * 
 * This function starts a new channel on @connection using the
 * CoyoteSimple profile.
 * 
 * Return value: A CoyoteSimple object on success and %NULL on failure.
 **/
CoyoteSimple * oseacomm_simple_start (RRConnection *connection, 
				    CoyoteSimpleCfg * config, 
				    GError **error)
{
	CoyoteSimple * channel;
	
	g_return_val_if_fail (RR_IS_CONNECTION (connection), NULL);

	channel = (CoyoteSimple *) rr_connection_start (connection, NULL, 
							TYPE_COYOTE_SIMPLE, config, 
							error);
	if (channel == NULL) {
		return NULL;
	}
	
	rr_channel_set_aggregate (RR_CHANNEL(channel), FALSE);

	return channel;

}

/**
  * oseacomm_simple_close:
 * @channel: a #CoyoteSimple object
 * @error: location to store an error of type RR_ERROR or RR_BEEP_ERROR.
 * 
 * This functions closes the channel.
 * 
 * Return value: %TRUE on success, %FALSE on failure.
 **/
gboolean
oseacomm_simple_close (CoyoteSimple * channel, GError **error)
{
	RRManager *manager;
	RRConnection *conn;

	g_return_val_if_fail (IS_COYOTE_SIMPLE (channel), FALSE);

	conn = rr_channel_get_connection (RR_CHANNEL (channel));
	g_return_val_if_fail (conn, FALSE);
	manager = rr_connection_get_manager (conn);
	g_return_val_if_fail (manager, FALSE);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Calling rr_channel_close");
	if (!rr_manager_close_channel_nonblock (manager, RR_CHANNEL (channel), RR_BEEP_CODE_SUCCESS, 
						 NULL, "bye", error)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "rr_channel_close returned with error");
		return FALSE;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "rr_channel_close returned");
	
	/* We don't need it anymore */
	g_object_unref (G_OBJECT (channel));

	return TRUE;
}


/**
 * oseacomm_simple_send:
 * @channel: the channel that will be used to send given data.
 * @data: data to be sent
 * @len: data length.
 * @free: if TRUE, data will be freed afted sent.
 * @error: 
 * 
 * Convenience function to send data throught a given channel. 
 * 
 * Return value: TRUE if sent process was right, otherwise FALSE.
 **/
gboolean 
oseacomm_simple_send (CoyoteSimple * channel, gchar * data, gint len, gint free,GError ** error)
{
	RRMessage * msg;
	
	g_return_val_if_fail (IS_COYOTE_SIMPLE (channel), FALSE);
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sending MSG frame...");
	msg = rr_message_static_new (RR_FRAME_TYPE_MSG, data, len, free);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "MSG frame sent...");

	rr_channel_send_message ( RR_CHANNEL(channel), msg, error);

	return TRUE;
}


/**
 * oseacomm_simple_reply:
 * @channel: the channel that will be used to send given data.
 * @data: data to be sent
 * @len: data length
 * @msg_no: message identifier which is going to be answered
 * @free: if TRUE, data will be freed after sent.
 * @error: 
 * 
 * This convenience function is similar than previous. This function
 * sends a reply message using the given params.
 * 
 * Return value: TRUE if reply process was right, otherwise FALSE.
 **/
gboolean 
oseacomm_simple_reply (CoyoteSimple * channel, gchar * data, gint len, gint msg_no, gint free, GError ** error)
{
	RRMessage * msg;
	
	g_return_val_if_fail (IS_COYOTE_SIMPLE (channel), FALSE);
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sending RPY frame...");
	msg = rr_message_static_new (RR_FRAME_TYPE_RPY, data, len, free);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "RPY frame sent...");

	msg->msgno = msg_no;
	
	rr_channel_send_message ( RR_CHANNEL(channel), msg, error);

	return TRUE;
}

/**
 * oseacomm_simple_reply_error:
 * @channel: the channel that will be used to send given data.
 * @data: data to be sent
 * @len: data length
 * @msg_no: message identifier which is going to be answered
 * @free: if TRUE, data will be freed after sent.
 * @error: 
 * 
 * This convenience function is similar than previous, but this one
 * sends a reply error message using the given params.
 * 
 * Return value: TRUE if reply process was right, otherwise FALSE.
 **/
gboolean 
oseacomm_simple_reply_error (CoyoteSimple * channel, gchar * data, gint len, gint msg_no, gint free, GError ** error)
{
	RRMessage * msg;
	
	g_return_val_if_fail (IS_COYOTE_SIMPLE (channel), FALSE);
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sending ERR frame...");
	msg = rr_message_static_new (RR_FRAME_TYPE_ERR, data, len, free);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "ERR frame sent...");

	msg->msgno = msg_no;
	
	rr_channel_send_message ( RR_CHANNEL(channel), msg, error);

	return TRUE;
}
