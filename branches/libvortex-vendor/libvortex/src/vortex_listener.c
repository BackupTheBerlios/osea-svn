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
#define LOG_DOMAIN "vortex-listener"

GAsyncQueue * vortex_listener_wait_lock = NULL;

// really bad trick to be able to set connected state to a channel
// this is needed for channel 0 connections and all channels 
extern void __vortex_channel_set_connected (VortexChannel * channel);

typedef struct _VortexListener {
	gchar      * host;
	gchar      * port;
	GHashTable * channels;

}VortexListener;


gint __vorter_listener_get_port (gchar * port)
{
	return strtol (port, NULL, 10);
}

/**
 * \defgroup vortex_listener Vortex Listener: Related function to create Vortex Listener (server for accepting incoming petitions)
 */

/// \addtogroup vortex_listener
//@{

/** 
 * @internal
 *
 * Internal vortex library function. This function does the initial
 * accept for a new incoming connection. New connections are accepted
 * throught two steps: an initial accept and final negociation. A
 * connection to be totally accepted must step over two previous
 * steps.
 *
 * The reason to vortex accept connections this way is because its way
 * of reading data from all connections. This is done inside the
 * vortex reader which, basicly is a loop executing a "select"
 * call. While reading data from those sockets accepted by the
 * "select" call is not a problem it is a problem to accept new
 * connections because implies reading data from remote peer: the
 * initial greeting and writing data to remote peer: the initial
 * greeting response. During the previous negociation a malicious
 * client can make negociation to be stopped making the select loop to
 * be stoped. As a consecuence this malicious client have thrown down
 * the reception for all channels inside all connections. 
 *
 * However, the vortex reader loop is prepared to avoid this problem
 * with already accepted connections because it doesn't pay attention
 * to those connection which are not sending any data and also support
 * to receive fragmented frames which are stored to be joined with
 * future fragment. Once connection is accepted the vortex reader is
 * strong enough to avoid DOS (denail of service) attacks.
 *
 * That way the mission for the first step: to only accept the new
 * connection and send the initial greeting to remote peer and *DO NOT
 * READING ANYTHING* to avoid DOS. On a second step, the response
 * reading is done and the connection is totally accepted in the
 * context of the vortex reader.
 *
 * A connection initial accepted is flaged to be on that step so
 * vortex reader can recognize it. 
 *
 * @param client_socket 
 */
void __vortex_listener_initial_accept (gint client_socket)
{
	VortexConnection     * connection = NULL;

	// before doing anything, we have to create a connection
	connection = vortex_connection_new_empty (client_socket);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "received connection from: %s:%s", 
	       vortex_connection_get_host (connection),
	       vortex_connection_get_port (connection));

	// activate vortex writer for this connection because all sending process is
	// done throught vortex sequencer plus vortex writer
	vortex_writer_watch_connection      (connection);

	// send greetings, get actual profile installation and report it to init peer
	if (! vortex_greetings_send (connection)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to send initial greeting message");
		vortex_connection_unref (connection, "vortex listener");
		return;
	}

	// register the connection on vortex reader
	// from here, the connection get a non-blocking state
	vortex_reader_watch_connection      (connection);

	// flag the connection to be on initial step
	vortex_connection_set_data (connection, "initial_accept", GINT_TO_POINTER (TRUE));

	// because this is a listener, we don't want to pay attention to free 
	// connection on errors. connection already have 2 references (reader and writer),
	// so let's reference counting to de job of free connection resources
	vortex_connection_unref (connection, "vortex listener (initial accept)");

	// close connection and free resources
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "worker ended, connection registered on manager (initial accept)");

	return;
}

/** 
 * @internal
 *
 * This function is for internal vortex library purposes. This
 * function actually does the second accept step, that is, to read the
 * greeting response and finally accept the conection is that response
 * is ok.
 *
 * You can also read the doc for __vortex_listener_initial_accept to
 * get an idea about the initial step.
 *
 * Once the greeting response is ok, the function unflag this
 * connection to be "being accepted" so the connection starts to work.
 * 
 * @param frame The frame which should contains the greeting response
 * @param connection the connection being on initial accept step
 */
