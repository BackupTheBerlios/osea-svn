//  LibVortex:  A BEEP implementation for af-arch
//  Copyright (C) 2005 Advanced Software Production Line, S.L.
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

#include <vortex.h>
#include <readline/readline.h>

void vortex_client_show_help () {
	g_print ("Vortex-client cmd help:\n");
	g_print ("  help              - show this help\n");
	g_print ("  connect           - connects to a remove vortex (BEEP enabled) server\n");
	g_print ("  close             - close connection opened\n");
	g_print ("  show profiles     - show remote peer profiles\n");
	g_print ("  write frame       - write and sends a frame\n");
	g_print ("  write start msg   - sends a raw start channel message and returns\n");
	g_print ("                      the remote peer response to stdout\n");
	g_print ("  new channel       - creates a new channel using the vortex api.\n");
	g_print ("  write close msg   - sends a raw close channel message and returns\n");
	g_print ("                      the remote peer response to stdout\n");
	g_print ("  close channel     - close a channel using the vortex api.\n");
	g_print ("  send message      - send a message using the vortex api.\n");
	g_print ("  write fragment    - write and sends a frame but splitting the message\n");
	g_print ("                      into several pieces to test\n");
	g_print ("                      remote frame fragment reading support.\n");
	g_print ("  channel status    - returns actual channel status\n");
	g_print ("  connection status - returns actual connection status\n");
	g_print ("  quit              - quit from tool\n");

}

void vortex_show_initial_greetings () 
{
	g_print ("Vortex-client v.%s: a cmd tool to test vortex (and BEEP-enabled) peers\n", VERSION);
	g_print ("Copyright (c) 2005 Adavnced Software Production Line, S.L.\n");
}

gchar *  get_and_check_cancel () {
	gchar * response = NULL;

	g_print ("  0) cancel process\n");
	response = readline ("you chose: ");
	if (!g_strncasecmp (response, "0", 1)) {
		g_free (response);
		response = NULL;
		g_print ("canceling process..\n");
	}
	return response;
}

VortexFrameType check_frame_type (gchar * response) {

	g_return_val_if_fail (response && (* response), VORTEX_FRAME_TYPE_UNKNOWN);

	if (!g_strncasecmp (response, "1", 1)) {
		return VORTEX_FRAME_TYPE_MSG;
	}
	if (!g_strncasecmp (response, "2", 1)) {
		return VORTEX_FRAME_TYPE_RPY;
	}
	if (!g_strncasecmp (response, "3", 1)) {
		return VORTEX_FRAME_TYPE_ERR;
	}
	if (!g_strncasecmp (response, "4", 1)) {
		return VORTEX_FRAME_TYPE_ANS;
	}
	if (!g_strncasecmp (response, "5", 1)) {
		return VORTEX_FRAME_TYPE_NUL;
	}
	return VORTEX_FRAME_TYPE_UNKNOWN;
}
	

gint32  get_number_int (gchar * ans, gint32 limit) {
	gchar * response;
	gint    num;
 label:
	g_print (ans);
        response  = readline ("you chose: ");
        num       = atoi (response);
	if (num < 0 || num > limit ) {
                g_free (response);
		g_print ("You must provide a valid number from 0 up to %d\n", limit);
		goto label;
	}
        g_free (response);
        response = NULL;
	return num;
}

guint32 get_number_long (gchar * ans, gint64 limit) {
	gchar * response;
	guint32 num;

 label:
	g_print (ans);
	response  = readline ("you chose: ");
	num       = strtoll (response, NULL, 10);
	if (num < 0 || num > limit ) {
                g_free (response);
		g_print ("You must provide a valid number from 0 up to %llu\n", limit);
		goto label;
	}
        g_free (response);
        response = NULL;
	return num;
}

