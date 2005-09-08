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

#define LOG_DOMAIN "vortex-reader"

static GAsyncQueue * vortex_reader_queue    = NULL;
GList              * con_list    = NULL;
GList              * srv_list    = NULL;

typedef enum {CONNECTION, LISTENER} WatchType;
typedef struct _VortexReaderData {
	WatchType          type;
	VortexConnection * connection;
	gint               listener;
}VortexReaderData;

/**
 * __vortex_reader_join_frames:
 * @channel: 
 * @frame: 
 * 
 * Join incoming frames into a single one if complete flag is
 * defined. It also check incoming frame received so if some error is
 * found the connection is set to be not connected and a FALSE is returned.
 * 
 * Return value: TRUE if frame joining was sucessful or FALSE if fails
 **/
gboolean __vortex_reader_join_frames (VortexChannel * channel, VortexFrame * frame)
{
	VortexFrame * previous_one;
	VortexFrame * new_one;

	// check if we have the first frame with more flag set
	if (vortex_channel_get_previous_frame (channel) == NULL) {
		vortex_channel_set_previous_frame (channel, frame);
		return TRUE;
	}
	// get a reference to previous frame
	previous_one = vortex_channel_get_previous_frame (channel);

	// get a new reference to a new allocated frame which have the content of previous_one and frame
	new_one      = vortex_frame_join (previous_one, frame);

	// the new_one as the new previous_frame for the next msg to come
	vortex_channel_set_previous_frame (channel, new_one);

	// if new_one is NULL this means frame joining have failed, broken beep peer
	if (new_one == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "received a frame part of a fragment which have incompatible values (sync lost, closing session)..");
		__vortex_connection_set_not_connected (vortex_channel_get_connection (channel),
						       "received a frame part of a fragment which have incompatible values");
	}
	// now free resources allocated by previous one and actual frame
	if (previous_one)
		vortex_frame_free (previous_one);
	vortex_frame_free (frame);

	// return if the joining process was ok
	return (new_one != NULL);
}

/**
 * __vortex_reader_process_socket:
 * @connection: the connection which have something to be read
 * 
 * The main purpose of this function is to dispatch receive frames
 * into the aproppiate channel. It also makes all checks to ensure the
 * frame receive have all indicators (seqno, channel, message numer,
 * payload size correctness,..) to ensure the channel receive correct
 * frames and filter those ones which have something wrong.
 *
 * This function also manage frame fragment joining. There are two
 * levels of frame fragment managed by the vortex reader.
 * 
 * We call the first level of fragment, the one described at RFC3080,
 * as the complete frame which belongs to a group of frames which
 * conform a message which was splitted due to channel window size
 * restrictions.
 *
 * The second level of fragment happens the vortex reader receive a
 * frame header which describes a frame size payload to receive but
 * not all payload was receive. This can happen because vortex uses
 * non-blocking socket configuration so it can avoid DOS attack. But
 * this behaviour introduce the asynchronous problem of reading at the
 * moment where the whole frame was not received.
 * 
 **/
