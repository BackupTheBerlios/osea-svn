//  LibVortex:  A BEEP (RFC3080/RFC3081) implementation.
//  Copyright (C) 2005 Advanced Software Production Line, S.L.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation; either version 2.1 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free
//  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
//  02111-1307 USA
//  
//  You may find a copy of the license under this software is released
//  at COPYING file. This is LGPL software: you are wellcome to
//  develop propietary applications using this library withtout any
//  royalty or fee but returning back any change, improvement or
//  addition in the form of source code, project image, documentation
//  patches, etc. 
//
//  You can also contact us to negociate other license term you may be
//  interested different from LGPL. Contact us at:
//          
//      Postal address:
//         Advanced Software Production Line, S.L.
//         C/ Dr. Michavila Nº 14
//         Coslada 28820 Madrid
//         Spain
//
//      Email address:
//         info@aspl.es - http://fact.aspl.es
//
#include <vortex.h>

#define LOG_DOMAIN "vortex-greetings"


/**
 * vortex_greetings_send:
 * @connection: 
 * 
 * Sends the BEEP init connection greeting over the channel 0 of this @connection
 * This init greetings message reply is sent by actual listener role to remote beep client peer.
 * 
 * Return value: TRUE if greetings message was sent or FALSE if not
 **/
gboolean vortex_greetings_send (VortexConnection * connection)
{
	gchar         * the_payload;
	gchar         * profiles            = NULL;
	gchar         * str_aux;
	GList         * registered_profiles = vortex_profiles_get_actual_list ();
	GList         * list_aux;

	// build up supported registered profiles
	if (registered_profiles == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "unable to build and send greetings message: unable to found any profile registered");
		return FALSE;
	}

	list_aux = g_list_first (registered_profiles);
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		if (profiles == NULL)
			profiles = g_strdup_printf ("   <profile uri='%s' />\r\n",
						    (gchar *) list_aux->data);
		else {
			str_aux  = profiles;
			profiles = g_strdup_printf ("%s   <profile uri='%s' />\r\n",
						    profiles, 
						    (gchar *) list_aux->data);
			g_free (str_aux);
		}
	}
	g_list_free (registered_profiles);

	// build greetings message using registered profile info
	the_payload = g_strdup_printf ("%s%s%s%s",
				       "Content-Type: application/beep+xml\r\n\r\n",
				       "<greeting>\r\n",
				       profiles,
				       "</greeting>\r\n");
	g_free (profiles);

	// send the message
	if (!vortex_channel_send_rpy (vortex_connection_get_channel (connection, 0),
				      the_payload,
				      strlen (the_payload),
				      0)) {
		__vortex_connection_set_not_connected (connection, "unable to send listener greetings message");
		g_free (the_payload);
		return FALSE;
	}

	g_free (the_payload);
	return TRUE;
}

/**
 * vortex_greetings_process:
 * @connection: connection where a greeting reply is expected.
 * 
 * This functions helps vortex library to parse and process greetings
 * message response.  The greetings message is a important piece of
 * the BEEP protocol. On this message, profiles for listener peer are
 * sent, so actual BEEP client can figure out about is posibility of
 * connection to remote site.
 *
 * This function should not be useful for vortex library consumer. It
 * is used mainly by vortex internal purposes.
 * 
 * Return value: The frame readed or NULL if fais
 **/
VortexFrame *  vortex_greetings_process (VortexConnection * connection)
{
	VortexFrame        * frame;

	// because this is a really especial case where we need to get
	// totally blocked until the *WHOLE* frame is receive, we set
	// the connection as blocking 
	frame = vortex_frame_get_next (connection);
	if (frame == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "no frame received from remote peer");
		return NULL;
	}
	// ensure from this point to be frame not NULL 
	if (vortex_greetings_is_reply_ok (frame, connection))
		return frame;
	return NULL;
}

/**
 * vortex_greetings_is_reply_ok:
 * @frame: the frame which contains the greetings reply.
 * 
 * This function should not be useful for vortex library
 * consumers.This function returns if the greeting reply is ok. The
 * greeting reply is supposed to be holded the the received frame.
 *
 * If the frame is wrong this function free the frame.
 * 
 * Return value: TRUE if frame greeting reply is ok or FALSE if not.
 **/
gboolean       vortex_greetings_is_reply_ok    (VortexFrame      * frame, VortexConnection * connection)
{
	VortexChannel * channel;

	// check greetings reply
	if (vortex_frame_get_type (frame) != VORTEX_FRAME_TYPE_RPY) {
		__vortex_connection_set_not_connected (connection, "frame error, expected RPY frame type on greetings process");
		vortex_frame_free (frame);
		return FALSE;
	}

	// check frame header spec
	if (vortex_frame_get_channel (frame) != 0 || 
	    vortex_frame_get_msgno   (frame) != 0 || 
	    vortex_frame_get_seqno   (frame) != 0) {
		__vortex_connection_set_not_connected (connection, "frame error, expected greetings message on channel 0, message 0 or sequence number 0");
		vortex_frame_free (frame);
		return FALSE;
	}
	
	// check content-type reply
	if ((vortex_frame_get_content_type (frame) == NULL) ||
	    (g_strcasecmp (vortex_frame_get_content_type (frame), "application/beep+xml"))) {

		__vortex_connection_set_not_connected (connection, 
						       "frame error, expected content type: application/beep+xml, not found");
		vortex_frame_free (frame);
		return FALSE;
	}

	// update channel status
	channel = vortex_connection_get_channel (connection, 0);
	vortex_channel_update_status_received (channel, vortex_frame_get_payload_size (frame),
					       UPDATE_SEQ_NO);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "greetings frame header specificaion is ok");
	
	return TRUE;	
}

/**
 * vortex_greetings_client_send:
 * @connection: 
 * 
 * This function helps vortex library to be able to complete initial session negociatiion. It
 * send the session greetings for a client role. You can se vortex_greetings_send documentation
 * for more details. 
 *
 * This function is mainly used by vortex library for its internal porpuses, so it should not be
 * useful for vortex library consumers.
 * 
 * Return value: TRUE greetins message was sent or FALSE if not.
 **/
gboolean      vortex_greetings_client_send     (VortexConnection * connection)
{
	gchar * the_payload;
	
	the_payload = g_strdup_printf ("%s%s",
				       "Content-Type: application/beep+xml\r\n\r\n",
				       "<greeting />\r\n");
	// send the message
	if (!vortex_channel_send_rpy (vortex_connection_get_channel (connection, 0),
				      the_payload,
				      strlen (the_payload),
				      0)) {
		g_free (the_payload);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,  "unable to send initial client greetings message");
		__vortex_connection_set_not_connected (connection, 
						       "unable to send initial client greetings message");
		return FALSE;
	}
	
	g_free (the_payload);				      
	return TRUE;
}

/**
 * vortex_greetings_client_process:
 * @connection: 
 * 
 * This function helps vortex library to be able to process client greetings response process.
 * This function is mainly used by vortex library for its internal puporses. This function should
 * not be useful for vortex library consumers.
 * 
 * Return value: The vortex frame readed or NULL if vail.
 **/
VortexFrame *  vortex_greetings_client_process (VortexConnection * connection)
{
	VortexFrame * frame = vortex_greetings_process (connection);

	return frame;
}