void vortex_client_write_frame (VortexConnection * connection, gboolean fragment)
{
	gchar            * response          = NULL;
	VortexFrameType    type              = VORTEX_FRAME_TYPE_UNKNOWN;
	gint32             channel_num       = 0;
	gint32             message_num       = 0;
	guint64            sequence_num      = 0;
	gint32             payload_size      = 0;
	gint32             ansno_num         = 0;
	gchar            * payload           = NULL;
	gchar            * str               = NULL;
	gchar            * the_whole_payload = NULL;
	gboolean           continuator;
	gchar            * the_frame;
	gint               iterator;
	gint               length;
	
	g_print ("What type of frame do you want to send?\n");
	g_print ("  1) MSG\n");
	g_print ("  2) RPY\n");
	g_print ("  3) ERR\n");
	g_print ("  4) ANS\n");
	g_print ("  5) NUL\n");
	response = get_and_check_cancel ();
	if (response == NULL)
		return;

	// get frame type
	type = check_frame_type (response);
	g_free (response);

	channel_num = get_number_int ("What is the channel to be used to send frame?\n",
				      MAX_CHANNEL_NO);
	
	message_num = get_number_int ("What is the message number to set?\n",
				      MAX_CHANNEL_NO);
	
	g_print ("What message continuator to set?\n");
	g_print ("  1) . (no more message indicator)\n");
	g_print ("  2) * (more messages to come indicator)\n");
	response = get_and_check_cancel ();
	if (response == NULL)
		return;

	if (!g_strncasecmp (response, "1", 1))
		continuator = FALSE;
	else
		continuator = TRUE;
	g_free (response);

	sequence_num = get_number_long ("What is the sequence number to be used?\n", MAX_SEQ_NO);

	ansno_num    = get_number_int  ("What is the ansno number value ?\n", 214783647);

	payload_size = get_number_int  ("What is the payload size?\n", 214783647);

	g_print ("Write the frame payload to send (control-D to finish on last line):\n");
	g_print ("NOTE: every line you introduce, is completed with a \\r\\n (aka CR LF) terminator\n");
	while ((payload = readline (NULL)) != NULL) {
		if (the_whole_payload == NULL) {
			the_whole_payload = g_strdup_printf ("%s\r\n", payload);
			g_free (payload);
			continue;
		}
		str = the_whole_payload;
		the_whole_payload = g_strdup_printf ("%s%s\r\n", the_whole_payload, payload);
		g_free (str);
		g_free (payload);
	}

	g_print ("Building frame..\n");
	the_frame = vortex_frame_build_up_from_params (type,
						       channel_num,
						       message_num,
						       continuator,
						       sequence_num,
						       payload_size,
						       ansno_num,
						       the_whole_payload ? the_whole_payload : "");
	if (the_whole_payload != NULL)
		g_free (the_whole_payload);
	g_print ("frame to send:\n%s\nDo you want to send this frame?\n", the_frame);
	g_print ("  1) yes\n");
	response = get_and_check_cancel ();
	if (response == NULL) 
		return;
	
	g_print ("Sending frame..\n");
	if (fragment) {
		// send a raw frame (splitting it)
		iterator = 0;
		length   = strlen (the_frame) / 3;
		while (iterator < length) {
			g_print ("sending from %d to %d bytes..\n", iterator, length);
			if (!vortex_frame_send_raw (connection, (the_frame + iterator), 
						    length)) {
				g_print ("unable to send frame in raw mode (fragments): %s\n", 
					 vortex_connection_get_message (connection));
			}
			
			// update indexes
			iterator +=length;
			if ((strlen (the_frame + iterator)) < length)
				length = strlen (the_frame + iterator);

		}
		

	} else {
		// send a raw frame (complete one) 
		if (!vortex_frame_send_raw (connection, the_frame, strlen (the_frame))) {
			g_print ("unable to send frame in raw mode: %s\n", 
				 vortex_connection_get_message (connection));
		}
	}
	g_free (the_frame);

	return;
}

void vortex_client_get_channel_status (VortexConnection * connection)
{
	gchar         * response;
	gint            channel_num;
	VortexChannel * channel;

	response    = readline ("Channel number to get status: ");
	channel_num = atoi (response);
	g_free (response);

	channel = vortex_connection_get_channel (connection, channel_num);
	if (channel == NULL) {
		g_print ("No channel %d is actually opened, leaving status operation..\n", channel_num);
		return;
	}
	
	g_print ("Channel %d status is: \n", channel_num);
	g_print ("  Profile definition: \n");
	g_print ("     %s\n", vortex_channel_get_profile (channel));
	g_print ("  Synchronization: \n");
	if (vortex_channel_get_next_msg_no (channel) == 0) 
		g_print ("     Last msqno sent:          no message sent yet\n");
	else
		g_print ("     Last msqno sent:          %d\n",  vortex_channel_get_next_msg_no (channel) - 1);
	
	g_print ("     Next msqno to use:        %d\n",  vortex_channel_get_next_msg_no (channel));
	if (vortex_channel_get_next_expected_msg_no (channel) == 0) 
		g_print ("     Last msgno received:      no message received yet\n");
	else
		g_print ("     Last msgno received:      %d\n",  vortex_channel_get_next_expected_msg_no (channel) - 1);
	g_print ("     Next reply to sent:       %d\n",  vortex_channel_get_next_reply_no (channel));
	g_print ("     Next reply exp. to recv:  %d\n",  vortex_channel_get_next_expected_reply_no (channel));
	g_print ("     Next exp. msgno to recv:  %d\n",  vortex_channel_get_next_expected_msg_no (channel));
	g_print ("     Next seqno to sent:       %u\n",  vortex_channel_get_next_seq_no (channel));
	g_print ("     Next seqno to receive:    %u\n",  vortex_channel_get_next_expected_seq_no (channel));
	
	return;
}