void __vortex_reader_process_socket (VortexConnection * connection)
{

	VortexFrame      * frame;
	VortexChannel    * channel;
	gboolean           more;
	gchar            * raw_frame;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "something to read");

	// read all frames received from remote site
	frame   = vortex_frame_get_next (connection);
	if (frame == NULL) 
		return;

	if (getenv ("VORTEX_DEBUG")) {
		raw_frame = vortex_frame_get_raw_frame (frame);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "frame received (before all filters)\n%s",
		       raw_frame);
		g_free (raw_frame);
	}

	
	// check if this connection is being initially accepted
	if (GPOINTER_TO_INT (vortex_connection_get_data (connection, "initial_accept"))) {
		// it doesn't matter to have a connection accepted or
		// not accepted to the vortex reader mission, so
		// simply call second step accept and return.
		__vortex_listener_second_step_accept (frame, connection);
		return;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "passed initial accept stage");


	// now we have a frame, try to invoke second level for received
	if (!vortex_connection_channel_exists (connection, 
					       vortex_frame_get_channel (frame))) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "received a frame refering to a non-opened channel, closing session");
		__vortex_connection_set_not_connected (connection, 
						       "received a frame refering to a non-opened channel, closing session");
		return;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "passed connection existance stage");	

	// channel exists, get a channel reference
	channel = vortex_connection_get_channel (connection,
						 vortex_frame_get_channel (frame));

	// critial and mutually exclusive section
	vortex_channel_lock_to_update_received (channel);

	switch (vortex_frame_get_type (frame)) {
	case VORTEX_FRAME_TYPE_MSG:
		// MSG frame type: check if msgno is correct
		if (vortex_channel_get_next_expected_msg_no (channel) != vortex_frame_get_msgno (frame)) {
			vortex_channel_unlock_to_update_received (channel);
			__vortex_connection_set_not_connected (connection, "expected message number for channel wasn't found");
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "expected message number %d for channel wasn't found, received %d",
			       vortex_channel_get_next_expected_msg_no (channel), 
			       vortex_frame_get_msgno (frame));
			return;
		}
		break;
	case VORTEX_FRAME_TYPE_RPY:
	case VORTEX_FRAME_TYPE_ERR:
		// RPY or ERR frame type: check if reply is expected
		if (vortex_channel_get_next_expected_reply_no (channel) != vortex_frame_get_msgno (frame)) {
			vortex_channel_unlock_to_update_received (channel);
			__vortex_connection_set_not_connected (connection, "expected reply message previous to received");
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "expected reply message %d previous to received %d",
			       vortex_channel_get_next_expected_reply_no (channel),
			       vortex_frame_get_msgno (frame));
			return;
		}
		break;
	default:
		// do not make nothing (only checking msgno and rpyno)
		break;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "passed message number checking stage");


	// check next sequence number, this check is allways applied
	if (vortex_channel_get_next_expected_seq_no (channel) !=
	    vortex_frame_get_seqno (frame)) {
		vortex_channel_unlock_to_update_received (channel);
		__vortex_connection_set_not_connected (connection, "expected seq no number for channel wan't found");
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "expected seq no %d for channel wasn't found, received %d",
		       vortex_channel_get_next_expected_seq_no (channel),
		       vortex_frame_get_seqno (frame));
		return;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "passed next expected seq number checking stage");
	
	// update channel internal status for next incoming messages
	switch (vortex_frame_get_type (frame)) {
	case VORTEX_FRAME_TYPE_MSG:
		// is a MSG frame type: update msgno and seqno
		vortex_channel_update_status_received (channel, 
						       vortex_frame_get_payload_size (frame), 
						       vortex_frame_get_more_flag (frame) ?
						       UPDATE_SEQ_NO :
						       UPDATE_SEQ_NO | UPDATE_MSG_NO);
		break;
	case VORTEX_FRAME_TYPE_RPY:
	case VORTEX_FRAME_TYPE_ERR:
		// flag the channel with reply_proccessed so, close
		// channel process can avoid to close the
		// channel. This is necesary because the channel close
		// process can be waiting to received (and process)
		// all replies for message sent to be able to close
		// the channel.  
		// 
		// And we have to do this flaging before updating
		// channel status because close process will compare
		// messages sent to message replies receive.
		vortex_channel_flag_reply_processed (channel, FALSE);
		

		// is a ERR or RPY type: update rpy no and seqno
		vortex_channel_update_status_received (channel, 
						       vortex_frame_get_payload_size (frame), 
						       vortex_frame_get_more_flag (frame) ? 
						       UPDATE_SEQ_NO : 
						       UPDATE_SEQ_NO| UPDATE_RPY_NO);
		break;
	case VORTEX_FRAME_TYPE_ANS:
	case VORTEX_FRAME_TYPE_NUL:
		// is not a MSG frame type: update seqno
		vortex_channel_update_status_received (channel, 
						       vortex_frame_get_payload_size (frame), 
						       UPDATE_SEQ_NO);
		break;
	case VORTEX_FRAME_TYPE_UNKNOWN:
		// nothing to do
		break;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "passed channel update status due to frame received stage");

	// end critial and mutually exclusive section
	vortex_channel_unlock_to_update_received (channel);

	// if we have a frame to be joined then threat it instead of invoke
	// frame received handler check for more flag
	more = vortex_frame_get_more_flag (frame);

	// check complete frames flags
	if ((more || (!more && vortex_channel_have_previous_frame (channel))) && 
	    vortex_channel_have_complete_flag (channel)) {

		// join frames and return if fail or frame join result is also
		// a incomplete frame
		if ((!__vortex_reader_join_frames (channel, frame)) || more) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "frame fragmented, joined or frame fragment faulty");
			return;
		}
		
		// now we have completed the frame, deliver it
		frame = vortex_channel_get_previous_frame (channel);
		vortex_channel_set_previous_frame (channel, NULL);
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "passed channel completion checking stage");

	// invoke frame received handler for second level and, if not defined, the first level handler
	if (vortex_channel_invoke_received_handler (channel, frame)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "frame delivered on second level handler channel=%d msgno=%d",
		       vortex_frame_get_channel (frame), vortex_frame_get_msgno (frame));
		return; // frame was sucessfully delivered
	}
	
	
	// if not, try to deliver to first level. If second level invocation was ok, the frame have been
	// freed. If not, the first level will do this
	if (vortex_profiles_invoke_frame_received (vortex_channel_get_profile    (channel),
						   vortex_channel_get_number     (channel),
						   vortex_channel_get_connection (channel),
						   frame)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "frame delivered on second level handler channel=%d msgno=%d",
		       vortex_frame_get_channel (frame), vortex_frame_get_msgno (frame));
		return; // frame was sucessfully delivered
	}
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
	       "unable to deliver incoming frame, no first or second level handler defined, dropping frame");

	// in case of a frame reply
	if (vortex_frame_get_type (frame) == VORTEX_FRAME_TYPE_RPY ||
	    vortex_frame_get_type (frame) == VORTEX_FRAME_TYPE_ERR) {
		// flag this frame over the channel to be delivered because a
		// dead-lock can happen if we receive a close message at this
		// point. We have to be able to close a channel having defined
		// a frame receive handler for any level or not
		vortex_channel_flag_reply_processed (channel, TRUE);
		
	}
	
	// unable to deliver the frame, free it
	vortex_frame_free (frame);

	// that's all I can do
	return;
}

