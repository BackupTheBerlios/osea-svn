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
#define LOG_DOMAIN "vortex-channel"
#include <vortex.h>

struct _VortexChannel {
	gint                    channel_num;
	gint                    last_message_sent;
	gint                    last_message_received;
	gint                    last_reply_sent;
	gint                    last_reply_expected;
	gint                    last_seq_no;
	gint                    last_seq_no_expected;
	// the profile
	// ~~~~~~~~~~~
	gchar                 * profile;
	// Defines the profile under the channel is working.

	VortexHash            * data;

	// is_opened
	// ~~~~~~~~~
	gboolean                is_opened;
	// This value allows to detect if a channel is closed and
	// cannot be used anymore for any type of tramsmision. This
	// value is actually used by vortex_channel_send_msg,
	// __vortex_channel_common_rpy.
	
	// being_closed
	// ~~~~~~~~~~~~
	gboolean                being_closed;
	// This values is similiar to is_opened but allows a different
	// error detection. While a channel is closed (a process that
	// can take a while) BEEP RFC 3080 says no msg can be sent but
	// more msg can be received and that msg must be reply so,
	// once a channel is requested to be closed, it can still
	// receive more msg and must be replied. This value is actuall
	// used by vortex_channel_send_msg.
	
	gint                    window_size;
	gboolean                complete_flag;
	VortexFrame           * previous_frame;
	VortexConnection      * connection;
	VortexOnCloseChannel    close;
	gpointer                close_user_data;
	VortexOnFrameReceived   received;
	gpointer                received_user_data;

	// the waiting_msgno
	// ~~~~~~~~~~~~~~~~~
	VortexQueue           * waiting_msgno;
	// This thread safe queue is mainly used by
	// vortex_channel_wait_reply and
	// vortex_channel_invoke_received_handler. A detailed
	// explanation can be found on at
	// vortex_channel_invoke_received_handler.
	
	// As a brief, this waiting queue contains all waiting threads
	// that are waiting to receive a reply no matter what type:
	// ERR, RPY, ANS and NUL.

	// the send mutex:	
	// ~~~~~~~~~~~~~~~
	GMutex                * send_mutex;
	// This mutex is used to make the function
	// vortex_channel_send_msg to be a critical section. This
	// avoid race conditions on getting actual message number to
	// use on other frame header while building and sending a
	// message.
	//
	// This mutex it is also used by __vortex_channel_common_rpy
	// which is a common implementation for err and rpy
	// replies. Due to seqno BEEP specification, this value is
	// shared among all message sent, no matter the type of the
	// frame actually send, ERR, RPY MSG, ANS or NUL. 
	// 
	// So this mutex allows to make critical section and mutally
	// exclusise each function that send one of the previous
	// message types.

	// the receive_mutex:
	// ~~~~~~~~~~~~~~~~~~
	GMutex                * receive_mutex;
        // This mutex is used to make mutally exclusive some parts of
	// the function vortex_channel_invoke_receive_handler and
	// vortex_channel_wait_reply. 
	// 
	// The wait_reply function it is used to wait blocked for a
	// specific reply to a msgno so it creates a async queue and
	// insert it into hash_table that can be used by vortex reader
	// thread to be able to figure out if there are a waiting
	// thread or can directly execute the frame recieved handler.
	//
	// The vortex reader also use this mutex throught the
	// function vortex_channel_lock_to_receive and
	// vortex_channel_unlock_to_receive so it can 
	
	// the update_received_mutex:
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~
	GMutex                * update_received_mutex;
	// This mutex is used on vortex_reader_process_socket to
	// create a critical section where some checkings are done
	// about channel synchronization and the channel update state
	// process. At this moment can be a non-so-good implementation
	// because is posible to define at vortex reader module a
	// static mutex which can do the work the same.

	// the reply_cond:
	// ~~~~~~~~~~~~~~~
	GCond                 * reply_cond;
	// This conditial mutex is used to ensure all replies on the
	// same channel are sent in order.
	//
	// If client application calls to vortex_chhanel_send_reply
	// trying to reply a message having previous message without
	// been replied then application is blocked.

	// the queue_msgs:
	// ~~~~~~~~~~~~~~~
	VortexQueue           * queue_msgs;
	// The queue_msgs is used by the vortex sequencer process as a
	// buffer of frames to be sent.  The sequencer receive request
	// of messages to be sent. Then, them are splitted if the
	// message is larger than channel window size into one or
	// several frames ready to sent.  
	//
	// Once the sequencer have queue some frames to be sent it
	// signal the vortex writer to sent pending frames.

	// the close_cond:
	// ~~~~~~~~~~~~~~
	GCond                 * close_cond;
	// This conditional is used to get blocked close message
	// process and is response. As described in RFC 3080 a channel
	// can receive the channel close message at any time but, must
	// still await to receive its MSG replies. 
	// 
	// Once all replies have been received the channel can be
	// closed and the ok or error message will be sent.
	
	// the close_mutex:
	// ~~~~~~~~~~~~~~~~
	GMutex                * close_mutex;
	// This mutex is used in conjuntion with close_cond to ensure
	// all message replies have been received during the close
	// channel process.

	// the waiting_replies
	// ~~~~~~~~~~~~~~~~~~~
	//
	gboolean               waiting_replies;
	// This variable is used by the
	// vortex_channel_0_frame_received_close_msg to flag a channel
	// that have been blocked on that function because it didn't
	// still receive all replies to message already sent.

	// the reply_processed:
	// ~~~~~~~~~~~~~~~~~~~~
	gboolean               reply_processed;
	// This variable is used by the vortex reader and close
	// channel process to synchronize channel frame replies
	// reception and its proccess and the posible on going channel
	// closing. A channel can be closed until all replies to all
	// message are receive and they are delivered to application
	// level. 

	// the being_sending
	// ~~~~~~~~~~~~~~~~~
	gboolean               being_sending;

	// the pending_mutex
	// ~~~~~~~~~~~~~~~~~
	GMutex                * pending_mutex;
	// This mutex is used to ensure we have sent all replies to
	// all message receive during the close process before getting
	// blocked waiting for the ok message.  This mutex is used at
	// __vortex_channel_block_until_replies_are_sent and
	// vortex_channel_signal_reply_sent_on_close_blocked

	// the pending_cond
	// ~~~~~~~~~~~~~~~~
	GCond                 * pending_cond;
	// The conditional variable is used in conjuntion with
	// previous mutex pending_mutex.

	// the pool
	// ~~~~~~~~
	VortexChannelPool     * pool;
	// If the channel was created inside a pool this variable will
	// be set to point to the channel it belongs so on close
	// operation this module will be able to notify vortex channel
	// pool module to remove the channel reference.

};

#define _error_msg               "Content-Type: application/beep+xml\r\n\r\n<error code='%s'>%s</error>"
#define _channel_start_msg       "Content-Type: application/beep+xml\r\n\r\n<start number='%d'>\r\n   <profile uri='%s' />\r\n</start>\r\n"
#define _channel_start_rpy_msg   "Content-Type: application/beep+xml\r\n\r\n<profile uri='%s' />\r\n"
#define _channel_close_msg       "Content-Type: application/beep+xml\r\n\r\n<close number='%d' code='%s' />\r\n"
#define _ok_msg                  "Content-Type: application/beep+xml\r\n\r\n<ok />\r\n"

typedef struct _VortexChannelData {
	VortexConnection        * connection;
	gint                      channel_num;
	VortexOnChannelCreated    on_channel_created;
	gchar                   * profile;
	gpointer                  user_data;
	VortexOnCloseChannel      close;
	gpointer                  close_user_data;
	VortexOnFrameReceived     received;
	gpointer                  received_user_data;
	gboolean                  threaded;
}VortexChannelData;

/**
 * \defgroup vortex_channel Vortex Channel: Related function to create and manage Vortex Channels.
 */

/// \addtogroup vortex_channel
//@{


/** 
 * @internal
 * @brief Set channel status to be connected.
 *
 * Private function to set channel connected status to TRUE.
 * 
 * @param channel the channel to operate.
 */
void __vortex_channel_set_connected (VortexChannel * channel)
{
	g_return_if_fail (channel);

	channel->is_opened = TRUE;

	return;
}

gboolean __vortex_channel_validate_start_reply (VortexFrame * frame, gchar * _profile)
{

	xmlDocPtr   doc;
	xmlNodePtr  cursor;
	gchar *     profile;
	gboolean    result;

	// check frame type response
	if (vortex_frame_get_type (frame) != VORTEX_FRAME_TYPE_RPY)
		return FALSE;

	// parse xml document
	doc = xmlParseMemory ((xmlChar *) vortex_frame_get_payload (frame), 
			      vortex_frame_get_payload_size (frame)); // * sizeof (char)));
	if (!doc) 
		return FALSE;
	
	// get the root element ( profile element )
	cursor  = xmlDocGetRootElement (doc);
	if (xmlStrcmp (cursor->name, (const xmlChar *) "profile")) {
		xmlFreeDoc (doc);
		return FALSE;
	}
	// check profile requested
	profile = xmlGetProp (cursor, "uri");
	if (g_strcasecmp (profile, _profile))
		result = FALSE;
	else
		result = TRUE;

	g_free (profile);
	xmlFreeDoc (doc);
	return result;
}

// helper function for vortex_channel_new
gpointer __vortex_channel_new (VortexChannelData * data) 
{
	VortexChannel   * channel    = NULL;
	VortexChannel   * channel0   = NULL;
	VortexFrame     * frame      = NULL;
	WaitReplyData   * wait_reply = NULL;
	gint              msg_no;

	// check if remote peer support actual profile
	if (!vortex_connection_is_profile_supported (data->connection, data->profile)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "channel profile not supported, channel can't be created");
		goto __vortex_channel_new_invoke_caller;
	}

	// check if channel to be created already exists
	if ((data->channel_num != 0) && 
	    vortex_connection_channel_exists (data->connection, data->channel_num)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "creating a channel identified by %d that already exists",
		       data->channel_num);
		goto __vortex_channel_new_invoke_caller;
	}

	// check if user wants to create a new channel and automatically allocate a channel number
	if (data->channel_num == 0) {
		data->channel_num      = vortex_connection_get_next_channel (data->connection);
		if (data->channel_num == -1) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
			       "vortex connection get next channel have failed, this could be bad, I mean, really bad..");
			goto __vortex_channel_new_invoke_caller;
		}

	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel num returned=%d", data->channel_num);

	// creates the channel
	channel = vortex_channel_empty_new (data->channel_num, data->profile, data->connection);
	

	// set default handlers
	vortex_channel_set_close_handler    (channel, data->close, data->close_user_data);
	vortex_channel_set_received_handler (channel, data->received, data->received_user_data);
	
	// get the channel 0
	channel0 = vortex_connection_get_channel (data->connection, 0);
	if (channel0 == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "internal vortex error: unable to get channel 0 for a session");

		// free allocated channel and exist
		vortex_channel_free (channel);
		channel = NULL;
		goto __vortex_channel_new_invoke_caller;
	}

	// build up the frame to send
	wait_reply = vortex_channel_create_wait_reply ();
	if (!vortex_channel_send_msg_and_waitv (channel0, &msg_no, wait_reply, 
						_channel_start_msg,
						vortex_channel_get_number (channel),
						data->profile)) {
		channel = NULL;
		vortex_channel_free_wait_reply (wait_reply);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to send start message for channel %d and profile %s",
		       vortex_channel_get_number (channel),
		       vortex_channel_get_profile (channel));
		goto __vortex_channel_new_invoke_caller;
		
	}

	// now, remote peer must accept channel creation or deny it
	// this operation will block us until frame is received
	frame      = vortex_channel_wait_reply (channel0, msg_no, wait_reply);
	if (frame == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "something have failed while received start message response for channel %d under profile %s",
		       vortex_channel_get_number (channel), 
		       vortex_channel_get_profile (channel));

		vortex_channel_free      (channel);
		channel = NULL;
		goto __vortex_channel_new_invoke_caller;
	}

	// check start reply data
	if (!__vortex_channel_validate_start_reply (frame, channel->profile)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "start message reply validation fail, channel %d with profile %s cannot be created",
		       channel->channel_num, channel->profile);
		vortex_channel_free (channel);
		vortex_frame_free   (frame);
		channel = NULL;
		goto __vortex_channel_new_invoke_caller;
	}
	vortex_frame_free (frame);

	// register channel on connection
	if (channel != NULL) {
		// it seems the channel have been created, know we
		// have to check if user have forgot to register
		// profile used. If does, register it for the user.
		if (!vortex_profiles_is_registered (channel->profile)) {
			vortex_profiles_register (channel->profile,
						  // use default start handler
						  NULL, NULL, 
						  // use default close handler
						  NULL, NULL,
						  // use no frame received handler
						  NULL, NULL);
		}

		vortex_connection_add_channel (data->connection, channel);
	}
	
	__vortex_channel_new_invoke_caller:
	// finally, invoke caller with channel result
	if (channel == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "channel=%d creation have failed", 
		       data->channel_num);
	}

	if (data->threaded) {
		data->on_channel_created (channel ? channel->channel_num : -1, 
					  channel ? channel : NULL, 
					  data->user_data);
		g_free (data);
		return NULL;
	}

	g_free (data);
	return channel;
}

