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

#define MAX_BUFFER_SIZE 65536
#define LOG_DOMAIN "vortex-frame-factory"


struct _VortexFrame {
	// frame header definition
	VortexFrameType   type;
	gint32            channel;
	gint32            msgno;
	gboolean          more;
	gchar             more_char;
	guint32           seqno;
	gint32            size;
	gint32            ansno;
	
	// content type
	gchar           * content_type;

	// the payload message
	gchar           * payload;
 };

/**
 * \defgroup vortex_frame Vortex Frame Factory: Function to manipulate frames inside Vortex Library
 */

/// \addtogroup vortex_frame
//@{

/**
 * @brief Retuns the frame in plain text forma from a VortexFrame object.
 * 
 * Returns actual frame message (the complete string) <i>frame</i> represents. The return value
 * must be freed when no longed is needed.
 *
 * @param frame the frame to get as raw text.
 * 
 * @return the raw frame or NULL if fails
 **/
gchar *       vortex_frame_get_raw_frame         (VortexFrame * frame)
{
	g_return_val_if_fail (frame, NULL);

	return vortex_frame_build_up_from_params (frame->type,
						  frame->channel,
						  frame->msgno,
						  frame->more,
						  frame->seqno,
						  frame->size,
						  frame->ansno,
						  frame->payload);
}

/** 
 * @brief Builds a frame using the given parameters.
 *
 * This function should not be useful for Vortex Library consumer
 * because all frames received and sent are actually builded by Vortex
 * Library.
 *
 * @param type Frame type.
 * @param channel the channel number for the frame.
 * @param msgno the message number for the frame.
 * @param more more flag status for the frame.
 * @param seqno sequence number for the frame
 * @param size the frame payload size.
 * @param ansno the answer number for the frame
 * @param payload the payload is going to have the frame.
 * 
 * @return a newly created frame that should be freed using \ref vortex_frame_free.
 */
gchar * vortex_frame_build_up_from_params (VortexFrameType   type,
					   gint32            channel,
					   gint32            msgno,
					   gboolean          more,
					   guint32           seqno,
					   gint32            size,
					   gint32            ansno,
					   gchar  *          payload)
{
	
	gchar * message_type = NULL;
	gchar * ansno_string;
	gchar * value;

	switch (type) {
	case VORTEX_FRAME_TYPE_UNKNOWN:
		return NULL;
	case VORTEX_FRAME_TYPE_MSG:
		message_type = "MSG";
		break;
	case VORTEX_FRAME_TYPE_RPY:
		message_type = "RPY";
		break;
	case VORTEX_FRAME_TYPE_ANS:
		message_type = "ANS";
		break;
	case VORTEX_FRAME_TYPE_ERR:
		message_type = "ERR";
		break;
	case VORTEX_FRAME_TYPE_NUL:
		message_type = "NUL";
		break;
		
	}
	if (type == VORTEX_FRAME_TYPE_ANS) 
		ansno_string = g_strdup_printf (" %d", ansno);
	else
		ansno_string = g_strdup ("");

	value = g_strdup_printf ("%s %d %d %c %u %d%s\r\n%sEND\r\n",
				 message_type,           // message type
				 channel,                // channel 
				 msgno,                  // msgno 
				 more ? '*' : '.',       // more 
				 seqno,                  // seqno 
				 size,                   // size  
				 ansno_string,           // ansno ? 
				 payload);               // payload
	
	g_free (ansno_string);
	return value;
}


/** 
 * @internal
 * @brief reads n bytes from the connection.
 * 
 * @param connection the connection to read data.
 * @param buffer buffer to hold data.
 * @param maxlen 
 * 
 * @return how many bytes were read.
 */
gint         __vortex_frame_readn    (VortexConnection * connection, gchar * buffer, gint maxlen)
{
	gint    socket = vortex_connection_get_socket (connection);
	gint    nread;
	gchar * error_msg;

 __vortex_frame_readn_keep_reading:
	if ((nread = recv (socket, buffer, maxlen, 0)) == VORTEX_SOCKET_ERROR) {
		if (errno == EWOULDBLOCK) {
			return 0;
		}
		if (errno == EINTR)
			goto __vortex_frame_readn_keep_reading;
		
		error_msg = vortex_errno_get_last_error ();
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to readn=%d, error was: %s",
		       maxlen, error_msg ? error_msg : "");
	}

	buffer[nread] = 0;
	return nread;
}