gboolean  check_connected (gchar * error, VortexConnection * connection) {
	
	if ((connection == NULL) || (!vortex_connection_is_ok (connection, FALSE))) {
		g_print ("%s, connect first\n", error);
		return FALSE;
	}
	return TRUE;

}

void vortex_client_write_start_msg (VortexConnection * connection)
{
	GList           * list_aux;
	GList           * profiles;
	gchar           * response;
	gchar           * the_frame;
	gchar           * the_msg;
	gint              profile;
	gint              channel_num;
	gint              iterator = 1;
	VortexChannel   * channel0;
	VortexFrame     * frame;
	WaitReplyData   * wait_reply;
	gint              msg_no;

	// get profile to use
	g_print ("This procedure will send an start msg to remote peer. This actually\n");
	g_print ("bypass vortex library channel creation api. This is used for debuging\n");
	g_print ("beep enabled peer responses and its behaviour.\n\n");
		 
	g_print ("Select what profile to use to create for the new channel?\n");
	profiles = vortex_connection_get_remote_profiles (connection);
	g_print ("profiles for this peer: %d\n", g_list_length (profiles));
	list_aux = g_list_first (profiles);
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		g_print ("  %d) %s\n", iterator, (gchar*) list_aux->data);
		iterator++;
	}
	response = get_and_check_cancel ();
	if (response == NULL) {
		g_print ("canceling process..\n");
		return;
	}
	profile  = atoi (response);
	g_free (response);

	// get channel num to use
	channel_num = get_number_int ("What channel number to create: ", 2147483647);
	
	// get management channel
	channel0    = vortex_connection_get_channel (connection, 0);

	// create the start message
	the_msg     = g_strdup_printf ("Content-Type: application/beep+xml\r\n\r\n<start number='%d'>\r\n   <profile uri='%s' />\r\n</start>\r\n",
				       channel_num, 
				       (gchar *) g_list_nth_data (profiles, profile - 1));
	
	the_frame   = vortex_frame_build_up_from_params (VORTEX_FRAME_TYPE_MSG,
							 0,
							 vortex_channel_get_next_msg_no (channel0),
							 FALSE,
							 vortex_channel_get_next_seq_no (channel0),
							 strlen (the_msg),
							 0,
							 the_msg);

	// send the message
	wait_reply = vortex_channel_create_wait_reply ();
	if (!vortex_channel_send_msg_and_wait (channel0, the_msg, strlen (the_msg), &msg_no, wait_reply)) {
		vortex_channel_free_wait_reply (wait_reply);
		g_print ("unable to send start message");
		return;
	}
	
	// free no longer needed data
	g_free (the_msg);
	g_free (the_frame);

	// wait for response
	g_print ("start message sent, waiting reply..");
	frame = vortex_channel_wait_reply (channel0, msg_no, 
					   wait_reply);

	// check frame replyied
	if (vortex_frame_get_type (frame) == VORTEX_FRAME_TYPE_RPY) {
		g_print ("channel start ok, message (size: %d)\n%s", 
			 vortex_frame_get_payload_size (frame),
			 vortex_frame_get_payload (frame));
	}else {
		g_print ("channel start failed, message (size: %d)\n%s",
			 vortex_frame_get_payload_size (frame),
			 vortex_frame_get_payload (frame));
	}
	vortex_frame_free (frame);
	return;
}

void vortex_client_new_channel (VortexConnection * connection)
{
	GList         * list_aux;
	GList         * profiles;
	gchar         * response;
	gint            profile;
	gint            channel_num;
	gint            iterator = 1;
	VortexChannel * channel;

	// get profile to use
	g_print ("This procedure will request to create a new channel using Vortex api.\n");
		 
	g_print ("Select what profile to use to create for the new channel?\n");
	profiles = vortex_connection_get_remote_profiles (connection);
	g_print ("profiles for this peer: %d\n", g_list_length (profiles));
	list_aux = g_list_first (profiles);
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		g_print ("  %d) %s\n", iterator, (gchar*) list_aux->data);
		iterator++;
	}
	response = get_and_check_cancel ();
	if (response == NULL) {
		g_print ("canceling process..\n");
		return;
	}
	profile  = atoi (response);
	g_free (response);

	// get channel num to use
	channel_num = get_number_int ("What channel number to create: ", 2147483647);

	g_print ("creating new channel..");
	channel     = vortex_channel_new (connection, 
					  channel_num, 
					  (gchar *) g_list_nth_data (profiles, profile - 1),
					  NULL, NULL, NULL, NULL, NULL, NULL);
	if (channel == NULL) {
		g_print ("failed\n");
		return;
	}
	g_print ("ok, channel created: %d\n", vortex_channel_get_number (channel));

	return;
}