/** 
 * @brief Creates a new channel over the given connection.
 *
 * Creates a new channel over the session <i>connection</i> with the channel
 * num <i>channel_num</i> specified. If channel num is 0, the next
 * free channel number is used. The new channel will be created under
 * the term of the profile defined. Both peers must support the given
 * profile, otherwise the channel will not be created.
 *
 * Because the channel is a <i>MSG/RPY</i> transaction between the
 * client and the server, it will block caller until channel is
 * created. 
 * 
 * If you want to avoid getting blocked, you can use
 * <i>on_channel_created</i> handler. If you define this handler, the
 * function will unblock the caller and create the channel on a
 * separated thread. Once the channel is created, caller will be
 * notified throught the callback defined by <i>on_channel_created</i>.
 *
 * There are some events that happens during channel life. They are
 * <i>close handler</i> and <i>received handler</i>. They are executed
 * when the event they represent happens. A description for each
 * handler is the following:
 *
 * <ul>
 *
 * <li>
 * <i>close handler</i>: (see more info on vortex_handlers:
 * VortexOnCloseChannel) This handler is executed when a channel close
 * petition have arrived from remote peer. 
 *
 * If you signal to close channel throught \ref vortex_channel_close,
 * this handler is NOT executed, but it is executed when the given
 * channel receive a remote channel close request. This handler should
 * return TRUE if channel can be closed or FALSE if not.
 * 
 * This can be convenient if there are more data to be sent and to be
 * able to notify remote peer this is happen. 
 * 
 * This handler is optional. You can use NULL and the first level
 * handler for close event will be used. If the first level handler is
 * also not defined, a default handler will be used, which returns
 * allways TRUE. This means to agree to close the channel allways.
 * 
 * You can also define user data to be passed into <i>close
 * handler</i> on its execution. This user data is
 * <i>close_user_data</i>.</li>
 * 
 * <li><i>received handler</i>: (see more info on vortex_handlers:
 * VortexOnFrameReceived) This handler is executed when a channel
 * receive a frame. 
 * 
 * As the same as <i>close</i> handler definition, if you
 * provide a NULL value for this handler the first level handler will
 * be executed. If this first level handler is not defined, frame is
 * dropped.</li>
 *
 * </ul>
 * 
 * As you can see, there are 2 level of handlers to be executed when
 * events happens on channel life.  You can also see
 * \ref vortex_profiles_register documention. Check out also
 * \ref dispatch_schema "this section where is explained in more detail how frames are received". 
 * 
 * Later, to be able to close and free the channels created you must
 * use \ref vortex_channel_close. DO NOT USE \ref vortex_channel_free to close
 * or free channel resources. This function is actually called by
 * Vortex Library when the channel resources can be deallocated.
 *
 * On channel creation, it could happen an application do not register
 * the profile that is going to be used for the new channel. This is a
 * problem because if you don't define the close, start or frame
 * received many problems will appear. 
 * 
 * By default, if \ref vortex_channel_new detects you didn't register
 * a profile, then the function will do it for you. On this automatic profile
 * registration, vortex library will assign the default close and
 * start handler which allways replies TRUE to close and start
 * channels, but frame received handler will still not defined.
 * 
 * As a consecuence you must ensure to register a <i>frame
 * received</i> handler at first level using vortex_profiles function
 * or ensure to register a frame received handler for this function.
 *
 * Again, you may be asking your self why not simply deny channel
 * creation for those petition which didn't define frame received at
 * any level. This is done because there \ref dispatch_schema "are still more method" 
 * to retrieve frames from remote peers, bypassing
 * the frame received handlers (for the second and first levels). 
 *
 * This method can be used with \ref vortex_channel_wait_reply which
 * wait for a especific reply. Check out \ref wait_reply "this section to know more about Wait Reply method".
 *
 * <i><b>NOTE:</b> on threaded mode, channel creation process can fail, as the
 * same as non-threaded mode, so you can also get a null value for
 * both invocation models. 
 * 
 * This executing model is slightly different from
 * \ref vortex_connection_new where the value returned must be checked with
 * \ref vortex_connection_is_ok.
 * 
 * Because connection creation is made only once, and channel creation
 * many, the model to check values for new channel created is to check if
 * returned values is not NULL.</i>
 * 
 *
 * @param connection           session where channel will be created.
 * @param channel_num          the channel number. Using 0 automatically assigns the next channel number free.
 * @param profile              the profile under the channel will be created.
 * @param close                handler to manage channel closing.
 * @param close_user_data      user data to be passed in to close handler.
 * @param received             handler to manage frame reception on channel.
 * @param received_user_data   data to be passed in to <i>received</i> handler.
 * @param on_channel_created   a callback to be able to make channel process to be async.
 * @param user_data            user data to be passed in to <i>on_channel_created</i>.
 * 
 * @return the newly created channel or NULL if fails under
 * non-threaded model.  Under threaded model returned value will
 * allways be NULL and newly channel created will be notifyed on
 * on_channel_created.
 */
VortexChannel * vortex_channel_new (VortexConnection      * connection, 
				    gint                    channel_num, 
				    gchar                 * profile,
				    VortexOnCloseChannel    close,
				    gpointer                close_user_data,
				    VortexOnFrameReceived   received,
				    gpointer                received_user_data,
				    VortexOnChannelCreated  on_channel_created, gpointer user_data)
{
	VortexChannelData * data;

	g_return_val_if_fail (connection, NULL);
	g_return_val_if_fail (vortex_connection_is_ok (connection, FALSE), NULL);
	g_return_val_if_fail (profile, NULL);
	

	// creates data to be passed in to __vortex_channel_new
	data                     = g_new0 (VortexChannelData, 1);
	data->connection         = connection;
	data->channel_num        = channel_num;
	data->close              = close;
	data->close_user_data    = close_user_data;
	data->received           = received;
	data->received_user_data = received_user_data;
	data->profile            = profile;
	data->on_channel_created = on_channel_created;
	data->user_data          = user_data;
	data->threaded           = (on_channel_created != NULL);

	// launch threaded version if on_channel_created is defined
	if (data->threaded) {
		vortex_thread_pool_new_task ((GThreadFunc) __vortex_channel_new, data);
		return NULL;
	}

	return __vortex_channel_new (data);
}


/** 
 * @brief Returns the next message number to be used on this channel.
 *
 * This message number is the one used to message sent. This have
 * nothing to do with next message number expected.
 *
 * This function is used by the Vortex Library for its internal
 * process to figure out what message number to use on next sending request.
 * 
 * 
 * @param channel the channel to work on
 * 
 * @return the message number or -1 if fail
 */
gint            vortex_channel_get_next_msg_no (VortexChannel * channel)
{	
	g_return_val_if_fail (channel, -1);

	return channel->last_message_sent + 1;
}

/**
 * vortex_channel_get_next_expected_msg_no:
 * @channel: 
 * 
 * Returns actual message number expected to be received on this channel. This function must
 * be not confused with vortex_channel_get_next_msg_no. Explanation:
 * Once a channel is created, both peer connected by a session where the channel is created, are
 * expected message from each other and will send messages to each other. In this context, 
 * message that peer A sends to peer B are controled by vortex_get_next_expected_msg_no on B peer
 * side. This means, B is expecting messge 12 to arrive and this '12' will be returned by previous
 * function. Once a message is properly received and processed, this previous number will updated to
 * 13. 
 * Now, when A sends to peer B a message, A needs to now what message number to use on its next
 * message. That why vortex_channel_get_next_msg_no stands for. This function returns actual message
 * number to use or actual message number is waiting other peer to receive.
 *
 * This explanation applies to all frame indication found on this module. As a short path to remember
 * how this function works think as follow:
 * "If actual peer is going to send a message I will use vortex_channel_get_next_* functions to build up
 * it. If I need to validate that incomming message is right I will use vortex_channel_get_next_expected_*
 * functions.
 * 
 * Return value: 
 **/

/** 
 * @brief Retuns actual message number expected to be received on this channel.
 *
 *
 * This function must be not confused with
 * \ref vortex_channel_get_next_msg_no. 
 * 
 * Explanation: Once a channel is created, both peer connected by a
 * session where the channel is created, are expecting messages from
 * each other and will send messages to each other. 
 * 
 * On this context, message that peer A sends to peer B are controled
 * by vortex_get_next_expected_msg_no on B peer side. This means, B is
 * expecting messge 12 to arrive and this '12' will be returned by
 * previous function. 
 * 
 * Once a message is properly received and processed, this previous
 * number will updated to 13.  Now, when A sends to peer B a message,
 * A needs to now what message number to use on its next message. That
 * why vortex_channel_get_next_msg_no stands for. 
 * 
 * This function returns actual message number to use or actual
 * message number is waiting other peer to receive.
 *
 * This explanation applies to all frame indication found on this
 * module. As a short path to remember how this function works think
 * as follow: 
 * 
 * <i>"If actual peer is going to send a message I will use
 * vortex_channel_get_next_* functions to build up it. If I need to
 * validate that incomming message is right I will use
 * vortex_channel_get_next_expected_* functions."</i>
 *
 * 
 * @param channel the channel to operate on.
 * 
 * @return expected message number to recevie or -1 if fails
 */
gint            vortex_channel_get_next_expected_msg_no (VortexChannel * channel)
{
	g_return_val_if_fail (channel, -1);

	return channel->last_message_received + 1;
}


/** 
 * @internal
 * @brief Creates a empty channel structure
 *
 * 
 * Creates a new empty channel struct, filling it with passed in
 * data. This function should not be used from outside of vortex
 * library. It is used for internal vortex library purposes. 
 * 
 * Its basic behaviour is to create a ready to use channel initialized
 * with the correct internal state to be used inside vortex library.
 * So, it can't be useful for vortex consumers.
 * 
 * @param channel_num the channel number to use.
 * @param profile the profile that will manage this channel.
 * @param connection the connection where channel is associated.
 * 
 * @return a new allocated channel.
 */
VortexChannel * vortex_channel_empty_new (gint channel_num,
					  gchar * profile,
					  VortexConnection * connection)
{
	VortexChannel * channel = NULL;

	g_return_val_if_fail (profile, NULL);
	g_return_val_if_fail (vortex_connection_is_ok (connection, FALSE), NULL);

	channel                                 = g_new0 (VortexChannel, 1);
	channel->channel_num                    = channel_num;
	channel->last_message_sent              = -1;
	channel->last_message_received          = -1;
	channel->last_reply_sent                = 0;
	channel->last_reply_expected            = 0;
	channel->last_seq_no                    = 0;
	channel->last_seq_no_expected           = 0;
	channel->is_opened                      = FALSE;
	channel->being_closed                   = FALSE;
	channel->complete_flag                  = TRUE;
	channel->profile                        = g_strdup (profile);
	channel->connection                     = connection;
	channel->window_size                    = 4096;
	channel->waiting_msgno                  = vortex_queue_new ();
	channel->queue_msgs                     = vortex_queue_new ();
	channel->send_mutex                     = g_mutex_new ();
	channel->receive_mutex                  = g_mutex_new ();
	channel->update_received_mutex          = g_mutex_new ();
	channel->reply_cond                     = g_cond_new  ();
	channel->close_cond                     = g_cond_new  ();
	channel->close_mutex                    = g_mutex_new ();
	channel->pending_mutex                  = g_mutex_new ();
	channel->pending_cond                   = g_cond_new  ();
	channel->waiting_replies                = FALSE;
	channel->reply_processed                = TRUE;
	channel->data                           = vortex_hash_new_full (g_str_hash, g_str_equal, NULL, NULL);

	// handle special case, channel 0
	if (channel_num == 0) {
		channel->received = vortex_channel_0_frame_received;
	}

	return channel;
}

/** 
 * @brief Allows to set close channel to the given channel.
 *
 * Set the handler to be executed on this channel when a close channel
 * event arrives.  If you call several times to this function, the
 * handler and user data will be changed to passed in data. So you can
 * use this function to reset handlers definition on this channel.
 *
 * @param channel the channel to configure
 * @param close the close handler to execute
 * @param user_data the user data to pass to <i>close</i> execution
 */
void            vortex_channel_set_close_handler (VortexChannel * channel,
						  VortexOnCloseChannel close,
						  gpointer user_data)
{
	g_return_if_fail (channel);
	
	channel->close           = close;
	channel->close_user_data = user_data;
	
	return;
}

/** 
 * @brief Allows to set frame received handler.
 *
 * Set the handler to be executed on this channel when a <i>received</i>
 * channel event arrives.  If you call several times to this function,
 * the handler and user data will be changed to passed in data. So you
 * can use this function to reset handlers definition on this channel.
 * 
 * @param channel the channel to configure.
 * @param received the start handler to execute.
 * @param user_data the user data to pass to <i>received</i> execution.
 */