/**
 * @internal

 * 
 * Read the next line, byte by byte until it gets a \n or maxlen is reached. Some code errors
 * are used to manage exceptions (see return values)
 * 
 * @param connection
 * @param buffer
 * @param maxlen
 * 
 * @return  values returned by this funtion follows:
 *  0 - remote peer have closed the file
 * -1 - an error have happend while reading
 * -2 - could read because this connection is on non-blocking mode and there is no
 *  n - some data was readed.
 * 
 **/
gint         __vortex_frame_readline (VortexConnection * connection, gchar * buffer, gint maxlen)
{
	gint socket = vortex_connection_get_socket (connection);
	
	gint  n, rc;
	gchar c, *ptr;
	gchar    * error_msg;

	ptr = buffer;
	for (n = 1; n < maxlen; n++) {
	__vortex_frame_readline_again:
		if (( rc = recv (socket, &c, 1, 0)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		}else if (rc == 0) {
			if (n == 1)
				return 0;
			else
				break;
		}else {
			if (errno == EINTR) 
				goto __vortex_frame_readline_again;
			if (errno == EWOULDBLOCK) 
				return (-2);

			error_msg = vortex_errno_get_last_error ();
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to read a line, error was: %s",
			       error_msg ? error_msg : "");
			return (-1);
		}
	}
	*ptr = 0;
	return (n);

}


/**
 * @internal

 * 
 * Tries to get the next incomming frame inside the @socket. If
 * received frame is ok, it return a newly-allocated VortexFrame with
 * its data. This function is only useful for vortex library
 * internals. Vortex library consumer should not use it.
 *
 * This function also close you connection if some error happens.
 * 
 * @param connection the connection where frame is going to be
 * received
 * 
 * @return a frame  or NULL if frame is wrong.
 **/