void vortex_client_write_close_msg (VortexConnection * connection)
{
	g_print ("no implemented yet..\n");
}

void vortex_client_close_channel (VortexConnection * connection)
{
	VortexChannel * channel;
	gint            channel_num;

	g_print ("This procedure will close a channel using the vortex api.\n");
	// get channel num to use
	channel_num = get_number_int ("What channel number to close: ", 2147483647);
	
	if (channel_num == 0) {
		g_print ("You can't close channel 0. It will be closed when session (or connection) is closed\n");
		return;
	}

	// get a channel reference and also check if this channel is registered by 
	// the vortex connetion
	channel = vortex_connection_get_channel (connection, channel_num);
	if (channel == NULL) {
		g_print ("no channel %d is opened, finishing..\n", channel_num);
		return;
	}

	// close the channel
	if (vortex_channel_close (channel, NULL)) 
		g_print ("Channel close: ok\n");
	else
		g_print ("Channel close: failed\n");
	
	return;
}

void vortex_client_send_message (VortexConnection * connection)
		
{
	gint              channel_num;
	VortexChannel   * channel;
	VortexFrame     * frame;
	WaitReplyData   * wait_reply   = NULL;
	gchar           * msg_to_send;
	gchar           * wait;
	gboolean          do_a_wait;
	gboolean          result;
	gint              msg_no;

	g_print ("This procedure will send a message using the vortex api.\n");
	// get the channel number to use and check this message exists
	channel_num = get_number_int  ("What channel do you want to use to send the message? ",
				       MAX_CHANNEL_NO);
	
	if (!vortex_connection_channel_exists (connection, channel_num)) {
		g_print ("This channel doesn't exists..\n");
		return;
	}

	// get the message to send
	msg_to_send = readline ("Type the message to send:\n");
	if (msg_to_send == NULL) {
		g_print ("no message to send..\n");
		return;
	}

	// get if user wants to wait for the message
	g_print ("Do you want to wait for message reply? (Y/n) ");
	wait  = readline (NULL);
	do_a_wait = ((wait != NULL) && ((strlen (wait) == 0) || !g_strcasecmp ("Y", wait) || !g_strcasecmp ("y", wait)));
	if (wait != NULL)
		g_free (wait);

	// send the message
	channel     = vortex_connection_get_channel (connection, channel_num);
	if (do_a_wait) {
		wait_reply = vortex_channel_create_wait_reply ();
		result      = vortex_channel_send_msg_and_wait (channel, msg_to_send, 
								strlen (msg_to_send), &msg_no, wait_reply);
	}else {
		result      = vortex_channel_send_msg (channel, msg_to_send, strlen (msg_to_send), &msg_no);
	}
	
	// check result
	if (!result) {
		g_print ("Unable to send the message\n");
		g_free (msg_to_send);
		if (do_a_wait)
			vortex_channel_free_wait_reply (wait_reply);
		return;
	}
	g_free (msg_to_send);
	g_print ("Message number %d was sent..\n", msg_no);

	// do a wait if posible
	if (do_a_wait) {
		g_print ("waiting for reply...");
		frame = vortex_channel_wait_reply (channel, msg_no, wait_reply);
		g_print ("reply received: \n%s\n", vortex_frame_get_payload (frame));
		vortex_frame_free (frame);
		return;
	}
	g_print ("leaving without waiting..\n");
	return;
}

void __vortex_client_connection_status (gpointer key, gpointer value, gpointer user_data)
{
	VortexChannel * channel = value;

	g_print ("  channel: %d, profile: %s\n", 
		 vortex_channel_get_number (channel), 
		 vortex_channel_get_profile (channel));

	return;
}

void vortex_client_connection_status (VortexConnection * connection)
{
	g_print ("Created channel over this session:\n");

	vortex_connection_foreach_channel (connection, __vortex_client_connection_status, NULL);
	
	return;
}