void            vortex_channel_set_received_handler (VortexChannel * channel, 
						     VortexOnFrameReceived received,
						     gpointer user_data)
{
	g_return_if_fail (channel);

	channel->received           = received;
	channel->received_user_data = user_data;

	return;
}

/** 
 * @brief Allows to set complete frames flag
 *
 * BEEP protocol defines that a message must be splited into pieces
 * equal to or less than the channel window size. This enable beep
 * peers to avoid receiving to large frames and other problems. As a
 * consecuence, while receiving a response this can be a part of a
 * list of frames.
 * 
 * If don't want to handle frames segments and then join them into a
 * single frame you can set the complete flag to every frame you
 * receive is complete. Vortex will join all fragments and deliver the
 * frame only if is completed.
 *
 *
 * By default, every channel created have the complete flag activated. 
 *
 * @param channel the channel to configure.
 * @param value TRUE activate complete flag, FALSE deactivates it.
 */
void               vortex_channel_set_complete_flag            (VortexChannel * channel,
								gboolean value)
{
	g_return_if_fail (channel);

	channel->complete_flag = value;

	return;
}

/** 
 * @brief Returns if the given channel have stored a previous channel.
 *
 * This function is used by the vortex reader to be able to join frame
 * fragments which are bigger than the channel window size.
 *
 * This function should not be useful for Vortex Library consumers.
 *
 * @param channel the channel to operate on.
 * 
 * @return TRUE if previous frame is defined or FALSE if not
 */
gboolean           vortex_channel_have_previous_frame          (VortexChannel * channel)
{
	g_return_val_if_fail (channel, FALSE);

	return (channel->previous_frame != NULL);
}


/** 
 * @internal
 * @brief Allows to get previous frame stored for the given channel.
 * 
 * @param channel the channel to operate on.
 * 
 * @return the previous frame or NULL if fails
 */
VortexFrame      * vortex_channel_get_previous_frame           (VortexChannel * channel)
{
	g_return_val_if_fail (channel, NULL);

	return channel->previous_frame;
}

/** 
 * @internal
 * @brief Allows to set previous frame for the given channel.
 * 
 * @param channel the channel to operate on.
 * @param frame the frame to sent.
 */
void               vortex_channel_set_previous_frame           (VortexChannel * channel, VortexFrame * frame)
{
	g_return_if_fail (channel);

	channel->previous_frame = frame;

	return;
}

/** 
 * @brief Allows to get actual complete flag status for the given channel.
 *
 * This function allows to get actual complete flag for the given
 * channel. This function is actually used by the vortex reader to be
 * able to join fragment frames. 
 *
 * @param channel the channel to operate on.
 * 
 * @return TRUE if complete flag is defined or FALSE if not.
 */
gboolean           vortex_channel_have_complete_flag           (VortexChannel * channel)
{
	g_return_val_if_fail (channel, FALSE);

	return (channel->complete_flag); 
}

/** 
 * @brief Returns the channel number for selected channel.
 * 
 * @param channel the channel to get the number
 * 
 * @return the channel number or -1 if fail
 */
gint            vortex_channel_get_number (VortexChannel * channel)
{
	g_return_val_if_fail (channel, -1);
//      g_return_val_if_fail (channel->connection, -1);
//	g_return_val_if_fail (vortex_connection_is_ok (channel->connection, FALSE), -1);

	return channel->channel_num;
}

/** 
 * @internal
 * @brief Update channel status for sending messages.
 *
 * This function is for internal vortex library support. It update
 * actual channel status for messages to be sent.  Function also
 * allows to control what values are updated. This is necessary to
 * update only seqno value for a channel and not the msgno while
 * sending RPY messages.
 * 
 * 
 * @param channel the channel to update internal status for next message to be sent
 * @param frame_size the frame size of the frame sent.
 * @param update what values to update
 */
void vortex_channel_update_status (VortexChannel * channel, gint frame_size, WhatUpdate update)
{
	g_return_if_fail (channel);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "updating channel status..");

	// update msgno
	if ((update & UPDATE_MSG_NO) == UPDATE_MSG_NO) {
		channel->last_message_sent +=  1;
	}

	// update rpy no
	if ((update & UPDATE_RPY_NO) == UPDATE_RPY_NO) {
		channel->last_reply_sent   += 1;
	}

	// update seqno
	if ((update & UPDATE_SEQ_NO) == UPDATE_SEQ_NO) {
		channel->last_seq_no        = ((channel->last_seq_no + frame_size) % (MAX_SEQ_NO));
	}

	return;
}

/** 
 * @internal
 * @brief Updates channel status for received messages.
 *
 *
 * This function is for internal vortex library support. It update
 * actual channel status for messages to be received. This is actully
 * used by vortex_reader thread to check incoming messages.
 *
 * 
 * @param channel The channel to update internal status for next message to be received 
 * @param frame_size the frame size of the frame received
 * @param update what parts of the channel status to update.
 */
void vortex_channel_update_status_received (VortexChannel * channel, gint frame_size,
					    WhatUpdate update)
{
	g_return_if_fail (channel);
	
	// update expected msgno
	if ((update & UPDATE_MSG_NO) == UPDATE_MSG_NO) {
		channel->last_message_received += 1;
	}

	// update expected rpy no
	if ((update & UPDATE_RPY_NO) == UPDATE_RPY_NO) {
		channel->last_reply_expected   += 1;
	}

	// update expected seqno
	if ((update & UPDATE_SEQ_NO) == UPDATE_SEQ_NO) {
		channel->last_seq_no_expected   = (guint32) ((channel->last_seq_no_expected + frame_size) % (MAX_SEQ_NO));
	}

	return;
}


/** 
 * @internal
 * @brief Common function support other function to send message.
 * 
 * @param channel the channel where the message will be sent.
 * @param message the message to be sent.
 * @param message_size the message size.
 * @param msg_no message number reference used for this message sending atempt.
 * @param wait_reply optional wait reply to implement Wait Reply method.
 * 
 * @return TRUE if channel was sent or FALSE if not.
 */
gboolean    __vortex_channel_send_msg_common (VortexChannel   * channel,
					      gchar           * message,
					      gint              message_size,
					      gint            * msg_no,
					      WaitReplyData   * wait_reply)
{
	VortexSequencerData * data;

	g_return_val_if_fail (channel,                           FALSE);
	g_return_val_if_fail (message,                           FALSE);
	g_return_val_if_fail (channel->is_opened,                FALSE);

	if (channel->channel_num != 0)
		g_return_val_if_fail (! channel->being_closed,   FALSE);
	

	// lock send mutex
	//g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "lock send_mutex at msg_common channel=%d, message:\n%s", 
	//       vortex_channel_get_number (channel),
	//       message);
	g_mutex_lock (channel->send_mutex);
	channel->being_sending = TRUE;
	// flag the channel to be awaiting to process message


	data                   = g_new0 (VortexSequencerData, 1);
	data->channel          = channel;
	data->type             = VORTEX_FRAME_TYPE_MSG;
	data->channel_num      = vortex_channel_get_number (channel);
	data->msg_no           = vortex_channel_get_next_msg_no (channel);
	data->first_seq_no     = vortex_channel_get_next_seq_no (channel);
	data->message_size     = message_size;
	data->message          = g_strdup (message);

	// return back message no used 
	if (msg_no != NULL) 
		(* msg_no) = vortex_channel_get_next_msg_no (channel);

	// is there a wait reply?
	if (wait_reply != NULL) {
		
		// install queue 
		wait_reply->msg_no_reply = vortex_channel_get_next_msg_no (channel);
		
		// enqueue reply data on this 
		g_mutex_lock   (channel->receive_mutex);
		vortex_queue_push (channel->waiting_msgno, wait_reply);
		g_mutex_unlock (channel->receive_mutex);
	}

	// update channel status
	vortex_channel_update_status (channel, message_size, UPDATE_SEQ_NO | UPDATE_MSG_NO);

	// send data to sequencer
	vortex_sequencer_queue_data (data);

	// unlock send mutex
	// g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unlock send_mutex at msg_common channel=%d", vortex_channel_get_number (channel));
	g_mutex_unlock (channel->send_mutex);

	// flag the channel to be ready (no message being_sending)
	channel->being_sending = FALSE;

	return TRUE;
}

/** 
 * @brief Allows to send a message.
 *
 * Sends the <i>message</i> over the selected <i>channel</i>. It will
 * create the needed frames to be sent to remote peer connected by
 * session where <i>channel</i> is created.
 *
 * On <i>msg_no</i> will be returned the message number used for this
 * delivery. This can be useful for waiting response for this message
 * on this channel. 
 *
 * A usual question you may have is: could I remove the message chunk
 * I've used to call this function. The answer is yes. This function
 * makes a local copy you can directly free used message as follows:
 *
 * \code 
 *
 *      // we create some dinamically allocated message
 *      gchar * message = g_strdup_printf ("%d - %s ..\n", 10, "some kind of very important message");
 *
 *      if (!vortex_channel_send_msg (channel, message, strlen (message), NULL)) {
 *             // some kind of error process and free your resources
 *             g_free (message);
 *
 *      }
 *      // you should free dinamically allocated resources no longer used just after running 
 *      // vortex_channel_send_msg.
 *      g_free (message);
 *
 * \endcode
 * 
 * @param channel where message will be sent
 * @param message the message to send
 * @param message_size the message size (user space calculated, that is actual <i>message</i> size)
 * @param msg_no returns the message number used for this deliver. If NULL the message number will not be returned.
 * 
 * @return TRUE if message could be sent, FALSE if fail
 */
gboolean        vortex_channel_send_msg   (VortexChannel * channel,
					   gchar         * message,
					   gint            message_size,
					   gint          * msg_no)
{
	return __vortex_channel_send_msg_common (channel, message, message_size, msg_no, NULL);
}

/** 
 * @brief Allows to send message using a printf-like format.
 *
 * This function works and does the same like \ref vortex_channel_send_msg but allowing you
 * the define a message in a printf like format.
 * A simple example can be:
 * \code
 *    vortex_channel_send_msgv (channel, "this a value %s", &msg_no, "an string");
 * \endcode
 *
 * It uses stdargs to build up the message to send and uses strlen to
 * figure out the message size to send.  See also \ref vortex_channel_send_msg.
 *
 * 
 * @param channel the channel used to send the message
 * @param msg_no the msgno assigned to message sent
 * @param format the message format (printf-style) 
 * 
 * @return the same as \ref vortex_channel_send_msg.
 */
gboolean        vortex_channel_send_msgv       (VortexChannel * channel,
						gint          * msg_no,
						gchar         * format,
						...)
{
	gchar      * msg;
	gboolean     result;
	va_list      args;

	// initialize the args value
	va_start (args, format);
	// build the message
	msg = g_strdup_vprintf (format, args);
	// end args values
	va_end (args);

	// execute send_msg
	result = vortex_channel_send_msg (channel, msg, strlen (msg), msg_no);
	
	// free value and return
	g_free (msg);
	return result;
	
}

/** 
 * @brief Allows to send a message and start a \ref wait_reply "wait reply".
 *
 * This is especial case of \ref vortex_channel_send_msg. This function is
 * mainly used to be able to make a synchronous reply waiting for the
 * message this function is going to sent. 
 *
 * Because in some cases the thread sending the message can run
 * faster, or simply the thread planner have choosen to give greeter
 * priority, than the thread actually does the wait, this function
 * allows to avoid this race condition.
 *
 * Application designer which are planning to call \ref vortex_channel_wait_reply
 * should use this function to send message.
 *
 * You can also read more about Wait Reply Method \ref wait_reply "here".
 * 
 * @param channel the channel where message will be sent.
 * @param message the message to be sent.
 * @param message_size the message size to be sent.
 * @param msg_no a required integer reference to store the message number used
 * @param wait_reply a Wait Reply object created using \ref vortex_channel_create_wait_reply 
 * 
 * @return the same as \ref vortex_channel_send_msg
 */
gboolean           vortex_channel_send_msg_and_wait               (VortexChannel   * channel,
								   gchar           * message,
								   gint              message_size,
								   gint            * msg_no,
								   WaitReplyData   * wait_reply)
{
	return __vortex_channel_send_msg_common (channel, message, message_size, msg_no, wait_reply);
}

/** 
 * @brief Printf-like version for the \ref vortex_channel_send_msg_and_wait function.
 *
 * This function works the same way \ref
 * vortex_channel_send_msg_and_wait does but supporting printf-like
 * message definition.
 * 
 * @param channel the channel where message will be sent.
 * @param msg_no a required integer reference to store message number used.
 * @param wait_reply a wait reply object created with \ref vortex_channel_create_wait_reply
 * @param format a printf-like string format defining the message to be sent.
 * 
 * @return 
 */