VortexFrame * vortex_frame_get_next     (VortexConnection * connection)
{
	gint          bytes_readed;
	gint          remaining;
	gint          body_init;
	gint          body_size;
	VortexFrame * frame;
	gint          iterator;
	gchar         line[100];
	gchar       * buffer = NULL;

	g_return_val_if_fail (connection, NULL);
	g_return_val_if_fail (vortex_connection_is_ok (connection, FALSE), NULL);

	// before reading anything else, we have to check if previous
	// read was complete if not, we are in a frame fragment case
	buffer = vortex_connection_get_data (connection, "buffer");
	if (buffer && (* buffer)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "received more data after a frame fragment, previous read isn't still complete");
		// get previous frame
		frame        = vortex_connection_get_data (connection, "frame");
		g_return_val_if_fail (frame, NULL);

		// get previous remaining
		remaining    = GPOINTER_TO_INT (vortex_connection_get_data (connection, "remaining_bytes"));
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "remaining bytes to be read: %d", remaining);
		g_return_val_if_fail (remaining > 0, NULL);

		// get previous bytes read
		bytes_readed = GPOINTER_TO_INT (vortex_connection_get_data (connection, "bytes_read"));
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "bytes already read: %d", bytes_readed);

		bytes_readed = __vortex_frame_readn (connection, buffer + bytes_readed, remaining);
		if (bytes_readed == 0) {
			vortex_frame_free (frame);
			g_free (buffer);
			vortex_connection_set_data (connection, "buffer", NULL);
			vortex_connection_set_data (connection, "frame", NULL);
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "remote peer have closed connection while reading the rest of the frame having received part of it");
			__vortex_connection_set_not_connected (connection, "remote peer have closed connection while reading the rest of the frame");
			return NULL;
		}


		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "bytes ready this time: %d", bytes_readed);

		// check data
		if (bytes_readed != remaining) {
			// add bytes readed to keep on reading
			bytes_readed += GPOINTER_TO_INT (vortex_connection_get_data (connection, "bytes_read"));
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "the frame fragment isn't still complete, total read: %d", bytes_readed);
			goto save_buffer;
			
		}
		
		// we have a complete buffer for the frame, let's
		// continue the process but, before doing that we have
		// to restore expected state of bytes_readed.
		bytes_readed = (frame->size + 5);
		vortex_connection_set_data (connection, "buffer", NULL);
		vortex_connection_set_data (connection, "frame", NULL);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "this already complete (total size: %d", frame->size);
		goto process_buffer;
	}

	// parse frame header, read the first line
	bytes_readed = __vortex_frame_readline (connection, line, 99);
	if (bytes_readed == -2) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "no data were waiting on this non-blocking set connection");
		return NULL;
	}

	if (bytes_readed == 0) {

		// check if channel is expected to be closed
		if (vortex_connection_get_data (connection, "being_closed")) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "properly connection close");
			__vortex_connection_set_not_connected (connection, "connection properly closed");
			return NULL;
		}

		// check if we have a non-blocking connection
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "client have disconnected without closing properly this session");
		__vortex_connection_set_not_connected (connection, "client have disconnected without closing session");
		return NULL;
	}
	if (bytes_readed == -1) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "an error have ocurred while reading socket");
		__vortex_connection_set_not_connected (connection, "client have disconnected without closing session");
		return NULL;
	}

	if ((line[bytes_readed - 1] != '\n') || (line[bytes_readed - 2] != '\r')) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "no line definition found for frame, bytes readed: %d, line: \n'%s'\n,closing session",
		       bytes_readed, line);
		__vortex_connection_set_not_connected (connection, "no line definition found for frame");
		return NULL;
	}

	// create a frame
	frame = g_new0 (VortexFrame, 1);

	// check initial frame spec
	frame->type = VORTEX_FRAME_TYPE_UNKNOWN;
	if (!g_strncasecmp (line, "MSG", 3))
		frame->type = VORTEX_FRAME_TYPE_MSG;
	if (!g_strncasecmp (line, "RPY", 3))
		frame->type = VORTEX_FRAME_TYPE_RPY;
	if (!g_strncasecmp (line, "ANS", 3))
		frame->type = VORTEX_FRAME_TYPE_ANS;
	if (!g_strncasecmp (line, "ERR", 3))
		frame->type = VORTEX_FRAME_TYPE_ERR;
	if (!g_strncasecmp (line, "NUL", 3))
		frame->type = VORTEX_FRAME_TYPE_NUL;

	if (frame->type == VORTEX_FRAME_TYPE_UNKNOWN) {
		g_free (frame);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "poorly-formed frame: message type not defined");
		__vortex_connection_set_not_connected (connection, "poorly-formed frame: message type not defined");
		return NULL;
	}

	// parse initial header
	if (sscanf (&(line[3]), "%d %d %c %u %d %d\r\n", 
		    &frame->channel, 
		    &frame->msgno, 
		    &frame->more_char, 
		    &frame->seqno, 
		    &frame->size,
		    &frame->ansno) < 5) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "poorly-formed frame: message values are wrong");
		__vortex_connection_set_not_connected (connection, "poorly-formed frame: message values are wrong");
		return NULL;
	}

	// check more flag
	if (frame->more_char != '.' && frame->more_char != '*') {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "poorly-formed frame: more char is wrong");
		g_free (frame);
		__vortex_connection_set_not_connected (connection, "poorly-formed frame: more char is wrong");
		return NULL;
	}
	// set more flag
	if (frame->more_char == '.')
		frame->more = FALSE;
	else
		frame->more = TRUE;

	// check incoming frame size
	if (frame->size >= MAX_BUFFER_SIZE) {
		g_free (frame);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "received an execesive sized frame, closing session");
		__vortex_connection_set_not_connected (connection, "received an execesive sized frame, closing session");
		return NULL;
	}

	// allocate exactly frame->size + 5 bytes
	buffer = g_new0 (gchar, frame->size + 6);
	
	// read the next frame content
	bytes_readed = __vortex_frame_readn (connection, buffer, frame->size + 5);
	if (bytes_readed == 0) {
		g_free (frame);
		g_free (buffer);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "remote peer have closed connection while reading the rest of the frame");
		__vortex_connection_set_not_connected (connection, "remote peer have closed connection while reading the rest of the frame");
		return NULL;
	}

	if (bytes_readed != (frame->size + 5)) {
		// ok, we have received few bytes than expected but
		// this is not wrong. Non-blocking sockets behave this
		// way. What we have to do is to store the frame chunk
		// we already read and the rest of bytes expected. 
		// 
		// Later on, next frame_get_next calls on the same
		// connection will return the rest of frame to be read.

	save_buffer:
		// save current frame
		vortex_connection_set_data (connection, "frame", frame);
		
		// save current buffer read
		vortex_connection_set_data (connection, "buffer", buffer);
		
		// save remaining bytes
		vortex_connection_set_data (connection, "remaining_bytes", 
					    GINT_TO_POINTER ((frame->size + 5) - bytes_readed));

		// save read bytes
		vortex_connection_set_data (connection, "bytes_read", 
					    GINT_TO_POINTER (bytes_readed));

		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		       "received a frame fragment (expected: %d readed: %d remaning: %d), storing into this connection id=%d",
		       (frame->size + 5), bytes_readed, (frame->size + 5) - bytes_readed, vortex_connection_get_id (connection));
		
		return NULL;
	}