gboolean vortex_reader_register_watch (VortexReaderData * data)
{
	VortexConnection * connection;

	switch (data->type) {
	case CONNECTION:
		connection = data->connection;
		g_free (data);
		if (!vortex_connection_is_ok (connection, FALSE)) {
			// check if we can free this connection
			vortex_connection_unref (connection, "vortex reader (process)");
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "received a non-valid connection, ignoring it");
			return FALSE;
		}
			
		// now we have a first connection, we can start to wait
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "new connection to be watched (%d)", 
		       vortex_connection_get_socket (connection));
		con_list = g_list_append (con_list, connection);

		break;
	case LISTENER:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "new listener connection to be watched (%d)",
		       data->listener);
		srv_list = g_list_append (srv_list, GINT_TO_POINTER (data->listener));
		g_free (data);
		break;
	}		
	
	return TRUE;
}

void vortex_reader_read_queue ()
{
	VortexReaderData * data;

 label:
	data = g_async_queue_pop (vortex_reader_queue);
	if (!vortex_reader_register_watch (data))
		goto label;

	return;
}

void vortex_reader_read_pending ()
{
	VortexReaderData * data;
	gint               length;

	length = g_async_queue_length (vortex_reader_queue);
	while (length > 0) {
		
		length--;
		data = g_async_queue_pop (vortex_reader_queue);

		vortex_reader_register_watch (data);
	}
	return;
}

gint   __vortex_reader_build_set_to_watch (fd_set * on_reading)
{
	GList            * new_list    = NULL;
	GList            * list_aux    = g_list_first (con_list);
	gint               max_fds     = 0;
	gint               fds         = 0;
	gint               actual_size = g_list_length (list_aux);
	VortexConnection * connection;

	// read connection list
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		connection = list_aux->data;
		if (!vortex_connection_is_ok (connection, FALSE)) {
			vortex_connection_unref (connection, "vortex reader (process)");
			continue;
		}
		
		fds        = vortex_connection_get_socket (connection);
		max_fds    = MAX (fds, max_fds);
		FD_SET (fds, on_reading);
		new_list   = g_list_append (new_list, connection);
	}
	// update connection list
	g_list_free (con_list);
	con_list = new_list;

	// read listener list
	list_aux   = g_list_first (srv_list);
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		fds = GPOINTER_TO_INT (list_aux->data);
		max_fds    = MAX (fds, max_fds);
		FD_SET (fds, on_reading);
	}


	// connection list watch
	if (actual_size != g_list_length (con_list)) {

		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "connection watched by the reader");
		list_aux        = g_list_first (con_list);

		for (;list_aux; list_aux = g_list_next (list_aux)) {
			connection = list_aux->data;
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "   connection to id:%d) %s:%s", 
			       vortex_connection_get_id   (connection),
			       vortex_connection_get_host (connection),
			       vortex_connection_get_port (connection));
		}
	}

	return max_fds;
	
}

void __vortex_reader_check_connection_list (fd_set * on_reading)
{
	GList             * list_aux   = g_list_first (con_list);
	GList             * new_list   = NULL;
	gint                fds        = 0;
	VortexConnection  * connection = NULL;

	// g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "checking connection pool (connections: %d)",
	// g_list_length (con_list));
	for (; list_aux; list_aux = g_list_next (list_aux)) {

		// check if we have to keep on listening on this connection
		connection = list_aux->data;
		if (!vortex_connection_is_ok (connection, FALSE)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "free non-ok connection resources");
			vortex_connection_unref (connection, "vortex reader (process)");
			continue;
		}
		
		// get the connection and socket.
	        fds = vortex_connection_get_socket (connection);
		
		// ask if this socket have changed
		if (FD_ISSET (fds, on_reading)) {
			__vortex_reader_process_socket (connection);
		}

		// check the connection
		if (!vortex_connection_is_ok (connection, FALSE)) {
			vortex_connection_unref (connection, "vortex reader (process)");
			continue;
		}

		// build up new connection list
		new_list = g_list_append (new_list, connection);

	}
	// g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "connection pool checked");

	// set the new connection list to be watched
	// g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "updating watching connection list to: %d", g_list_length (new_list));
	g_list_free (con_list);
	con_list = g_list_first (new_list);

	return;
}