gboolean           vortex_channel_send_msg_and_waitv              (VortexChannel   * channel,
								   gint            * msg_no,
								   WaitReplyData   * wait_reply,
								   gchar           * format,
								   ...)
{
	gchar      * msg;
	gboolean     result;
	va_list      args;

	// initialize the args value
	va_start (args, format);
	// build the message
	msg = g_strdup_vprintf (format, args);
	// end args values
	va_end (args);

	// execute send_msg
	result = vortex_channel_send_msg_and_wait (channel, msg, strlen (msg), msg_no, wait_reply);
	
	// free value and return
	g_free (msg);
	return result;
}


/** 
 * @internal
 * @brief Common function to perform message replies.
 * 
 * @param channel the channel where the reply will be sent.
 * @param type the reply type for the message.
 * @param message the message included on the reply.
 * @param message_size the message reply size
 * @param msg_no_rpy the message number this function is going to reply.
 * 
 * @return TRUE if reply was sent, FALSE if not.
 */
gboolean  __vortex_channel_common_rpy (VortexChannel    * channel,
				       VortexFrameType    type,
				       gchar            * message,
				       gint               message_size,
				       gint               msg_no_rpy)
{
	VortexSequencerData * data;

	g_return_val_if_fail (channel,                           FALSE);
	g_return_val_if_fail (message,                           FALSE);
	g_return_val_if_fail (channel->is_opened,                FALSE);
	

	// lock send mutex
	// g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "lock send_mutex at common_rpy channel=%d, message:\n%s", 
	//       vortex_channel_get_number (channel),
	//       message);
	g_mutex_lock (channel->send_mutex);
	channel->being_sending = TRUE;

	// check we are going to reply the message is waiting to be replied
	while (vortex_channel_get_next_reply_no (channel) != msg_no_rpy) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		       "Received a rpy request for message %d while already waiting to reply to %d, blocking caller",
		       msg_no_rpy,
		       vortex_channel_get_next_reply_no (channel));
		g_cond_wait (channel->reply_cond, channel->send_mutex);
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sending reply for message %d (size: %d)\n", 
	       msg_no_rpy, message_size);
	
	// create data to be sent by the sequencer
	data                 = g_new0 (VortexSequencerData, 1);
	data->channel        = channel;
	data->type           = type;
	data->channel_num    = vortex_channel_get_number (channel);
	data->msg_no         = msg_no_rpy;
	data->first_seq_no   = vortex_channel_get_next_seq_no (channel);
	data->message_size   = message_size;
	data->message        = g_strdup (message);

	// update channel status: here goes a little exception to
	// channel update status.
	// 
	// On initial greetings message exchange both peers sends to
	// each other a RPY message using channel 0 and message 0. But
	// this RPY must be ignored as well as the message number used.
	// 
	// This is because all RPY messages have to reply a MSG sent
	// but, as I say, on initial greeting no MSG frame is sent.
	// 
	// For this particular case only update sequence number. Next
	// times also update RPY message number.
	if ((channel->last_seq_no == 0) && (channel->last_reply_sent == 0) && (channel->channel_num == 0))
		vortex_channel_update_status (channel, message_size, UPDATE_SEQ_NO);
	else
		vortex_channel_update_status (channel, message_size, UPDATE_SEQ_NO | UPDATE_RPY_NO);

	// send data to sequencer
	vortex_sequencer_queue_data (data);

	// unlock send mutex
	g_cond_broadcast (channel->reply_cond);

	// g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unlock send_mutex at common_rpy channel=%d", vortex_channel_get_number (channel));
	g_mutex_unlock   (channel->send_mutex);
	channel->being_sending = FALSE;	

	return TRUE;
}

/** 
 * @brief printf-like version for \ref vortex_channel_send_rpy function.
 *
 * This function is a wrapper to \ref vortex_channel_send_rpy but allowing
 * you to use a prinf-style message format definition. See also
 * vortex_channel_send_rpy.
 * 
 * @param channel the channel where the message will be sent.
 * @param msg_no_rpy the message number to reply over this channel.
 * @param format a printf-like string format to build the message reply to be sent.
 * 
 * @return TRUE if reply was sent, FALSE if not.
 */
gboolean        vortex_channel_send_rpyv       (VortexChannel *channel,
						gint    msg_no_rpy,
						gchar * format,
						...)
{
	gchar      * msg;
	gboolean     result;
	va_list      args;

	// initialize the args value
	va_start (args, format);
	// build the message
	msg = g_strdup_vprintf (format, args);
	// end args values
	va_end (args);

	// execute send_msg
	result = vortex_channel_send_rpy (channel, msg, strlen (msg), msg_no_rpy);
	
	// free value and return
	g_free (msg);
	return result;
}

/** 
 * @brief Allows to send a message reply using RPY type.
 *
 * Sends a RPY message to received message MSG identified by
 * <i>msg_no_rpy</i>. This functions allows you to reply a to a received MSG
 * in a way you only need to provide the <i>msg_no_rpy</i>.  According to
 * RFC3080: the BEEP Core, a message reply can not be done if there
 * are other message awaiting to be replied.
 *
 * This means calling to \ref vortex_channel_send_rpy trying to send a
 * reply having previous message been waiting for a reply yields to
 * block caller until previous replies are sent.
 *
 * These may seems to be an anoying behaviour but have some advantages
 * and can be also avoided easily.  First of all, think about the
 * following scennario: a vortex client (or beep enabled client) sends
 * over the same channel 3 request using 3 MSG frame type without
 * waiting to be replied.
 * 
 * Beep RPC 3080 section "2.6.1 Withing a Single Channel" says
 * listener role side have to reply to these 3 MSG in the order they
 * have come it they have come on the same channel. This ensure client
 * application that its request are sent in order to remote server but also
 * its reply are received in the same order. 
 *
 * This feature is a really strong one because application designer
 * don't need to pay attention how message replies are received no
 * mather how long each one have taken.
 *
 * As a side effect, it maybe happen you don't want to get hang until
 * reply are received just because it doesn't make sense for the
 * application to receive all replies in the same order msg were sent.
 * 
 * For this case client application must send all msg over differnt
 * channel so each MSG/RPY pair is independent from each other. This
 * can also be found on RFC 3080 section "2.6.2 Between Different
 * Channels"
 *
 * As a consequence:
 * <ul>
 *
 *  <li>Message sent over the same channel will receive its replies in
 *  the same order the message were sent. This have a <i>"serial behaviour"</i></li>
 *  
 *  <li>Message sent over different channels will receive replies as
 *  fast as the remote BEEP node replies to them. This have a <i>"parallel behaviour"</i></li>
 *
 * </ul>
 * 
 * 
 * @param channel the channel where  the message will be sent
 * @param message the message to sent
 * @param message_size the message size
 * @param msg_no_rpy the message number this function is going to reply to.
 * 
 * @return TRUE if message could be sent, FALSE if fail
 */
gboolean        vortex_channel_send_rpy        (VortexChannel *channel,  
						gchar * message,
						gint    message_size,
						gint    msg_no_rpy)
{
	return __vortex_channel_common_rpy (channel, VORTEX_FRAME_TYPE_RPY,
					    message, message_size, msg_no_rpy);
}

/** 
 * @brief Allows to reply a message using ERR message type.
 *
 * Sends a error reply to message received identified by
 * <i>msg_no_rpy</i>. This function does the same as \ref vortex_channel_send_rpy
 * but sending a ERR frame, so take a look to its documentation.
 *
 * @param channel the channel where error reply is going to be sent
 * @param message the message
 * @param message_size the message size
 * @param msg_no_rpy the message number to reply
 * 
 * @return TRUE if message was sent or FALSE if fails
 */
gboolean   vortex_channel_send_err        (VortexChannel *channel,  
					   gchar * message,
					   gint    message_size,
					   gint    msg_no_rpy)
{
	return __vortex_channel_common_rpy (channel, VORTEX_FRAME_TYPE_ERR,
					    message, message_size, msg_no_rpy);
}

/** 
 * @brief Printf-like version for \ref vortex_channel_send_err.
 *
 * Allows to send a ERR message type using a printf-like message format.
 * 
 * @param channel the channel where message reply will be sent.
 * @param msg_no_err the message number to reply to.
 * @param format a printf-like string format.
 * 
 * @return TRUE if message was sent of FALSE if fails
 */
gboolean        vortex_channel_send_errv       (VortexChannel * channel,
						gint    msg_no_err,
						gchar * format,
						...)
{
	gchar      * msg;
	gboolean     result;
	va_list      args;

	// initialize the args value
	va_start (args, format);
	// build the message
	msg = g_strdup_vprintf (format, args);
	// end args values
	va_end (args);

	// execute send_msg
	result = vortex_channel_send_err (channel, msg, strlen (msg), msg_no_err);
	
	// free value and return
	g_free (msg);
	return result;
}

/** 
 *
 * @brief Retuns next sequence number to be used. 
 *
 * Return the next sequence number to be used on this channel. This function must not be
 * confused with \ref vortex_channel_get_next_expected_seq_no. 
 *
 * This function is actually used to know what message seq number to
 * use on next frame to be sent. If you what to get next frame seq no
 * to expected to be received on this channel you must use \ref vortex_channel_get_next_expected_seq_no.
 * 
 * @param channel the channel to operate on.
 * 
 * @return the next seq_number to use, or -1 if fail
 */
guint32          vortex_channel_get_next_seq_no (VortexChannel * channel)
{
	g_return_val_if_fail (channel, -1);
	return channel->last_seq_no;
}

/** 
 * @brief Returns actual channel window size.
 *
 * Returns actual channel window size. This parameter is used mainly
 * by the vortex sequencer so that process can split message received
 * into several frames according to that configuration.
 *
 * @param channel the channel to operate on
 * 
 * @return the window size or -1 if fails
 */
gint            vortex_channel_get_window_size (VortexChannel * channel)
{
	g_return_val_if_fail (channel, -1);
	
	return channel->window_size;
}

/** 
 * @internal
 * @brief Queue a new frame to be sent by the Vortex Writer.
 *
 * This function is for vortex library internals. This function is not
 * useful to vortex consumer.  This function allows vortex sequencer
 * process to queue frames to be sent on this channel.  Later the
 * vortex writer will take all queue message for each channel on each
 * connection a sent them in a round robbin manner.
 * 
 * @param channel the channel where frame will be queued
 * @param data the frame to be queued.
 */
void            vortex_channel_queue_frame     (VortexChannel * channel, VortexWriterData * data)
{
	g_return_if_fail (channel);
	g_return_if_fail (channel->queue_msgs);
	g_return_if_fail (data);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "pushing a new frame for channel=%d", 
	       channel->channel_num);

	// queue a frame to be sent by vortex writer
	vortex_queue_push (channel->queue_msgs, data);

	return;
}

/** 
 * @internal
 * @brief Returns true is actual frame queue is empty or not
 * 
 * @param channel the channel to check
 * 
 * @return TRUE if frame queue is empty, otherwise FALSE.
 */
gboolean    vortex_channel_queue_is_empty  (VortexChannel * channel)
{
	g_return_val_if_fail (channel, FALSE);

	return vortex_queue_is_empty (channel->queue_msgs);
}

/** 
 * @internal
 * @brief Return the next queued frame to be sent for the given channel.
 * 
 * Get the next frame to be sent over this channel. A description of
 * the result returned by this function can be found at \ref vortex_types
 *
 * This function is for internal vortex purposes so it should not be
 * useful to vortex library consumers.
 * 
 * 
 * @param channel the channel to operate on.
 * 
 * @return the frame already prepare to be sent of NULL if fail
 */
VortexWriterData    * vortex_channel_queue_next_msg (VortexChannel * channel)
{

	g_return_val_if_fail (channel, NULL);
	g_return_val_if_fail (! vortex_queue_is_empty (channel->queue_msgs), NULL);

	return vortex_queue_pop (channel->queue_msgs);

}

/** 
 * @internal
 *
 * @brief Returns the current status of pending frames to be sent for
 * the given channel.
 *
 * This function will return how many frames are pending to be sent
 * for the given channel.
 * 
 * @param channel the channel where messages pending will be counted.
 * 
 * @return pending message for the given channel
 */
gint               vortex_channel_queue_length                    (VortexChannel * channel)
{
	g_return_val_if_fail (channel, 0);
	return vortex_queue_get_length (channel->queue_msgs);
}

/** 
 * @brief Allows to store a pair key/value into the channel
 * 
 * This allows application designers to use the channel as a way to
 * transport data among function or handler execution.
 *
 * To remove store item you will have to store again the key used
 * passing in a NULL value. Example:
 * \code
 *          vortex_channel_set_data (my_channel, "my_key", NULL);
 *          // previous will remove "my_key" pair from the channel
 * \endcode
 *
 * @param channel channel where data will be stored.
 * @param key the key index to look up the data store
 * @param value the data to be stored.
 */