process_buffer:

	// check frame have ended
	if (g_strcasecmp (&buffer[bytes_readed - 5], "END\r\n")) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "poorly formed frame: frame trailer CR LF not found, discarding content");
		g_free (buffer);
		g_free (frame);
		__vortex_connection_set_not_connected (connection, "poorly formed frame: frame trailer CR LF not found, discarding content");
		return NULL;
	}
	
	// get if frame have a valid content type spec, get actual content type
	iterator = 0;
	if (!g_strncasecmp (buffer, "Content-Type: ", 14)) {

		while (buffer[(iterator + 14)] != '\r')
			iterator++;

		frame->content_type = g_new0 (gchar, iterator + 1 );
		memcpy (frame->content_type, &buffer[14], iterator);
		frame->content_type[iterator] = 0;

		// update iterator index
		iterator += 18;
	}else
		frame->content_type = NULL;

	// locate body frame init
	body_init                   = iterator;
	body_size                   = frame->size - iterator;

	// allocate memory from frame payload and limit it
	frame->payload              = g_new0 (gchar, body_size + 1);

	// copy content
	memcpy (frame->payload, &buffer[body_init], body_size);
	// bcopy (&buffer[body_init], frame->payload, body_size);

	// free allocated buffer
	g_free (buffer);

	// log frame on channel received
	if (vortex_log_is_enabled ())
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Frame received on channel %d", frame->channel);

// content-type: %s\npayload:%s\n",
//	       frame->channel, 
//	       frame->content_type ? frame->content_type : "none", 
//	       frame->payload);

	return frame;

}


/** 
 * @internal
 * 
 * Sends data over the given connection
 * 
 * @param connection 
 * @param a_frame 
 * @param frame_size 
 * 
 * @return 
 */
gboolean          vortex_frame_send_raw     (VortexConnection * connection, gchar * a_frame, gint frame_size)
{
	gboolean result = TRUE;
	gint     socket = vortex_connection_get_socket (connection);
	gint     bytes  = 0;
	gchar  * error_msg;

	g_return_val_if_fail (connection, FALSE);
	g_return_val_if_fail (vortex_connection_is_ok (connection, FALSE), FALSE);
	g_return_val_if_fail (a_frame && (* a_frame), FALSE);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "writing raw on socket %d",  socket);
 again:

	if ((bytes = send (socket, a_frame, frame_size, 0)) == VORTEX_SOCKET_ERROR) {
		if (errno == EINTR)
			goto again;
		if (errno == EWOULDBLOCK) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to write data to socket, socket not prepare to write");
			goto end;
		}
		
		// check if socket have been disconnected (macro definition at vortex.h)
		if (vortex_is_disconnected) {
			__vortex_connection_set_not_connected (connection, 
							       "remote peer have closed connection");
			return FALSE;
		}
		error_msg = vortex_errno_get_last_error ();
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to write data to socket: %s",
		       error_msg ? error_msg : "");
		__vortex_connection_set_not_connected (connection, "unable to write data to socket:");
		return FALSE;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "bytes written: %d", bytes);

	if (bytes == 0) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
		       "remote peer have closed before sending proper close connection, closing");
		__vortex_connection_set_not_connected (connection, 
						       "remote peer have closed before sending proper close connection, closing");
		return FALSE;
	}

	if (bytes != frame_size) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "write request missmatch with write done (%d != %d)",
		       bytes, frame_size);
		return FALSE;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "write on socket request=%d writed=%d", frame_size, bytes);
	
 end:
	return result;
}


/**
 * @brief fress the frame.
 *
 * Frees a allocated VortexFrame
 *
 * @param frame the frame to free
 **/
void          vortex_frame_free (VortexFrame * frame)
{
	if (frame == NULL)
		return;

	// free content type;
	if (frame->content_type != NULL)
		g_free (frame->content_type);
	if (frame->payload != NULL)
		g_free (frame->payload);
	g_free (frame);
	return;
}

/**
 * @internal
 * 
 * Joins the frame a with b. This is done by concatenating frame a
 * payload followed by frame b payload and adding both payload size,
 * both seqno number.  It also checks the more flag for both frames to
 * if it detect some error it will return NULL.  Frame types for both
 * are also checked so if frame type differs a NULL is
 * returned. Channel number for both frames are also checked.
 * 
 * @param a
 * @param b
 * 
 * @return the frame joined or NULL if fails. Returned value is
 * newly allocated.
 **/