void __vortex_listener_second_step_accept (VortexFrame * frame, VortexConnection * connection)
{

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "greetings sent, waiting reply");

	// process greetings from init peer
	
	if (!vortex_greetings_is_reply_ok (frame, connection)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "wrong greeting rpy from init peer, closing session");
		vortex_connection_unref (connection, "vortex listener");
		return;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "accepting connection on vortex_reader (second accept step)");
	
	// free the last frame and watch connection on changes
	vortex_frame_free (frame);
	
	// flag the connection to be totally accepted.
	vortex_connection_set_data (connection, "initial_accept", NULL);

	return;	
}



void vortex_listener_accept_connections (gint server_socket)
{
	struct sockaddr_in inet_addr;
	gint               addrlen;
	gint               client_socket;

	addrlen       = sizeof(struct sockaddr_in);
	// accept the connection new connection
	client_socket = accept(server_socket, (struct sockaddr *)&inet_addr, &addrlen);
	if (client_socket == VORTEX_SOCKET_ERROR) {
		g_warning ("accept () failed\n");
		return;
	}


	// instead of negociate the connection at this point simply accept it to
	// negociate it inside vortex_reader loop. (Of course, as RFC3080 says, a listener
	// must send immediatly the initial greetings
	__vortex_listener_initial_accept (client_socket);

	return;
}

typedef struct _VortexListenerData {
	gchar               * host;
	gchar               * port;
	VortexListenerReady   on_ready;
	gpointer              user_data;
	gboolean              threaded;
}VortexListenerData;

gpointer __vortex_listener_new (VortexListenerData * data)
{
	struct hostent     * he;
        struct in_addr     * haddr;
        struct sockaddr_in   saddr;
	struct sockaddr_in   sin;
	gint                 sin_size  = sizeof (sin);
	gint                 fd;
#ifdef G_OS_WIN32
	BOOL                 unit      = TRUE;
#else    	
	gint                 unit      = 1;
#endif	
	gchar              * host      = data->host;
	gchar              * port      = data->port;
	gint                 int_port  = __vorter_listener_get_port (port);
	gboolean             threaded  = data->threaded;
	VortexListenerReady  on_ready  = data->on_ready;
	gpointer             user_data = data->user_data;
	gchar              * message   = NULL;
	
	// free data
	g_free (data);

	if (vortex_profiles_registered () == 0) {
		message = "no registered profile was found, unable to start";
		goto error;
	}
	
	he = gethostbyname (host);
        if (he == NULL) {
		message = "unable to get hostname by calling gethostbyname";
		goto error;
	}

	haddr = ((struct in_addr *) (he->h_addr_list)[0]);
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		message = "unable to create a new socket";
		goto error;
        }

#ifdef G_OS_WIN32
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (gchar *)&unit, sizeof(BOOL));
#else
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &unit, sizeof (unit));
#endif

	memset(&saddr, 0, sizeof(struct sockaddr_in));
	saddr.sin_family          = AF_INET;
        saddr.sin_port            = htons(int_port);
        memcpy(&saddr.sin_addr, haddr, sizeof(struct in_addr));

        if (bind(fd, (struct sockaddr *)&saddr,  sizeof (struct sockaddr_in)) == VORTEX_SOCKET_ERROR) {
		vortex_close_socket (fd);
		message = "unable to bind address";
		goto error;
        }
	if (listen(fd, 5) == VORTEX_SOCKET_ERROR) {
		message = "an error have occur while executing listen";
		goto error;
        }

	// notify listener
	if (getsockname (fd, (struct sockaddr *) &sin, &sin_size) < -1) {
		message = "an error have occur while executing getsockname";
		goto error;
	}

	// __vortex_listener_accept_connections (fd);
	vortex_reader_watch_listener (fd);
		
	if (threaded) {
		on_ready (inet_ntoa(sin.sin_addr), ntohs (sin.sin_port), VortexOk,
			  "server ready for requests", user_data);
	}
	return NULL;

 error:
	if (threaded) {
		on_ready (NULL, 0, VortexError, message, user_data);
	}else {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to start vortex server, error was: %s, unblocking vortex_listener_wait",
		       message);
		g_async_queue_push (vortex_listener_wait_lock, GINT_TO_POINTER (TRUE));
	}
	return NULL;	
}