void               vortex_channel_set_data                        (VortexChannel * channel,
								   gpointer key,
								   gpointer value)
{
	g_return_if_fail (channel);
	g_return_if_fail (key);

	if (value == NULL) {
		vortex_hash_remove (channel->data, key);
		return;
	}

	vortex_hash_replace (channel->data, key, value);
	return;
}

/** 
 * @brief Returns the value indexed by the given key inside the given
 * channel.
 * 
 * @param channel the channel where data will be looked up.
 * @param key the index key to use.
 * 
 * @return the value or NULL if fails. 
 */
gpointer           vortex_channel_get_data                        (VortexChannel * channel,
								   gpointer key)
{
	g_return_val_if_fail (channel, NULL);
	g_return_val_if_fail (key,     NULL);

	return vortex_hash_lookup (channel->data, key);
	
}

/** 
 * @brief Returns actual reply number to be used.
 * 
 * This reply number must be used to now if a message reply can be
 * done. This represent the first message identifier to be replied.
 * 
 * @param channel the channel to query
 * 
 * @return the next reply number to use or -1 if fails
 */
gint            vortex_channel_get_next_reply_no          (VortexChannel * channel)
{
	g_return_val_if_fail (channel, -1);
	
	return channel->last_reply_sent;
}

/**
 * @brief Returns actual reply number expected to be recevied from
 * remote peer.
 * 
 * @param channel the channel to query
 * 
 * @return the reply number expected or -1 if fails
 */
gint            vortex_channel_get_next_expected_reply_no (VortexChannel * channel)
{
	g_return_val_if_fail (channel, -1);

	return channel->last_reply_expected;
}

/** 
 * @brief Returns actual seq no expected to be received on this channel. 
 *
 * This function is used to check incoming message seq no.
 * 
 * @param channel the channel to operate on.
 * 
 * @return -1 if fails or expected seq no for this channel.
 */
guint32        vortex_channel_get_next_expected_seq_no (VortexChannel * channel)
{
	g_return_val_if_fail (channel, -1);
	
	return channel->last_seq_no_expected;
}

typedef struct {
	VortexChannel              * channel;
	VortexOnClosedNotification   on_closed;
	gboolean                     threaded;
} VortexChannelCloseData;


/** 
 * @internal
 * @brief Validates err message received from remote peer for close channel request sent.
 * 
 * @param channel 
 * @param frame 
 * @param code 
 * @param msg 
 */
void __vortex_channel_close_validate_err (VortexChannel * channel, VortexFrame * frame, 
					  gchar ** code, gchar **msg)
{
	xmlDtdPtr       channel_dtd;
	xmlValidCtxt    context;
	xmlDocPtr       doc;
	xmlNodePtr      cursor;

	// get channel management DTD
	if ((channel_dtd = vortex_dtds_get_channel_dtd ()) == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to load dtd file, cannot validate incoming message, returning error frame");
		return;
	}

	// parser xml document
	doc = xmlParseMemory ((xmlChar *) vortex_frame_get_payload (frame), 
			      vortex_frame_get_payload_size (frame));  // * sizeof (char)));
	if (!doc) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to parse err reply, we have a buggy remote peer");
		return;
	}

	// validate de document
	if (!xmlValidateDtd (&context, doc, channel_dtd)) {
		// Validation failed
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "validation failed for err reply, we have a buggy remote peer");
		xmlFreeDoc (doc);
		return;		 
	}
	
	// get a reference to document root (the error)
	cursor  = xmlDocGetRootElement (doc);
	if (xmlStrcmp (cursor->name, (const xmlChar *) "error")) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "not found error element on err reply, we have a buggy remote peer");
		xmlFreeDoc (doc);
		return;
	}
	
	// get error code returned
	(* code ) = xmlGetProp (cursor, "code");

	// get the message value returned
	(* msg  ) = g_strdup (cursor->content);

	// free not needed data
	xmlFreeDoc (doc);
	return;
}

/** 
 * @internal
 * @brief Validates rpy message received from remote peer for close
 * channel request sent.
 * 
 * @param channel 
 * @param frame 
 */
void __vortex_channel_close_validate_rpy (VortexChannel * channel, 
					  VortexFrame   * frame)
{
	
	xmlDtdPtr       channel_dtd;
	xmlValidCtxt    context;
	xmlDocPtr       doc;
	xmlNodePtr      cursor;

	// get channel management DTD
	if ((channel_dtd = vortex_dtds_get_channel_dtd ()) == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to load dtd file, cannot validate incoming message, returning error frame");
		return;
	}

	// parser xml document
	doc = xmlParseMemory ((xmlChar *) vortex_frame_get_payload (frame), 
			      vortex_frame_get_payload_size (frame)); // * sizeof (char)));
	if (!doc) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to parse rpy reply, we have a buggy remote peer");
		return;
	}

	// validate de document
	if (!xmlValidateDtd (&context, doc, channel_dtd)) {
		// Validation failed
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "validation failed for rpy reply, we have a buggy remote peer");
		xmlFreeDoc (doc);
		return;		 
	}
	
	// get a reference to document root (the error)
	cursor  = xmlDocGetRootElement (doc);
	if (xmlStrcmp (cursor->name, (const xmlChar *) "ok")) 
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "not found ok element on rpy reply, we have a buggy remote peer");
	
	// free not needed data
	xmlFreeDoc (doc);
	return;
}

/** 
 * @internal
 * @brief Blocks a channel until all replies to message sent are
 * received.
 *
 * This function is synchronized with vortex reader so that process
 * notifies when a reply have been received.
 *
 * This function uses a mutex (close_mutex) and a conditional var
 * (close_cond) that are signaled from
 * vortex_channel_signal_on_close_blocked.
 * 
 * 
 * @param channel the channel to operate on.
 */
void __vortex_channel_block_until_replies_are_received (VortexChannel * channel)
{
	
	g_return_if_fail (channel);

	channel->being_closed = TRUE;
	g_mutex_lock (channel->close_mutex);

	while (! vortex_channel_is_ready (channel)) {

		channel->waiting_replies = TRUE;
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		       "we still didn't receive all replies for connection=%d channel=%d, block until all replies are received (MSG %d != RPY %d)",
		       vortex_connection_get_id (channel->connection),
		       channel->channel_num,
		       channel->last_message_sent, channel->last_reply_expected - 1);
		
		g_cond_wait (channel->close_cond, channel->close_mutex);
	}
	g_mutex_unlock (channel->close_mutex);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       "we have received the last reply we can now close the channel %d (MSG %d = RPY %d)", 
	       channel->channel_num, channel->last_message_sent, channel->last_reply_expected - 1);

	return;
}



/** 
 * @internal
 * @brief Blocks a channel until all replies to message receive are
 * sent.
 *
 * This function is used by __vortex_channel_close to ensure we send
 * all replies awaited by the remote peer until getting blocked for
 * the ok message.
 *
 * 
 * @param channel the channel to block
 */
void __vortex_channel_block_until_replies_are_sent (VortexChannel * channel)
{
	g_return_if_fail (channel);

	g_mutex_lock (channel->pending_mutex);

	while (channel->last_message_received != (channel->last_reply_sent - 1)) {
		
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, 
		       "we still didn't sent replies for connection=%d channel=%d, block until all replies are sent (RPY %d != MSG %d)",
		       vortex_connection_get_id (channel->connection),
		       channel->channel_num,
		       (channel->last_reply_sent - 1), channel->last_message_received);
		
		g_cond_wait (channel->pending_cond, channel->pending_mutex);
	}
	g_mutex_unlock (channel->pending_mutex);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       "we have sent the last reply we can now close the channel %d (RPY %d = MSG %d)", 
	       channel->channel_num, 
	       (channel->last_reply_sent - 1), 
	       channel->last_message_received);
}

gpointer __vortex_channel_close (VortexChannelCloseData * data)
{
	VortexChannel              * channel;
	VortexChannel              * channel0;
	VortexFrame                * frame;
	VortexConnection           * connection;
	WaitReplyData              * wait_reply;
	VortexOnClosedNotification   on_closed;
	// TRUE channel is closed, FALSE not.
	gboolean                     result      = FALSE;
	gboolean                     threaded;
	gchar                      * code        = NULL;
	gchar                      * msg         = NULL;
	gint                         msg_no      = -1;
	
	
	g_return_val_if_fail (data, NULL);

	// get a reference to received data (comes from vortex_channel_close)
	channel    = data->channel;
	on_closed  = data->on_closed;
	threaded   = data->threaded;

	// free data
	g_free (data);

	// According to section 2.3.1.3 "The close message" from RFC 3080
	// 
	// We are going to close a channel, so the process is as follows:

	// 1) Send a close channel msg indication to remote peer.
	// But, if there are pending message replies to be received,
	// wait for them until send close message.

	// 2) Once close message have been sent, we have to avoid
	// sending futher message and only be prepared to still
	// receive more msg from remote peer. Of course we are allowed
	// to (and we must) reply to this received MSG frame type.

	// 3) Once received, we have to check there are not pending
	// RPY to be sent and then close the channel.

	// 4) At the same time we keep on waiting remote peer response
	// to close indication.

	// step 1)
	// ~~~~~~~
	connection = channel->connection;
	if (!vortex_connection_is_ok (connection, FALSE)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		       "trying to close a channel over a terminaed session, this is a bug on client side code, freeing vortex channel resources");

		result = TRUE;
		goto __vortex_channel_close_invoke_caller;
	}

	// before sending close message we have to ensure all replies
	// to message sent have been received if not, we can't send
	// the close message (2.3.1.3 The Close Message BEEP Core)
	__vortex_channel_block_until_replies_are_received (channel);

	// get a reference to administrative channel
	channel0 = vortex_connection_get_channel (connection, 0);

	// send the close channel message
	wait_reply = vortex_channel_create_wait_reply ();
	if (!vortex_channel_send_msg_and_waitv (channel0, &msg_no, wait_reply, _channel_close_msg, 
						channel->channel_num, "200")) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to send close channel message");
		result = FALSE;
		vortex_channel_free_wait_reply (wait_reply);
		goto __vortex_channel_close_invoke_caller;
	}

	// step 2)
	// ~~~~~~~
	// This step is actually been fulfilled due to being_closed
	// attribute. This allows to keep on sending rpy to incoming
	// msg but not to send more msg.

	// step 3)
	// ~~~~~~~
	// ensure we have sent all message replies to remote peer MSG
	__vortex_channel_block_until_replies_are_sent (channel);

	// step 4)
	// ~~~~~~~
	if (channel->channel_num == 0) {
		// flag de connection to be at close processing. We
		// set this flag so vortex reader won't complain about
		// not properly connection close.
		// To close the channel 0 means to close the beep session
		vortex_connection_set_data (connection, "being_closed", 
					    GINT_TO_POINTER (TRUE));
	}
	
	frame = vortex_channel_wait_reply (channel0, msg_no, wait_reply);
	switch (vortex_frame_get_type (frame)) {
	case VORTEX_FRAME_TYPE_ERR:
		// remote peer do not agree to close the channel, get
		// values returned
		__vortex_channel_close_validate_err (channel, frame, &code, &msg);
		result = FALSE;
		break;
	case VORTEX_FRAME_TYPE_RPY:
		// remote peer agree to close the channel
		__vortex_channel_close_validate_rpy (channel, frame);
		result = TRUE;
		break;
	default:
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		       "unexpected reply found for a close channel indication, remote peer have a buggy implementation");
		__vortex_connection_set_not_connected (connection, 
						       "unexpected reply found for a close channel indication, remote peer have a buggy implementation");
		result = TRUE;
		goto __vortex_channel_close_invoke_caller;
	}
	// now the frame have been sucessfully delivered free the frame
	vortex_frame_free (frame);
	
	// set channel to be closed. It result = TRUE channel will be
	// marked as closed so vortex reader can check if a message
	// can be actually delivered
	channel->is_opened = !result;

 __vortex_channel_close_invoke_caller:
	if (threaded) {
		// invoke handler
		on_closed (channel->channel_num, result, code, msg);
		// once the handler have been invoke, we can free its
		// resources
	}

	// remove the channel from the connection
	if (!result && (channel->channel_num == 0)) {
		// unflag the connection to be at close processing. We
		// unset this flag because the connection were not closed
		vortex_connection_set_data (connection, "being_closed", NULL);
	}

	if (result) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		       "because channel=%d over connection id=%d have been accepted to be removed, remove it from its session",
		       channel->channel_num,
		       vortex_connection_get_id (connection));
		vortex_connection_remove_channel (connection, channel);
		// actually, vortex_connection_remove_channel already
		// does a vortex_channel_free
	}
	
	// return the value
	if (threaded)
		return NULL; // on threaded mode allways returns NULL
	return result ? GINT_TO_POINTER (result) : NULL;
}