VortexFrame * vortex_frame_join         (VortexFrame * a, VortexFrame * b)
{
	gchar       * __payload;
	VortexFrame * result;

	if (a->type != b->type)
		return NULL;
	if (!a->more && !b->more)
		return NULL;
	if (!a->more && b->more)
		return NULL;
	if (a->channel != b->channel)
		return NULL;
	if (a->msgno != b->msgno)
		return NULL;
	if ((a->seqno + a->size) != b->seqno)
		return NULL;
	
	__payload = g_new0 (gchar, a->size + b->size + 1);
	// bcopy (a->payload, __payload, a->size);
	memcpy (__payload, a->payload, a->size);
	// bcopy (b->payload, __payload + a->size, b->size);
	memcpy (__payload + a->size, b->payload, b->size);
	
	result          = g_new0 (VortexFrame, 1);
	result->type    = a->type;
	result->channel = a->channel;
	result->msgno   = a->msgno;
	result->more    = (a->more && b->more);
	result->seqno   = a->seqno;
	result->size    = a->size + b->size;
	result->ansno   = a->ansno;
	result->payload = __payload;

	return result;
	
}

/**
 * @brief return actual frame type.
 *
 * 
 * Returns actual frame type.
 *
 * @param frame 
 * 
 * @return the actual frame type or -1 if fail
 *
 **/
VortexFrameType vortex_frame_get_type   (VortexFrame * frame)
{
	g_return_val_if_fail (frame, -1);
	
	return frame->type;
}

/**
 * @brief Returns frame content type.
 * 
 * Return actual frame's conent type. Return value can be a NULL value
 * which means this frame has no content type defined.  You must not
 * free returned value.
 *
 * @param frame
 *
 * @return the content type if defined or NULL if not. On error, NULL
 * also is returned. Only one error is check, if frame is not NULL. If
 * you don't want to fall in the same case of having a correct frame
 * with no content type and having a wrong frame (NULL value) you
 * should check if frame is not NULL first.
 **/
gchar       * vortex_frame_get_content_type (VortexFrame * frame)
{
	g_return_val_if_fail (frame, NULL);

	return frame->content_type;
}

/**
 * @brief Returns current channel used to sent or receive the given frame.
 * 
 * Returns actual frame's channel number.
 *
 * @param frame
 *
 * @return the channel number or -1 if fails
 **/
gint          vortex_frame_get_channel  (VortexFrame * frame)
{
	g_return_val_if_fail (frame, -1);

	return frame->channel;
}

/**
 * @brief Return current message number used for the given frame
 *
 * Returns actual frame's msgno.
 * 
 * @param frame
 * 
 * @return the msgno or -1 if fail.
 **/
gint          vortex_frame_get_msgno    (VortexFrame * frame)
{
	g_return_val_if_fail (frame, -1);

	return frame->msgno;
}

/**
 * @brief Returns actual more flag status for the given frame.
 * 
 * Returns actual frame more flag status. If more flag is activated 1
 * will be returned.  In case of been deactivated 0 will be returned.
 *
 * @param frame
 *
 * Return value: the actual more flag status or -1 if fails
 **/
gint          vortex_frame_get_more_flag (VortexFrame * frame)
{
	g_return_val_if_fail (frame, -1);

	if (frame->more)
		return 1;
	else
		return 0;
}

/**
 * @brief Returns the current sequence number for the given frame.
 * 
 * Returns the actual frame's seqno.
 * 
 * @param frame
 *
 * @return the frame seqno or -1 if fails
 **/
gint          vortex_frame_get_seqno    (VortexFrame * frame)
{
	g_return_val_if_fail (frame, -1);

	return frame->seqno;
}

/**
 * @brief Returns the payload the given frame have.
 * 
 * Return actual frame payload. You must not free returned payload
 *
 * @param frame
 * 
 * @return the actual frame's payload or NULL if fail.
 **/
gchar      *  vortex_frame_get_payload  (VortexFrame * frame)
{
	g_return_val_if_fail (frame, NULL);

	return frame->payload;
}

/**
 * @brief Returns current ans number for the given frame.
 * 
 * Returns actual frame ansno.
 *
 * @param frame
 *
 * @return the ansno value or -1 if fails
 **/
gint          vortex_frame_get_ansno    (VortexFrame * frame)
{
	g_return_val_if_fail (frame, -1);

	return frame->ansno;
}


/**
 * @brief Returns the current payload size the given frame have.

 * 
 * Return frame's payload size.
 * 
 * @param frame
 * @return the actual payload size or -1 if fail
 **/
gint   vortex_frame_get_payload_size (VortexFrame * frame)
{
	g_return_val_if_fail (frame, -1);

	return frame->size;

}

//@}