gint main (gint argc, gchar ** argv) 
{
	gchar            * host;
	gchar            * port;
	VortexConnection * connection = NULL;
	GList            * profiles;
	gchar            * line       = NULL;
	

	// init vortex library
	vortex_init ();

	vortex_show_initial_greetings ();

	while (TRUE) {
		
		if ((vortex_connection_is_ok (connection, FALSE)))
			line = readline ("[===] vortex-client > ");
		else
			line = readline ("[=|=] vortex-client > ");

		if (line == NULL) {
			continue;
		}
		if (!g_strncasecmp ("help", line, 4)) {
			vortex_client_show_help ();
		}
		if (!g_strncasecmp ("show profiles", line, 13)) {
			if (!check_connected ("can't show remote peer profiles", connection))
				goto loop_end;

			g_print ("Supported remote peer profiles:\n");
			profiles = vortex_connection_get_remote_profiles (connection);
			for (profiles = g_list_first (profiles) ;profiles; profiles = g_list_next (profiles)) {
				g_print ("  %s\n", (gchar *) profiles->data);
			}
		}

		if (!g_strncasecmp ("connection status", line, 17)) {
			if (!check_connected ("can't show connection status", connection))
				goto loop_end;
			
			vortex_client_connection_status (connection);
			
			continue;
		}

		if (!g_strncasecmp ("connect", line, 7)) {

			// close previous 
			if (vortex_connection_is_ok (connection, FALSE))
				vortex_connection_close (connection);

			// get user data and connect to vortex server
			host = readline ("server to connect to: ");

			port = readline ("port to connect to: ");
			
			g_print ("connecting to %s:%s..", host, port);
			// create a vortex session
			connection = vortex_connection_new (host, port, NULL, NULL);

			// check if connection is ok
			if (!vortex_connection_is_ok (connection, FALSE)) {
				g_print ("failed\n");
				g_print ("Unable to connect to vortex server on: %s:%s vortex message was: %s\n",
					 host, port, vortex_connection_get_message (connection));
				connection = NULL;
				vortex_connection_close (connection);
				continue;
			}
			g_print ("ok: vortex message: %s\n", vortex_connection_get_message (connection));
		}

		if (!g_strncasecmp ("close channel", line, 13)) {
			if (!check_connected ("can't close channel", connection))
				goto loop_end;
			g_print ("closing the channel..\n");
			vortex_client_close_channel (connection);
			continue;
		}

		if (!g_strncasecmp ("quit", line, 4)) {
			if (vortex_connection_is_ok (connection, FALSE))
				if (!vortex_connection_close (connection)) {
					g_print ("there was an error while closing the connection..");
				}
			// finish vortex client
			exit (0);
		}

		if (!g_strncasecmp ("close", line, 5)) {
			if (!check_connected ("can't close, not connected", connection))
				goto loop_end;
			
			if (!vortex_connection_close (connection)) {
				g_print ("Unable to close the channel");
			}else {
				connection = NULL;
			}
			continue;
		}

		if (!g_strncasecmp ("write frame", line, 11)) {
			if (!check_connected ("can't write and send a frame", connection))
				goto loop_end;

			vortex_client_write_frame (connection, FALSE);
			continue;
		}

		if (!g_strncasecmp ("channel status", line, 14)) {
			if (!check_connected ("can't get channel status", connection))
				goto loop_end;

			vortex_client_get_channel_status (connection);
			continue;
		}

		if (!g_strncasecmp ("write start msg", line, 15)) {
			if (!check_connected ("can't send start msg", connection))
				goto loop_end;

			vortex_client_write_start_msg (connection);
			continue;
		}

		if (!g_strncasecmp ("new channel", line, 11)) {
			if (!check_connected ("can't create new channel", connection))
				goto loop_end;

			vortex_client_new_channel (connection);
			continue;
		}

		if (!g_strncasecmp ("write close msg", line, 15)) {
			if (!check_connected ("can't send close msg", connection))
				goto loop_end;

			vortex_client_write_close_msg (connection);
			continue;
		}

		if (!g_strncasecmp ("send message", line, 12)) {
			if (!check_connected ("can't send a message", connection))
				goto loop_end;
			
			vortex_client_send_message (connection);
			continue;
		}

		if (!g_strncasecmp ("write fragment", line, 14)) {
			if (!check_connected ("can't send a message", connection))
				goto loop_end;
			
			vortex_client_write_frame (connection, TRUE);
			continue;
		}

		
	loop_end:
		if (line == NULL || strlen (line) == 0)
			vortex_client_show_help ();
		if (line && (*line))
			g_free (line);
		line = NULL;
	}

	// finaly free connection
	if (vortex_connection_is_ok (connection, FALSE))
		vortex_connection_close (connection);
	
	return (0);
}