/** 
 * @brief Close the given channel. 
 *
 * Free all resource allocated by channel and tries to close it if is
 * opened.  Once you have called this function, the channel will no
 * longer be available until it is created again. Once the channel is
 * closed, this function will notify vortex session (the connection
 * associated with this channel) to remove this channel reference from
 * it.
 *
 * This function by default is blocking. This is because the channel
 * close process may be denied from other side so channel close can be
 * done. This may happen if other side have something remaining to be
 * sent while you are trying to close. This means the function
 * will block you until channel close process reply is receive and
 * then returned to you.
 *
 * Of course you can avoid get blocked on channel close by setting
 * the <i>on_close_notification</i> which is called at the end of the process
 * notifiying if channel was closed or not.
 * 
 * On both models if channel is closed, channel resources will be
 * freed so channel pointer provided will no longer pointing to a valid
 * channel.
 * 
 * Another thing about channel close process to keep in mind it that
 * close handler defined at \ref vortex_channel_new or at
 * \ref vortex_profiles_register is NOT executed when \ref vortex_channel_close
 * is called.  Those handlers are actually executed when your channel
 * receive a close indication. This also means this function sends a
 * close indication to remote peer so, if remote peer is also a vortex
 * enabled and have a close handler defined, then it will be executed.
 * 
 * There are an exception to keep in mind. Channel 0 can not be closed
 * using this function. This channel is hard wired to actual session
 * so, there is no reason to support closing channel 0. Channel 0 is
 * closed on session close. If you still want to close channel 0 call
 * \ref vortex_connection_close instead.
 * 
 * During the <i>on_closed</i> handler execution, if defined, you can still
 * get access to the channel being closed even if actually is closed. This
 * is provided because you may be interesting on getting the channel status, etc.
 * After <i>on_closed</i> is executed and finished, the channel resources are freed.
 * 
 * @param channel the channel to free
 * @param on_closed a optional handler notification
 * 
 * @return TRUE if the channel was closed or FALSE if not. On
 * threaded mode, activated by defining the <i>on_closed</i> handler,
 * allways return TRUE. Actual close process result is notified on
 * <i>on_closed</i>
 */
gboolean vortex_channel_close (VortexChannel * channel, 
			       VortexOnClosedNotification on_closed)
{
	
	VortexChannelCloseData * data;
	
	g_return_val_if_fail (channel,                   FALSE);
	g_return_val_if_fail (channel->channel_num >= 0, FALSE);
	// check the channel have not received anothre close petition
	// from another thread or is being close due to a remote peer petition
	g_return_val_if_fail (! channel->being_closed,   FALSE);

	// set this channel to be in closed channel process
	channel->being_closed = TRUE;
	
	// creates data to be passed in to __vortex_channel_close 
	data            = g_new0 (VortexChannelCloseData, 1);
	data->channel   = channel;
	data->on_closed = on_closed;
	data->threaded  = (on_closed != NULL);

	// launch threaded mode
	if (data->threaded) {
		vortex_thread_pool_new_task ((GThreadFunc) __vortex_channel_close, data);
		return TRUE;
	}

	// launch blocking mode
	return (__vortex_channel_close (data) != NULL);
}


/** 
 * @brief Returns the actual channel profile. 
 * 
 * You must not free returned value.
 * 
 * @param channel the channel to operate on
 * 
 * @return the profile the channel have or NULL if fails
 */
gchar         * vortex_channel_get_profile (VortexChannel * channel)
{
	g_return_val_if_fail (channel, NULL);

	return channel->profile;
}

/** 
 * @brief Returns actual channel session (or Vortex Connection). 
 * 
 * @param channel the channel to operate on.
 * 
 * @return the connetion where channel was created or NULL if
 * fails.
 */
VortexConnection * vortex_channel_get_connection               (VortexChannel * channel)
{
	g_return_val_if_fail (channel, NULL);

	return channel->connection;
}

/** 
 * @brief Return whenever the received handler have been defined for this
 * channel.
 * 
 * @param channel the channe to check
 * 
 * @return If received handler have been defined returns TRUE if
 * not returns FALSE.
 */
gboolean        vortex_channel_is_defined_received_handler (VortexChannel * channel)
{
	g_return_val_if_fail (channel, FALSE);
	return (channel->received != NULL);
}


typedef struct _ReceivedInvokeData {
	VortexChannel * channel;
	VortexFrame   * frame;
	gint            channel_num;
}ReceivedInvokeData;

gpointer __vortex_channel_invoke_received_handler (ReceivedInvokeData * data)
{
	VortexChannel    * channel      = data->channel;
	VortexConnection * connection   = vortex_channel_get_connection (channel);
	VortexFrame      * frame        = data->frame;
	gchar            * raw_frame    = NULL;
	gboolean           is_connected;

	// get a reference to channel number so we can check after
	// frame received handler if the channel have been closed.
	// Once the frame received have finished this will help us to
	// know if application space have issued a close channel.
	gint              channel_num  = data->channel_num;
	
 	g_return_val_if_fail (data,             NULL);
	g_return_val_if_fail (data->channel,    NULL);
	g_return_val_if_fail (data->frame,      NULL);
	g_return_val_if_fail (channel_num >= 0, NULL);

	// show received frame
	if (vortex_log_is_enabled ()) {
		raw_frame = vortex_frame_get_raw_frame (frame);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		       "second level frame receive invocation for connection=%d channel=%d channel_received=%d (seqno: %d size: %d)\n%s",
		       vortex_connection_get_id (connection),
		       channel_num, channel->channel_num, 
		       vortex_frame_get_seqno (frame),
		       vortex_frame_get_payload_size (frame),
		       raw_frame);
		g_free (raw_frame);
	}

	// record actual connection state
	is_connected = vortex_connection_is_ok (connection, FALSE);
	if (! is_connected) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "invoking frame receive on a non-connected session");
		goto free_resources;
	}
	// increase connection reference counting to avoid reference loosing 
	// if connection is closed
	vortex_connection_ref (connection, "second level invocation (frame received)");

	// invoke handler
	if (channel->received) {
		channel->received (channel, channel->connection, frame, 
				   channel->received_user_data);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "frame received invocation for second level finished");
	}else {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "invoking frame received on channel %d with not handler defined",
		       vortex_channel_get_number (channel));
	}

	// before frame receive handler we have to check if client have closed its connection
	is_connected = vortex_connection_is_ok (connection, FALSE);

	if (! is_connected ) {
		// the connection have been closed inside frame receive
		goto free_resources;
	}

	// The function __vortex_channel_0_frame_received_close_msg
	// can be blocked awaiting to receive all replies
	// expected. The following signal tries to wake up a
	// posible thread blocked until last_reply_expected change.
	if (vortex_connection_channel_exists (connection, channel_num))
		vortex_channel_signal_on_close_blocked (channel);
	else
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel=%d was closed during frame receive execution, unable to signal on close",
		       channel_num);

	// log a message
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       "invocation frame received handler for channel %d finished (second level: channel)", channel_num);

	// free data and frame
 free_resources:

	// decrease connection reference counting
	vortex_connection_unref (connection, "second level handler (frame received)");

	vortex_frame_free (frame);
	g_free            (data);

	return NULL;
}



/** 
 * @internal
 * @brief Invokes received handler on this channel. 
 * 
 * This handler is mainly used by vortex reader process at function
 * vortex_reader_process_socket.
 *
 * This enable vortex reader to notify a frame have been received on a
 * particular channel. This function also check if there are waiting
 * thread blocked at vortex_channel_wait_reply. This means there are
 * threads waiting to a RPY frame type or ERR frame type to be
 * received.
 *
 * If no wait reply is found for a particular RPY or err, then the
 * default channel frame received handler is invoked.
 *
 * Frame comes to this function in a ordered manner. Vortex reader
 * thread filter all message received on registered connection to
 * detect channel synchronization failures. So, MSG recevied are
 * already ordered when they comes to this function and RPY, ERR
 * frames are also ordered.
 *
 * @param channel the channel where frame recevied invocation will happen
 * @param frame the frame actually producing the invocation
 * 
 * @return TRUE if frame was delivered or FALSE if not.
 */
gboolean      vortex_channel_invoke_received_handler (VortexChannel * channel, VortexFrame * frame)
{
	ReceivedInvokeData * data;
	WaitReplyData      * wait_reply;
	
	// check data to avoid self-encrintation ;-)
	g_return_val_if_fail (channel, FALSE);
	g_return_val_if_fail (frame,   FALSE);

	// query if we have a waiting queue
	switch (vortex_frame_get_type (frame)) {
	case VORTEX_FRAME_TYPE_RPY:
	case VORTEX_FRAME_TYPE_ERR:
		// lock the channel during channel operation
		g_mutex_lock (channel->receive_mutex);

		// we have a reply message: rpy or err. this means we can check if there are 
		// waiting thread
		if ((wait_reply = vortex_queue_peek (channel->waiting_msgno)) == NULL) {
			g_mutex_unlock (channel->receive_mutex);
			break;
		}
		// it seems we have waiting threads, so check if received frame
		// have a msgno for the next waiting queue
		if (vortex_frame_get_msgno (frame) != wait_reply->msg_no_reply) {
			g_mutex_unlock (channel->receive_mutex);
			break;
		}
		// ok, we have found there are a thread waiting, so push data
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "found a frame that were waited by a thread: reply no='%d' over channel=%d",
		       vortex_frame_get_msgno (frame),
		       channel->channel_num);

		// remove waiting reply data
		vortex_queue_pop (channel->waiting_msgno);

		// queue frame received
		g_async_queue_push (wait_reply->queue, frame);

		// unlock the channel
		g_mutex_unlock (channel->receive_mutex);

		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "frame push and receive (channel=%d) mutex unlocked",
		       channel->channel_num);

		return TRUE;
	default:
		// do nothing
		break;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "no waiting queue for this message: %d", 
	       vortex_frame_get_msgno (frame));
	
	// no waiting queue, invoke frame received handler so create the thread
	// on this newly created thread, the frame will be deleted
	if (channel->received == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "it seems you didn't define a second level received handler");
		return FALSE;
	}

	// prepare data to be passed in to thread
	data              = g_new0 (ReceivedInvokeData, 1);
	data->channel     = channel;
	data->frame       = frame;
	data->channel_num = channel->channel_num;
	
	// create the thread to invoke frame received handler
	vortex_thread_pool_new_task ((GThreadFunc)__vortex_channel_invoke_received_handler, data);
	return TRUE;

}

/** 
 * @brief Returns if the given channel have defined its close handler.
 * 
 * @param channel the channel to operate on
 * 
 * @return TRUE if close handler is defined or FALSE if not.
 */
gboolean        vortex_channel_is_defined_close_handler (VortexChannel * channel)
{
	g_return_val_if_fail (channel, FALSE);
	
	return (channel->close != NULL);
}

/** 
 * @internal
 * @brief Invoke the close handler for the given channel if were defined. 
 * 
 * @param channel the channel to operate on
 * 
 * @return what have returned close handler invocation
 */
gboolean            vortex_channel_invoke_close_handler     (VortexChannel  * channel)
{
	g_return_val_if_fail (channel,        FALSE);
	g_return_val_if_fail (channel->close, FALSE);

	return channel->close (channel->channel_num, channel->connection, 
			       channel->close_user_data);
}

/** 
 * @internal
 * @brief Internal Vortex Library function to validate received frames on channel 0.
 * 
 * @param channel0 the channel 0 itself
 * @param frame the frame received
 * 
 * @return TRUE if validated was ok or FALSE if it fails.
 */
gboolean __vortex_channel_0_frame_received_validate (VortexChannel * channel0, VortexFrame * frame)
{
	xmlDtdPtr       channel_dtd;
	xmlValidCtxt    context;
	xmlDocPtr       doc;

	// get channel management DTD
	if ((channel_dtd = vortex_dtds_get_channel_dtd ()) == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to load dtd file, cannot validate incoming message, returning error frame");
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "421", "service not available, unable to load dtd file, cannot validate incoming message");
		return FALSE;
	}

	// parser xml document
	doc = xmlParseMemory ((xmlChar *) vortex_frame_get_payload (frame), 
			      vortex_frame_get_payload_size (frame)); // * sizeof (char)));
	if (!doc) {
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "500", "general syntax error: xml parse error");
		__vortex_connection_set_not_connected (vortex_channel_get_connection (channel0),
						       "general syntax error: xml parse error");
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "received message on channel 0 with xml parse error, closing connection");
		return FALSE;		 
	}
	
	// validate document
	if (!xmlValidateDtd (&context, doc, channel_dtd)) {
		// Validation failed
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "501", "syntax error in parameters: non-valid XML");
		__vortex_connection_set_not_connected (vortex_channel_get_connection (channel0),
						       "501 syntax error in parameters: non-valid XML");
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "received message on channel 0 with syntax error in parameters: non-valid XML");
		xmlFreeDoc (doc);
		return FALSE;		 
	}
	xmlFreeDoc (doc);
	return TRUE;
}

enum {START_MSG, CLOSE_MSG, ERROR_MSG, OK_MSG};


/** 
 * @internal
 * @brief Identifies the frame type received over channel 0.
 * 
 * @param channel0 the channel 0 itself
 * @param frame the frame received
 * 
 * @return the frame type received.
 */
