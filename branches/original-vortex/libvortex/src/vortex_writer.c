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
//  The vortex writer thread
#include <vortex.h>


#define LOG_DOMAIN "vortex-writer"

GAsyncQueue * vortex_writer_queue = NULL;
GList       * con_writer_list     = NULL;

extern VortexSemaphore * vortex_sequencer_sem;


/**
 * vortex_writer_watch_connection:
 * @connection: 
 * 
 * Allows to register a new connection to watch inside the vortex writer process so,
 * frames queued on channels over that connection are sent.
 * This function is not useful for vortex library consumer. It is for internal vortex
 * library purposes.
 **/
void vortex_writer_watch_connection (VortexConnection * connection)
{
	g_return_if_fail (vortex_connection_is_ok (connection, FALSE));
	
	// increase reference counting
	vortex_connection_ref (connection, "vortex writer (process)");

	g_async_queue_push (vortex_writer_queue, connection);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "new connection on writer to be watched (%d)",
	       vortex_connection_get_socket (connection));


	return;
			  
}

gint __vortex_writer_build_set_to_watch (fd_set * on_writing)
{
	GList            * list_aux     = g_list_first (con_writer_list);
	GList            * new_list     = NULL;
	gint               max_fds      = 0;
	gint               fds          = 0;
	VortexConnection * connection;
	gint               pending_msgs = 0;

	// read connection list
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		connection = list_aux->data;
		if (!vortex_connection_is_ok (connection, FALSE)) {
			// ensure we are not unrefering a vortex
			// connection with pending frames to be
			// sent. If we found message pending on this
			// connection decrease the semaphore.
			pending_msgs = vortex_connection_get_pending_msgs (connection);
			if (pending_msgs > 0) {
				g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "vortex writing is unrefering a connection is pending messages=%d, decreasing vortex sequencer semaphore %d units",
				       pending_msgs,
				       pending_msgs);
				vortex_semaphore_down_n (vortex_sequencer_sem, pending_msgs);
			}
			vortex_connection_unref (connection, "vortex writer (process)");

			continue;
		}

		fds        = vortex_connection_get_socket (connection);
		max_fds    = MAX (fds, max_fds);
		FD_SET (fds, on_writing);
 		new_list   = g_list_append (new_list, connection);
	}

	// update connection list
	g_list_free (con_writer_list);
	con_writer_list = g_list_first (new_list);

	// connection list watch
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "connection watched by the writer");
	list_aux        = g_list_first (con_writer_list);
	for (;list_aux; list_aux = g_list_next (list_aux)) {
		connection = list_aux->data;
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "   connection to id:%d) %s:%s", 
		       vortex_connection_get_id   (connection),
		       vortex_connection_get_host (connection),
		       vortex_connection_get_port (connection));
	}

	return max_fds;
}

void __vortex_writer_watch_first_connection ()
{
	VortexConnection * connection;

	// wait for the first connection to watch
	connection      = g_async_queue_pop (vortex_writer_queue);

	// register new connection in list
	con_writer_list = g_list_append (con_writer_list, connection);

	return;
}

void __vortex_writer_read_pending ()
{
	VortexConnection * connection;
	gint               length;
	gint               pending_msgs;

	con_writer_list = g_list_first (con_writer_list);

	// reading pending connection
	length = g_async_queue_length (vortex_writer_queue);
	while (length > 0) {
		
		length--;
		connection = g_async_queue_pop (vortex_writer_queue);
		if (!vortex_connection_is_ok (connection, FALSE)) {
			// ensure we are not unrefering a vortex
			// connection with pending frames to be
			// sent. If we found message pending on this
			// connection decrease the semaphore.
			pending_msgs = vortex_connection_get_pending_msgs (connection);
			if (pending_msgs > 0) {
				g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "vortex writing is unrefering a connection is pending messages=%d, decreasing vortex sequencer semaphore %d units",
				       pending_msgs,
				       pending_msgs);
				vortex_semaphore_down_n (vortex_sequencer_sem, pending_msgs);
			}

			vortex_connection_unref (connection, "vortex writer (process)");
			continue;
		}

		// register the connection
		con_writer_list = g_list_append (con_writer_list, connection);
	}


	return;
}