void __vortex_reader_check_listener_list (fd_set * on_reading)
{
	GList * list_aux = g_list_first (srv_list);
	gint    fds      = 0;

	// g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "checking listener pool (listeners: %d)", 
	// g_list_length (srv_list));
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		
		// get the connection and socket.
		fds        = GPOINTER_TO_INT (list_aux->data);
		if (FD_ISSET (fds, on_reading)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "listener (%d) have requests, processing..", fds);
			vortex_listener_accept_connections (fds);
		}
	}
	// g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "listener pool checked");
	
	return;
}

gpointer __vortex_reader_run (GAsyncQueue * vortex_reader_queue)
{
	fd_set             on_reading;
	gint               max_fds     = 0;
	gint               result;
	gint               error_tries = 0;
	gchar            * error_msg;
	struct timeval     tv;

	// first step. Waiting blocked for our first connection to listen
 __vortex_reader_run_first_connection:
	vortex_reader_read_queue ();

	while (TRUE) {
		// reset descriptor set
		FD_ZERO (&on_reading);

		// build socket descriptor to be readed
		max_fds = __vortex_reader_build_set_to_watch (&on_reading);

		if ((g_list_length (con_list) == 0) && (g_list_length (srv_list) == 0)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "no more connection to watch for, putting into sleep thread");
			goto __vortex_reader_run_first_connection;
		}
		
		// init wait
		tv.tv_sec    = 0;
		tv.tv_usec   = 500000;
		result       = select (max_fds + 1, &on_reading, NULL, NULL, &tv);

		// check result
		if ((result == VORTEX_SOCKET_ERROR) && (errno == EINTR))
			continue;

		// check errors
		if ((result < 0) && (errno != 0)) {
			error_msg = vortex_errno_get_last_error ();
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
			       "an error have ocurred while polling actual connection pool: result=%d, errno=%d, error message was: %s",
			       result, 
			       errno,
			       error_msg ? error_msg : "");
			

			error_tries++;
			if (error_tries == 2) {
				g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
				       "tries have been reached on reader, exiting..");
				return NULL;
			}
			continue;
		}

		// check for each listener
		__vortex_reader_check_listener_list (&on_reading);

		// check for each connection to be watch is it have check
		__vortex_reader_check_connection_list (&on_reading);

		// we have finished the connection dispatching, so read the pending queue elements
		// to be watched
		
		// reset error tries
		error_tries = 0;

		// read new connections to be managed
		vortex_reader_read_pending ();

		if (result > 0) 
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "watching %d connection", 
			       g_list_length (con_list) + g_list_length (srv_list));

	}


	return NULL;
}

/**
 * vortex_reader_watch_connection:
 * @connection: 
 * 
 * Adds a new connection to be watched on vortex reader process. This function is for
 * internal vortex library use. 
k **/
void vortex_reader_watch_connection (VortexConnection * connection)
{
	VortexReaderData * data;
	g_return_if_fail (vortex_connection_is_ok (connection, FALSE));
	g_return_if_fail (vortex_reader_queue);

	if (!vortex_connection_set_nonblocking_socket (connection)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to set non-blocking I/O operation, sking connection registration, closing session");
 		return;
	}

	// increase reference counting
	vortex_connection_ref (connection, "vortex reader (process)");
       
	// prepare data to be queued
	data             = g_new0 (VortexReaderData, 1);
	data->type       = CONNECTION;
	data->connection = connection;

	// push data
	g_async_queue_push (vortex_reader_queue, data);

	return;
}

/**
 * vortex_reader_watch_listener:
 * @listener: 
 * 
 * Install a new listener to watch for new incomming connections.
 **/
void vortex_reader_watch_listener   (gint listener)
{
	VortexReaderData * data;
	g_return_if_fail (listener > 0);
	
	// prepare data to be queued
	data             = g_new0 (VortexReaderData, 1);
	data->type       = LISTENER;
	data->listener   = listener;

	// push data
	g_async_queue_push (vortex_reader_queue, data);

	return;
}

/**
 * vortex_reader_run:
 * @: 
 * 
 * Creates the reader thread process. It will be waiting for any
 * connection that have changed to read its connect and send it
 * appropiate channel reader.
 * 
 * Return value: 
 **/
void vortex_reader_run () 
{
	// create vortex queue
	vortex_reader_queue    = g_async_queue_new ();

	// create the vortex reader main thread
	g_thread_create ((GThreadFunc) __vortex_reader_run,
			 (gpointer)    vortex_reader_queue, 
			 FALSE, NULL);
}