gint __vortex_channel_0_frame_received_identify_type (VortexChannel * channel0, VortexFrame * frame)
{
	xmlDocPtr       doc;
	gchar         * error_msg;
	xmlNodePtr      cursor;
	gint            result = -1;

	// parser xml document
	doc = xmlParseMemory ((xmlChar *) vortex_frame_get_payload (frame), 
			      vortex_frame_get_payload_size (frame)); // * sizeof (char)));
	if (!doc) {
		error_msg = g_strdup_printf (_error_msg, "500", "general syntax error: xml parse error");
		vortex_channel_send_err (channel0, error_msg, strlen (error_msg),
					 vortex_frame_get_msgno (frame));
		g_free (error_msg);
		return FALSE;		 
	}


	// Get the root element ( ? element)
	cursor = xmlDocGetRootElement (doc);

	if (!xmlStrcmp (cursor->name, (const xmlChar *) "start"))
		result = START_MSG;
	if (!xmlStrcmp (cursor->name, (const xmlChar *) "close"))
		result = CLOSE_MSG;
	if (!xmlStrcmp (cursor->name, (const xmlChar *) "error"))
		result = ERROR_MSG;
	if (!xmlStrcmp (cursor->name, (const xmlChar *) "ok"))
		result = OK_MSG;

	// return valud found
	xmlFreeDoc (doc);
	return result;
}


/** 
 * @internal
 * 
 * @param frame 
 * @param channel_num 
 * @param profile 
 * 
 * @return 
 */
gboolean __vortex_channel_0_frame_received_get_start_param (VortexFrame * frame,
							    gint * channel_num,
							    gchar ** profile)
{
	xmlDocPtr     doc;
	xmlNodePtr    cursor;
	gchar       * channel;

	// parser xml document
	doc = xmlParseMemory ((xmlChar *) vortex_frame_get_payload (frame), 
			      vortex_frame_get_payload_size (frame)); // * sizeof (char)));
	if (!doc) 
		return FALSE;

	// Get the root element (start element)
	cursor          = xmlDocGetRootElement (doc);

	channel         = xmlGetProp (cursor, "number");
	(* channel_num) = atoi (channel);
	g_free (channel);
	
	// Get the position of the first profile element
	cursor = cursor->children;
	while (cursor) {
		// next 
		cursor = cursor->next;

		// get profiles
		if (!xmlStrcmp (cursor->name, (const xmlChar *) "profile")) {		
			(* profile) =  xmlGetProp (cursor, "uri");
			xmlFreeDoc (doc);
			return TRUE;
		}
	}

	return FALSE;
}

/** 
 * @internal
 * 
 * @param channel0 
 * @param frame 
 */
void __vortex_channel_0_frame_received_start_msg (VortexChannel * channel0, VortexFrame * frame)
{
	gint               channel_num  = -1;
	gchar            * profile      = NULL;
	VortexChannel    * new_channel  = NULL;
	VortexConnection * connection   = channel0->connection;
	
	// get and check channel num and profile
	if (!__vortex_channel_0_frame_received_get_start_param (frame, &channel_num, &profile)) {
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "500", "unable to get channel start param");
		return;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "start message received: channel='%d' profile='%s'", 
	       channel_num, profile);

	// check if channel exists
	if (vortex_connection_channel_exists (connection, channel_num)) {
		g_free (profile);
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "554", "transaction failed: channel requested already exists");
		return;
	}

	// check if profile already exists
	if (!vortex_profiles_is_registered (profile)) {
		g_free (profile);
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "554", "transaction failed: channel profile requested not supported");
		return;
	}

	// ask if we have an start handler defined
	if (!vortex_profiles_is_defined_start (profile)) {
		g_free (profile);
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "421", "service not available: channel can not be created, no start handler defined");
		return;
	}

	// ask if channel can be created 
	if (!vortex_profiles_invoke_start (profile, channel_num, connection)) {
		g_free (profile);
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "421", "service not available: channel can not be created, unable to start");
		return;
	}

	// so, anyone have something to say about this channel creation?, no?, then create the channel
	new_channel = vortex_channel_empty_new (channel_num, profile, connection);
	vortex_connection_add_channel (connection, new_channel);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel %d with profile %s sucessfully created", channel_num, profile);

	// send rpy and then create the channel
	if (!vortex_channel_send_rpyv (channel0, 
				       vortex_frame_get_msgno (frame),
				       _channel_start_rpy_msg, profile)) {
		g_free (profile);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "an error have happen while sending start channel reply, closing channel");
		return;
	}
	g_free (profile);	       

	return;
}


/** 
 * @internal
 * 
 * @param frame 
 * @param channel_num 
 * @param code 
 * 
 * @return 
 */
gboolean __vortex_channel_0_frame_received_get_close_param (VortexFrame * frame,
							    gint * channel_num,
							    gchar ** code)
{
	xmlDocPtr     doc;
	xmlNodePtr    cursor;
	gchar       * channel;

	// parser xml document
	doc = xmlParseMemory ((xmlChar *) vortex_frame_get_payload (frame), 
			      vortex_frame_get_payload_size (frame)); // * sizeof (char)));
	if (!doc) 
		return FALSE;

	// Get the root element (close element)
	cursor          = xmlDocGetRootElement (doc);

	channel         = xmlGetProp (cursor, "number");
	(* channel_num) = atoi (channel);
	g_free (channel);

	(* code )       = xmlGetProp (cursor, "code");
	
	
	xmlFreeDoc (doc);

	return TRUE;
}


/** 
 * @internal
 * 
 * @param channel0 
 * @param frame 
 */
void __vortex_channel_0_frame_received_close_msg (VortexChannel * channel0, 
						  VortexFrame   * frame)
{
	
	gint               channel_num     = -1;
	gchar            * code            = NULL;
	gchar            * channel_num_str = NULL;
	VortexChannel    * channel;
	VortexConnection * connection      = vortex_channel_get_connection (channel0);
	gboolean           close_value     = FALSE;
	
	// get and check channel num and profile
	if (!__vortex_channel_0_frame_received_get_close_param (frame, &channel_num, &code)) {
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "500", "unable to get channel close param");
		return;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "close message received: channel='%d' code='%s'", 
	       channel_num, code);

	// code actually not used
	g_free (code);

	// check if channel exists
	if (!vortex_connection_channel_exists (connection, channel_num)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "received a request to close a channel=%d which doesn't exist",
		       channel_num);
		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "554", "transaction failed: channel requested to close doesn't exists");
		return;
	}
	
	// get channel to remove
	channel = vortex_connection_get_channel (connection, channel_num);

	// ask second level handler 
	if (vortex_channel_is_defined_close_handler (channel)) {
		close_value = vortex_channel_invoke_close_handler (channel);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "close second level handler response=%d", close_value);
		goto end;
	}

	// ask first level handler 
	if (vortex_profiles_is_defined_close (channel->profile)) {
		close_value = vortex_profiles_invoke_close (channel->profile, 
							    channel->channel_num, 
							    channel->connection);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "close first level handler response=%d", close_value);
		goto end;
	}

	// if channel0 have no defined its close handler we asume close=TRUE
	if (channel->channel_num == 0) 
		close_value = TRUE;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "no close handler defined for any level, asumming=%d", close_value);
 end:
	// now we have executed close handler for all levels and we
	// know what to do we need to follow the next indications:
	// 
	// 1) If close_value is false, simple reply an error message
	// to close avoiding to get blocked until all replies have 
	// been received.
	// 
	// 2) If close_value is true, we have to wait to all replies
	// to be received and tag the channel as being closed because
	// the application have accepted its closing. This is equal to
	// call vortex_channel_close. The channel gets into a state
	// where no new MSG can be sent and only replies will be

	// check 1)
	// ~~~~~~~~
	// flag the channel to be usable again
	channel->being_closed = close_value;
	if (close_value == FALSE) {

		vortex_channel_send_errv (channel0, vortex_frame_get_msgno (frame),
					  _error_msg, "550", "still working");
		return;
	}

	// check 2)
	// ~~~~~~~~
	// Ok, close we have to make sure have already receive all
	// replies for all message, that is, to be equal channel MSG
	// sent to RPY or ERR received
	__vortex_channel_block_until_replies_are_received (channel);
	
	// So, anyone have something to say about closing this
	// channel?, no?, then close the channel.  We remove the
	// channel before sending replies so we avoid channel closing
	// collition.
	if (channel->channel_num == 0) {
		// install a waiter into the channel 0 for the message  to be sent
		channel_num_str = g_strdup_printf ("%d", vortex_frame_get_msgno (frame));
		vortex_channel_install_waiter  (channel0, channel_num_str);

		// flag de connection to be at close processing. We
		// set this flag so vortex reader won't complain about
		// not properly connection close.
		vortex_connection_set_data (connection, "being_closed", GINT_TO_POINTER (TRUE));

		// send ok message
		vortex_channel_send_rpyv       (channel0, vortex_frame_get_msgno (frame), _ok_msg);
		
		// block until the message is sent
		vortex_channel_wait_until_sent (channel0, channel_num_str);
		g_free (channel_num_str);

		// close the connection
		__vortex_connection_set_not_connected (connection, "channel closed properly");
		
	}else {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel %d with profile %s sucessfully closed", 
		       channel->channel_num, channel->profile);

		// send the positive reply (ok message)
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sending nreply to close channel=%d", channel_num);
		vortex_channel_send_rpyv (channel0, vortex_frame_get_msgno (frame), _ok_msg);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "reply sent to close=%d", channel_num);

		// before closing the channel ensure there are not message being_sending
		if (channel->being_sending) {
			g_mutex_lock   (channel->send_mutex);
			g_mutex_unlock (channel->send_mutex);
		}

		// remove channel from connection
		vortex_connection_remove_channel (connection, channel);
	}

	return;
}

/** 
 * @internal
 * 
 * @param channel0 
 * @param frame 
 */
void __vortex_channel_0_frame_received_ok_msg (VortexChannel * channel0, VortexFrame * frame)
{
	
	return;
}

/** 
 * @internal
 * @brief frame received for channel 0 inside Vortex Library.
 *
 * Internal vortex library function. This function handle all incoming
 * frames that are received on channel 0 for a given connection.
 * 
 * Channel 0 is a really important channel for each session on BEEP
 * standard. Over this channel is negociated new channel creation and
 * profiles used for these new channels.
 *
 * If you are looking for the handler which process: start and close
 * message defined on RFC 3080 this is the place.
 *
 * @param channel0 
 * @param connection 
 * @param frame 
 * @param user_data 
 */
void vortex_channel_0_frame_received (VortexChannel    * channel0,
				      VortexConnection * connection,
				      VortexFrame      * frame,
				      gpointer           user_data)
{
	if (vortex_channel_get_number (channel0) != 0) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "invoked channel 0 frame received over a different channel (%d)\n",
		       vortex_channel_get_number (channel0));
		__vortex_connection_set_not_connected (connection, 
						       "invoked channel 0 frame received over a different channel (%d)\n");
		return;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "frame received over channel 0 (size: %d):\n%s\n",
	       vortex_frame_get_payload_size (frame),
	       vortex_frame_get_payload (frame));

	// validate message
	if (!__vortex_channel_0_frame_received_validate (channel0, frame))
		return;
	
	switch (__vortex_channel_0_frame_received_identify_type (channel0, frame)) {
	case START_MSG:
		__vortex_channel_0_frame_received_start_msg (channel0, frame);
		break;
	case CLOSE_MSG:
		__vortex_channel_0_frame_received_close_msg (channel0, frame);
		break;
	case OK_MSG:
		break;
	case ERROR_MSG:
		break;
		// should not happen
	}
	
	return;
}



/** 
 * @brief Return channel opened state. 
 * 
 * This is actually used by vortex_reader thread to detect if an
 * incoming message received can be delivered. Vortex reader use this
 * function to detect if a channel is being closed but waiting for
 * some recevied msg to be replied after sending close message.
 *
 * Before issuing a close message to close a channel
 * (\ref vortex_channel_close) the channel must fulfill some steps. So,
 * until reply to a close message is receive a channel is considered
 * to be valid to keep on sending replies to received messages.  
 * 
 * Note that this situation can take some time because remote peer, at
 * the moment of receiving a close message, could have tons of message
 * waiting to be sent.
 * 
 * Vortex library consumer can use this function to avoid sending
 * message over a channel that can be on an ongoing closing process.
 * 
 * @param channel the channel to operate on.
 * 
 * @return TRUE if a channel is opened or FALSE if not.
 */
gboolean        vortex_channel_is_opened       (VortexChannel * channel)
{
	return channel->is_opened;
}

/** 
 * @brief Return if a channel is on the process of being closed. 
 * 
 * If a channel is being closed means no more message MSG can be sent
 * over it. But, actually more message can still be received (and
 * these message must be replied). On this context a channel being
 * closed can be used to send rpy and err message.
 *
 * This function can be used by vortex library consumer to know if a
 * channel can be used to send more replies to received messages.
 * 
 * @param channel the channel to operate on
 * 
 * @return if channel is being closed.
 */