gpointer __vortex_writer_run (gpointer __data)
{
	VortexConnection * connection;
	gint               fds;
	gint               msgs;
	gint               status;
	fd_set             on_writing;
	gint               max_fds;
	gint               result;
	GList            * list_aux;
	gchar            * error_msg;
	struct timeval     tv;

 first_con:
	// wait for the first connection to watch
	__vortex_writer_watch_first_connection ();

	while (TRUE) {
		// check if we have nothing to watch
		if (g_list_length (con_writer_list) == 0) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "no more connection to watch on vortex writer");
			goto first_con;
		}

		// wait until sequencer have queued some data
		vortex_semaphore_wait (vortex_sequencer_sem);

		// read the pending queue elements to be watched. Read
		// new connections to be managed
		__vortex_writer_read_pending ();

		// clear set
		FD_ZERO (&on_writing);
		
		// build list connection to watch
		max_fds = __vortex_writer_build_set_to_watch (&on_writing);

		// wait until something have changed and init timeout
		tv.tv_sec  = 2;
		tv.tv_usec = 0;
		result = select (max_fds + 1, NULL, &on_writing, NULL, &tv);

		// check result
		if ((result == VORTEX_SOCKET_ERROR) && (errno == EINTR))
			continue;

		if (result == -1) {
			error_msg = vortex_errno_get_last_error ();
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "an error have happen while waiting to write: %s",
			       error_msg ? error_msg : "");
		}

		if (result == 0) {
			// waiting timeout have occur, no descriptor have change, its time to check 
			// the queue
			continue;
		}

		// we can write, for each connection check if we can write
		list_aux = g_list_first (con_writer_list);
		for (; list_aux; list_aux = g_list_next (list_aux)) {
			// check if connection is ok
			connection = list_aux->data;
			if (!vortex_connection_is_ok (connection, FALSE))
				continue;

			fds  = vortex_connection_get_socket (connection);

			if (FD_ISSET (fds, &on_writing)) {
				msgs = vortex_connection_do_a_sending_round (connection);
				// if no message was sent, do not descrease anything
				if (msgs == 0) {
					status = vortex_semaphore_get_status (vortex_sequencer_sem);
					if (status == 0) {
						g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
						       "pending frames to be sent ok, writer is going to take a \"siesta\" now");
						break;
					}

					g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
					       "no message sent for connection id=%d but semaphore status signal there are %d message waiting",
					       vortex_connection_get_id (connection),
					       status);
					continue;
				}
				
				// decrease the semaphore for each message sent
				vortex_semaphore_down_n (vortex_sequencer_sem, msgs);
				
			} /* end if */

		} /* end for */

	} /* vortex writer while loop end */

	return NULL;
}

/**
 * vortex_writer_run:
 * 
 * Launches the vortex writer. The vortex writer in conjuntion with vortex sequencer are the processes
 * that actually make posible to send frames. Once the vortex sequencer have prepared the frames to
 * be sent and queue them into each channel queue, the vortex writer takes waits for that frames
 * to send them in a round robin manner. 
 **/
void vortex_writer_run ()
{

	// the vortex writer queue: this queue is used to register new connection to be watched
	// in order to send frames
	vortex_writer_queue = g_async_queue_new ();
	
	// launch the vortex writer thread
	g_thread_create (__vortex_writer_run, NULL, FALSE, NULL);
}


/** 
 * @internal
 * @brief frees Vortex Writer Data
 * 
 * @param writer_data 
 */
void vortex_writer_data_free (VortexWriterData * writer_data)
{
	g_return_if_fail (writer_data);
	
	g_free (writer_data->the_frame);
	g_free (writer_data);

	return;
}
