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

#define LOG_DOMAIN "vortex-sequencer"

GAsyncQueue            * vortex_sequencer_queue;
VortexSemaphore        * vortex_sequencer_sem;
gchar                  * buffer                 = NULL;

void vortex_sequencer_queue_data (VortexSequencerData * data)
{
	g_return_if_fail (data);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "new message to be sent: msgno %d, channel %d (size: %d):\n%s",
	       data->msg_no, 
	       data->channel_num, 
	       data->message_size,
	       data->message);

	g_async_queue_push (vortex_sequencer_queue, data);

	return;
}

gpointer __vortex_sequencer_run (gpointer __data)
{
	VortexSequencerData * data;
	VortexWriterData    * packet;
	VortexChannel       * channel;
	gint                  next_seq_no;
	gint                  first_seq_no;
	gint                  message_size;
	gint                  steps                  = 0;
	gint                  window_size            = 0;
	gint                  previous_window_size   = 0;
	gint                  size_to_copy;
	gchar               * a_frame;

	while (TRUE) {
		// block until receive a new message to be sent
		data    = g_async_queue_pop (vortex_sequencer_queue);

		//new message to be sent
		channel = data->channel;
		if (data->channel_num != vortex_channel_get_number (channel)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "channel number for frame sending missmatch, dropping this frame");
			goto process_end;
		}
			

		// create the frame (or frames to send) (splitter process)
		first_seq_no = data->first_seq_no;
		message_size = data->message_size;
		next_seq_no  = first_seq_no;
		window_size  = vortex_channel_get_window_size (channel);

		// prepare buffer_size to create frame chunks
		// this buffer will be used to copy (allocate the window size plus and \0 char ending)
		if ((previous_window_size != window_size) && (buffer != NULL)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing vortex sequencer due to channel window size changing.. ");
			g_free (buffer);
			buffer = NULL;
		}

		if (buffer == NULL) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "allocating %d bytes for vortex sequencer buffer",
			       window_size + 1);
			buffer = g_new0 (gchar, window_size + 1);
		}
		steps  = 0;
		while (next_seq_no < (first_seq_no + message_size)) {
			// calculate how many bytes to copy from the payload according to window_size
			if (((first_seq_no + message_size) - next_seq_no) > window_size)
				size_to_copy = window_size;
			else
				size_to_copy = (first_seq_no + message_size) - next_seq_no;
			
			// clear window buffer
			// bzero (buffer, window_size + 1);
			memset (buffer, 0, window_size + 1);
			
			// copy data
			// bcopy (data->message + steps, buffer, size_to_copy);
			memcpy (buffer, data->message + steps, size_to_copy);

			// we have the payload on buffer
			a_frame = vortex_frame_build_up_from_params (data->type,
								     data->channel_num,
								     data->msg_no,
								     ((size_to_copy == window_size) && 
								      (message_size != (next_seq_no - first_seq_no + size_to_copy))) ? TRUE : FALSE,
								     next_seq_no,
								     size_to_copy,
								     0,
								     buffer);

			// queue frame ready to be sent into channels queue
			packet            = g_new0 (VortexWriterData, 1);
			packet->type      = data->type;
			packet->msg_no    = data->msg_no;
			packet->the_frame = a_frame;
			packet->the_size  = strlen (a_frame);
			vortex_channel_queue_frame (channel, packet);

			// increase semaphore (one unit for each frame queued)
			vortex_semaphore_up (vortex_sequencer_sem);

			// update indexes
			next_seq_no += size_to_copy;
			steps       += size_to_copy;

			// do not free packet, vortex writer will do it for us.
		}

		// record previous window size so we have to not free memory
		previous_window_size = window_size;

		// free data no longer needed
	process_end:
		g_free (data->message);
		g_free (data);
		
	}
	
	// that's all vortex sequencer process can do
	return NULL;
}

/**
 * vortex_sequencer_run:
 * 
 * Starts the vortex sequencer process. This process with the vortex
 * writer process conforms the subsystem which actually send a message
 * inside vortex. While vortex reader process threats all incoming
 * message and dispatch them to apropiate destination, the vortex
 * sequencer waits for messages to be send over channels.
 *
 * Once the vortex sequencer receive a petition to send a message, it
 * checks if actual channel window size is apropiated to actual
 * message being sent. If not, the vortex sequencer splits the message
 * into many pieces until all pieces can be sent using the actual
 * channel window size.
 *
 * Once the message is segmented, the vortex sequencer build up the
 * frames and enqueue the messages inside the channel waiting
 * queue. Once the sequencer have queue the first frame to be sent, it
 * signal the vortex writer to starting to send messages.
 *
 *
 **/
void vortex_sequencer_run ()
{
	vortex_sequencer_queue = g_async_queue_new ();
	vortex_sequencer_sem   = vortex_semaphore_new ();

	g_thread_create ((GThreadFunc) __vortex_sequencer_run, NULL, FALSE, NULL);

	return;
}

/** 
 * @internal
 * @brief Stop vortex sequencer process.
 */
void vortex_sequencer_stop ()
{

	if (buffer != NULL) {
		g_free (buffer); 
		buffer = NULL;
	}
	
	return;
}