gboolean        vortex_channel_is_being_closed (VortexChannel * channel)
{
	return channel->being_closed;
}

/** 
 * @brief free channel allocated resources.
 *
 * Free channel resources. Vortex Library consumers must not call this
 * function directly.
 *
 * In order to close and free the channel properly
 * \ref vortex_channel_close must be used.
 * 
 * @param channel the channel to free.
 */
void vortex_channel_free (VortexChannel * channel)
{
	g_return_if_fail (channel);

	VortexWriterData * writer_data;
	WaitReplyData    * wait_reply;
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing resources for channel %d", channel->channel_num);

	// check if this channel belongs to a pool to remove it
	if (channel->pool) {
		// it seems this channel belongs to a pool remove it from the pool
		vortex_channel_pool_deattach (channel->pool, channel);
	}


	// ensure no wait reply is running remaing to unlock the
	// channel receive_mutex. Lock on it and unlock it inmediatly.
	g_mutex_lock        (channel->receive_mutex);
	g_mutex_unlock      (channel->receive_mutex);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing receive_mutex");
	g_mutex_free        (channel->receive_mutex);
	channel->receive_mutex = NULL;

	g_mutex_lock        (channel->send_mutex);
	g_mutex_unlock      (channel->send_mutex);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing send_mutex channel=%d", channel->channel_num);
	g_mutex_free        (channel->send_mutex);
	channel->send_mutex = NULL;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing update_receive__mutex");
	g_mutex_free        (channel->update_received_mutex);
	channel->update_received_mutex = NULL;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing close_mutex");
	g_mutex_free        (channel->close_mutex);
	channel->close_mutex = NULL;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing pending_mutex");
	g_mutex_free        (channel->pending_mutex);
	channel->pending_mutex = NULL;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing reply_cond");
	g_cond_free         (channel->reply_cond);
	channel->reply_cond = NULL;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing close_cond");
	g_cond_free         (channel->close_cond);
	channel->close_cond = NULL;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing pending_cond");
	g_cond_free         (channel->pending_cond);
	channel->pending_cond = NULL;

	g_free              (channel->profile);
	channel->profile = NULL;
	vortex_hash_destroy (channel->data);
	channel->data = NULL;

	// freeing queue messages
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing messages queue");
	while (!vortex_queue_is_empty (channel->queue_msgs)) {
		writer_data = vortex_queue_pop (channel->queue_msgs);
		vortex_writer_data_free (writer_data);
	}
	vortex_queue_free   (channel->queue_msgs);
	channel->queue_msgs = NULL;
	
	// freeing wait replies
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing waiting reply queue");
	while (!vortex_queue_is_empty (channel->waiting_msgno)) {
		wait_reply = vortex_queue_pop (channel->waiting_msgno);
		vortex_channel_free_wait_reply (wait_reply);
	}
	vortex_queue_free   (channel->waiting_msgno);
	channel->waiting_msgno = NULL;

	// free channel
	g_free (channel);

	return;
}


/** 
 * @brief Frees allocated memory by wait reply data
 *
 * See \ref wait_reply "this section" to know more about this function
 * and how it is used inside the Wait Reply process.
 * 
 * @param wait_reply Wait reply to free.
 */
void     vortex_channel_free_wait_reply (WaitReplyData * wait_reply)
{
	g_return_if_fail (wait_reply);

	g_async_queue_unref (wait_reply->queue);
	g_free (wait_reply);

	return;
}

/** 
 * @brief Returns the actual state for a given channel about pending
 * replies. 
 * 
 * Due to RFC3080/RFC3081 design any message sent will be replyed in
 * the order the message was issued. This means that if you use the same
 * channel to send two messages the reply for the second one will not
 * be received until the previous one. This is true even the second
 * message have task-impact lower than previous on the remote peer.
 *
 * Of course this is botleneck problem because you can increase
 * greatly the message/reply relation using channels that are ready in
 * the sense they are not waiting for message replies so your message
 * will not be blocked due to previous ones.
 *
 * This function returns if a channel is ready in the previous sense:
 * <i>"No previous message is waiting to be replied over this channel so
 * my message will only wait as long as the remote peer process my
 * message and reply it"</i>.
 *
 * Keep in mind a channel is allways ready to send messages. In fact,
 * eventually any message sent over a channel will have its reply but
 * this "ready sense" is a matter or performance not availability.
 * 
 * @param channel the channel to check.
 * 
 * @return TRUE if ready or FALSE if not.
 */
gboolean           vortex_channel_is_ready                       (VortexChannel * channel)
{
	g_return_val_if_fail (channel, FALSE);
	
	return ((channel->last_message_sent == (channel->last_reply_expected - 1)) && channel->reply_processed);
}

/** 
 * @internal
 * 
 * This function is for internal Vortex library purposes. Actually
 * this function is used by the vortex_channel_pool to check if a
 * message is ready from the point of view that all replies to message
 * sent have been received discarding if that replies have been
 * actually processed.
 *
 * This function should not be useful for vortex library consumer but
 * can be used to check if a channel have received (but may not
 * processed) all replies.
 * 
 * @param channel the channel to operate on
 * 
 * @return 
 */
gboolean           vortex_channel_is_up_to_date (VortexChannel * channel)
{
	g_return_val_if_fail (channel, FALSE);
	
	return (channel->last_message_sent == (channel->last_reply_expected - 1));
}

/** 
 * @brief Creates a new wait reply to be used to wait for a especific reply.
 * 
 * 
 * @return a new Wait Reply object. 
 */
WaitReplyData * vortex_channel_create_wait_reply ()
{
	WaitReplyData * data;	
	GAsyncQueue   * queue;

	queue              = g_async_queue_new ();
	data               = g_new0 (WaitReplyData, 1);
	data->queue        = queue;

	return data;
}

/** 
 * @brief Allows caller to wait for a particular reply to be received. 
 * 
 * 
 * Check \ref wait_reply "this section" to know more about how using
 * this function inside the Wait Reply Method.
 * 
 * @param channel the channel where Wait Reply is done
 * @param msg_no the message number we are waiting to be replyed
 * @param wait_reply the wait reply object used.
 * 
 * @return the frame reply or NULL if fails
 */
VortexFrame   * vortex_channel_wait_reply              (VortexChannel * channel, gint msg_no, 
							WaitReplyData * wait_reply)
{

	VortexFrame   * frame;

	g_return_val_if_fail (channel,     NULL);
	g_return_val_if_fail (msg_no >= 0, NULL);
	g_return_val_if_fail (wait_reply,  NULL);

	
	// wait for the message to be replyed
	frame = g_async_queue_pop (wait_reply->queue);

	// free data no longer needed
	vortex_channel_free_wait_reply (wait_reply);

	// because we have accept to deliver the frame to a
	// waiting thread, we understand this frame have being delivered
	channel->reply_processed = TRUE;
	
	return frame;
}

/** 
 * @internal
 *
 * This function actually is used by the vortex reader process. It
 * allows vortex reader process to create a critical section while
 * update channel status.
 * 
 * @param channel the channel to operate.
 */
void            vortex_channel_lock_to_update_received   (VortexChannel * channel)
{
	g_return_if_fail (channel);

	g_mutex_lock (channel->update_received_mutex);

	return;
}

/** 
 * @internal
 * 
 * This function actually is used by the vortex reader process. It
 * allows vortex reader process to create a critical section while
 * update channel status. 
 * 
 * @param channel the channel to operate on.
 */
void            vortex_channel_unlock_to_update_received (VortexChannel * channel)
{
	g_return_if_fail (channel);

	g_mutex_unlock (channel->update_received_mutex);

	return;
}

/** 
 * @internal
 * 
 * @param channel 
 */
void               vortex_channel_lock_to_receive                (VortexChannel * channel)
{
	g_return_if_fail (channel);

	g_mutex_lock (channel->receive_mutex);

	return;
}

/** 
 * @internal
 * 
 * @param channel 
 */
void               vortex_channel_unlock_to_receive              (VortexChannel * channel)
{
	g_return_if_fail (channel);
	
	g_mutex_unlock (channel->receive_mutex);

	return;
}

/** 
 * @internal
 *
 * The function __vortex_channel_0_frame_received_close_msg can be
 * blocked awaiting to receive all replies expected. The following
 * signal tries to wake up a posible thread blocked until
 * last_reply_expected change.
 * 
 * @param channel 
 */
void           	   vortex_channel_signal_on_close_blocked        (VortexChannel    * channel)
{

	g_return_if_fail (channel);
	g_mutex_lock   (channel->close_mutex);
	// flag channel to be up to date on reply process
	channel->reply_processed = TRUE;

	// check if channel is being closed
	if (!channel->being_closed) {
		g_mutex_unlock (channel->close_mutex);
		return;
	}
	
	// do not signal if not close waiting
	if (!channel->waiting_replies) {
		g_mutex_unlock (channel->close_mutex);
		return;
	}

	// do a close waiting signal
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "signaling blocked thread on close msg process");

	g_cond_signal  (channel->close_cond);
	g_mutex_unlock (channel->close_mutex);

	return;
}

/** 
 * @internal
 *
 * Function used to unblock threads on close process that are waiting
 * all replies to message received are sent. Until done this, the
 * channel can not wait for a ok message.
 * 
 * @param channel 
 */
void   vortex_channel_signal_reply_sent_on_close_blocked (VortexChannel * channel)
{
	g_return_if_fail (channel);

	// check if channel is being closed
	if (!channel->being_closed) {
		return;
	}
	

	g_mutex_lock (channel->pending_mutex);
	g_cond_signal (channel->pending_cond);
	g_mutex_unlock (channel->pending_mutex);

	return;
}

/** 
 * @internal
 *
 * Allows to flag reply_processed for a given channel to allow vortex
 * reader to be able to notify a reply frame have been received and is
 * waiting to be processed.
 * 
 * @param channel 
 * @param flag 
 */
void vortex_channel_flag_reply_processed (VortexChannel * channel, gboolean flag)
{
	g_return_if_fail (channel);

	g_mutex_lock   (channel->close_mutex);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "setting pending reply processed=%d for channel=%d",
	       flag, channel->channel_num);
	channel->reply_processed = flag;
	g_mutex_unlock (channel->close_mutex);

	return;
}


/** 
 * @internal
 *
 * This function allows to close channel processing while this message
 * refers to channel0 to properly synchronize the ok message response
 * and connection closing. 
 *
 * Used in conjuntion with \ref vortex_channel_wait_until_sent enable to
 * get blocked until a reply have been sent.
 *
 * This function is for vortex internal purposes so it should not be
 * useful for vortex consumers.
 * 
 * @param channel 
 * @param rpy 
 */
void               vortex_channel_install_waiter                 (VortexChannel * channel,
								  gchar * rpy)
{
	GAsyncQueue    * queue;
	g_return_if_fail (channel);
	g_return_if_fail (rpy && (* rpy));

	queue = g_async_queue_new ();
	vortex_channel_set_data (channel, rpy, queue);

	return;
}

/** 
 * @internal
 *
 * Block the caller until the message have been sent. This function is
 * for internal vortex purposes so it should not be useful to vortex
 * consumers.
 * 
 * @param channel 
 * @param rpy 
 */
void               vortex_channel_wait_until_sent                (VortexChannel * channel,
								  gchar *rpy)
{
	GAsyncQueue * queue;

	g_return_if_fail (channel);
	g_return_if_fail (rpy && (* rpy));

	// get the queue
	queue = vortex_channel_get_data (channel, rpy);

	g_return_if_fail (queue);

	// wait until message is sent
	g_async_queue_pop   (queue);
	g_async_queue_unref (queue);
	
	return;
}

/** 
 * @internal
 * 
 * @param channel 
 * @param rpy 
 */
void             vortex_channel_signal_rpy_sent                 (VortexChannel * channel,
								 gchar * rpy)
{
	GAsyncQueue * queue;
	g_return_if_fail (channel);
	g_return_if_fail (rpy && (* rpy));

	// get the queue
	queue = vortex_channel_get_data (channel, rpy);

	if (queue == NULL)
		return;

	// push some data, to wakup up the waiting thread
	g_async_queue_push (queue, GINT_TO_POINTER (TRUE));

	return;
}

/** 
 * @internal
 *
 * This function is for internal vortex purposes. This function
 * actually allows vortex channel pool module to notify this channel
 * that belongs to a pool so in case of a channel close (received or
 * invoked) this channel will be able to notify channel pool to remove
 * the reference to the channel.
 * 
 * @param channel the channel to notify it belongs to a pool.
 * @param pool the pool the channel belongs to.
 */
void               vortex_channel_set_pool                       (VortexChannel * channel,
								  VortexChannelPool * pool)
{
	g_return_if_fail (channel);
	g_return_if_fail (pool);

	channel->pool = pool;

	return;
}


//@}