/** 
 * @brief Creates a new Vortex Listener accepting connection on the given host port configuration.
 *
 * Tries to create a new vortex listener on the given host and port.
 *
 * If user pass in an on_ready callback, it will be notified on it in
 * a separated thread.
 * 
 * On that notification will also be passed the host and port actually
 * allocated. Think about using as host 0.0.0.0 and port 0. 
 * 
 * This values will cause to \ref vortex_listener_new to allocate the
 * system configured hostname and a randomly free port. See
 * \ref vortex_handlers "this section" for more info about on_ready.
 *
 * @param host the host to listen to.
 * @param port the port to listen to.
 * @param on_ready a optional notify callback to get when vortex listener is ready to perform replies.
 * @param user_data a user defined pointer to be passed in to <i>on_ready</i> handler.
 */
void vortex_listener_new (gchar * host, gchar * port, VortexListenerReady on_ready, gpointer user_data)
{

	VortexListenerData * data;
	
	g_return_if_fail (host && (* host));
	g_return_if_fail (port && (* port));
	
	// prepare function data
	data             = g_new0 (VortexListenerData, 1);
	data->host       = host;
	data->port       = port;
	data->on_ready   = on_ready;
	data->user_data  = user_data;
	data->threaded   = (on_ready != NULL);
	
	// make request
	if (data->threaded) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "invoking listener_new threaded mode");
		vortex_thread_pool_new_task ((GThreadFunc) __vortex_listener_new, data);
		return;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "invoking listener_new non-threaded mode");
	__vortex_listener_new (data);

	return;
	
}

/** 
 * @brief Blocks a listener (or listeners) launched until vortex finish.
 * 
 * This function should be called before creating a server calling to
 * \ref vortex_listener_new to block current thread. 
 * 
 * This function can be avoided if the program structure can ensure it
 * will not end after calling \ref vortex_listener_new.
 *
 * This function will be unblocked when the vortex listener created
 * ends or a failure have occur while creating the listener.
 * 
 */
void vortex_listener_wait ()
{
	// double locking to ensure waiting
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Locking listener");
	g_async_queue_pop   (vortex_listener_wait_lock);
	g_async_queue_unref (vortex_listener_wait_lock);
	vortex_listener_wait_lock = NULL;
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "(un)Locked listener");

	return;
}

/**
 * @internal
 * 
 * This function must not be called directly from user space. This
 * function actually unblock waiting listeners on vortex_listener_wait
 * function.
 **/
void vortex_listener_unlock ()
{
	// unlock listener
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "(un)Locking listener done");
	if (vortex_listener_wait_lock != NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "(un)Locking listener: data pushed..");
		g_async_queue_push (vortex_listener_wait_lock, GINT_TO_POINTER (TRUE));
		g_async_queue_unref (vortex_listener_wait_lock);
		return;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "(un)Locking listener: waiting queue not found..");

	return;
}

/**
 * @internal
 * 
 * Internal vortex function. This function allows vortex_init to
 * initialize vortex listener module.
 **/
void vortex_listener_init ()
{
	// init lock
	if (vortex_listener_wait_lock == NULL) {
		vortex_listener_wait_lock = g_async_queue_new ();
		g_async_queue_ref (vortex_listener_wait_lock);
	}

	return;
}

// @}
