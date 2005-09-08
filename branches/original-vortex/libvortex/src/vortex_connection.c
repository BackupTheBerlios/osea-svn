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
#define LOG_DOMAIN "vortex-connection"
#define VORTEX_CONNECTION_BUFFER_SIZE 32768

// a connection identifier (for internal vortex use)
glong                connection_id        = 1;
static GStaticMutex  connection_id_mutex  = G_STATIC_MUTEX_INIT;

gint __vortex_connection_get_next_id ()
{
	gint result;
	g_static_mutex_lock (&connection_id_mutex);
	
	result = connection_id;
	connection_id++;

	g_static_mutex_unlock (&connection_id_mutex);

	return result;
}

// really bad trick to be able to set connected state to a channel
// this is needed for channel 0 connections and all channels 
extern void __vortex_channel_set_connected (VortexChannel * channel);

struct _VortexConnection {
	gint         id;

	gchar      * host;
	gchar      * port;
	gboolean     is_connected;
	gchar      * message;
	gint         session;
	GList      * remote_supported_profiles;
	VortexHash * channels;
	VortexHash * data;
	gint         last_channel;

	// the channel_mutex
	// ~~~~~~~~~~~~~~~~~
	GMutex     * channel_mutex;
	// This mutex is used at vortex_connection_get_next_channel to
	// avoid race condition of various threads calling in a
	// reentrant way on this function getting the same value.
	// 
	// Althought this function use vortex_hash_table, a
	// thread-safe hash table, this is not enough to ensure only
	// one thread can execute inside this function.
	// 
	// The vortex_connection_add_channel adds a channel into a
	// existing connection. Inside the implementation, this
	// function first lookup if channel already exists which means
	// a programm error and the insert the new channel if no
	// channel was found.
	//
	// This mutex is also used on that function to avoid reentrant
	// conditions on the same function and between function which
	// access to channel hash table
	
	// the ref_count
	// ~~~~~~~~~~~~~
	gint ref_count;
	// This enable a vortex connection to keep track of the
	// reference counting.  The referece counting is controled
	// thought the vortex_connection_ref and
	// vortex_connection_unref.

	// the ref_mutex
	// ~~~~~~~~~~~~~
	GMutex  * ref_mutex;
	// This mutex is used at vortex_connection_ref and
	// vortex_connection_unref to avoid race conditions especially
	// at vortex_connection_unref. Because several threads can
	// execute at the same time this unref function this mutex
	// ensure only one thread will execute the vortex_connection_free.

	// the op_mutex
	// ~~~~~~~~~~~~
	GMutex * op_mutex;
	// This mutex allows to avoid race condition on operating with
	// the same connection from different threads.

	// the channel_pool_mutex
	// ~~~~~~~~~~~~~~~~
	GMutex * channel_pool_mutex;
	// This mutex is used by the vortex_channel_pool while making
	// operations with channel pool over this connection. This is
	// necesary to make mutually exclusive the function from that
	// module while using the same connection.

	// the next_channel_pool_id
	// ~~~~~~~~~~~~~~~~~~~~~~~~
	gint next_channel_pool_id;
	// This value is used by the channel pool module to hold the
	// channel pool identifiers that will be used.

	// the channel_pools
	// ~~~~~~~~~~~~~~~~~
	VortexHash * channel_pools;
	// This hash actually holds every channel pool created over
	// this connection. Every channel pool have an id that unique
	// identifies the channel pool from each over inside this
	// connection and it is used as key.
	
};

typedef struct _VortexConnectionNewData {
	VortexConnection    * connection;
	VortexConnectionNew   on_connected;
	gpointer              user_data;
	gboolean              threaded;
}VortexConnectionNewData;


void __vortex_connection_parse_greetings_get_profiles (VortexConnection * connection,  xmlDocPtr document)
{
	xmlNodePtr    cursor;
	gchar       * uri;

	// Get the root element (greetings element)
	cursor = xmlDocGetRootElement (document);

	// Get the position of the first profile element
	cursor = cursor->children;
	while (cursor) {

		// get profiles
		if (!xmlStrcmp (cursor->name, (const xmlChar *) "profile")) {		
			uri = xmlGetProp (cursor, "uri");
			connection->remote_supported_profiles = g_list_append (connection->remote_supported_profiles,
									       uri);
		}
		
		// next 
		cursor = cursor->next;
	}

	return;
}

gboolean __vortex_connection_parse_greetings (VortexConnection * connection, VortexFrame * frame)
{
	xmlValidCtxt    context;
	xmlDocPtr       doc;
	xmlDtdPtr       channel_dtd;

	g_return_val_if_fail (connection, FALSE);
	g_return_val_if_fail (frame,      FALSE);

	if ((channel_dtd = vortex_dtds_get_channel_dtd ()) == NULL) {
		__vortex_connection_set_not_connected (connection, "Cannot find DTD definition for channel management");
		return FALSE;
	}

	// dtd correct
	doc = xmlParseMemory ((xmlChar *) vortex_frame_get_payload (frame), 
			      vortex_frame_get_payload_size (frame)); //* sizeof (char)));
	if (!doc) {
		__vortex_connection_set_not_connected (connection, "Cannot parse xml message to validate it");
		return FALSE;
	}

	// doc correct
	if (xmlValidateDtd (&context, doc, channel_dtd)) {
		__vortex_connection_parse_greetings_get_profiles (connection, doc);
		
		// Validation succesful
		xmlFreeDoc (doc);
		return TRUE;
	}

	// Validation failed
	xmlFreeDoc (doc);
	__vortex_connection_set_not_connected (connection, "Incomming greetings validation failed");
	return FALSE;
}

/**
 * \defgroup vortex_connection Vortex Connection: Related function to create and manage Vortex Connections.
 */

/// \addtogroup vortex_connection
//@{





/** 
 * \internal
 *
 * Internal function used by Vortex library to simulate
 * VortexConnection on listener side.  You should not use this
 * function
 * 
 * 
 * @param session 
 * 
 * @return a VortexConnection
 */
VortexConnection * vortex_connection_new_empty            (gint session)
{
	VortexConnection   * connection;
	VortexChannel      * channel;
	struct sockaddr_in   sin;
	gint                 sin_size = sizeof (sin);
	
	if (getpeername (session, (struct sockaddr *) &sin, &sin_size) < -1) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to get remote hostname and port");
		return NULL;
	}

	connection                     = g_new0   (VortexConnection, 1);
	connection->id                 = __vortex_connection_get_next_id ();
	connection->host               = g_strdup (inet_ntoa (sin.sin_addr));
	connection->port               = g_strdup_printf ("%d", ntohs (sin.sin_port));
	connection->message            = g_strdup ("listener role connection ready");
	connection->session            = session;
	connection->is_connected       = TRUE;
	connection->ref_count          = 1;
	connection->ref_mutex          = g_mutex_new ();
	connection->op_mutex           = g_mutex_new ();
	connection->channel_pool_mutex = g_mutex_new ();
	connection->channels           = vortex_hash_new_full (NULL, NULL, NULL,
							       (GDestroyNotify) vortex_channel_free);
	connection->data               = vortex_hash_new_full (g_str_hash, g_str_equal,
							       NULL, NULL);
	connection->channel_pools      = vortex_hash_new_full (NULL, NULL,
							       NULL, 
							       (GDestroyNotify) vortex_channel_pool_close);
	connection->channel_mutex      = g_mutex_new ();
	
	// create channel 0 (virtually allways is created but, is necessary to have a representation for channel 0,
	// in order to make channel managment function to be consistent).
	channel = vortex_channel_empty_new (0, "not applicable", connection);
	
	// associate channel 0 with actual connection
	vortex_connection_add_channel (connection, channel);


	return connection;
}


/** 
 * \brief Allows to change connection semantic to blocking.
 *
 * This function should not be useful for Vortex Library consumers
 * because the internal Vortex Implementation requires connections to
 * be non-blocking semanthic.
 * 
 * @param connection the connection to set as blocking
 * 
 * @return TRUE if blocking state was set or FALSE if not.
 */
gboolean vortex_connection_set_blocking_socket (VortexConnection    * connection)
{
#ifdef G_OS_UNIX
	gint flags;
#endif

	g_return_val_if_fail (connection, FALSE);
	g_return_val_if_fail (connection->is_connected, FALSE);
	
#ifdef G_OS_WIN32
	if (!vortex_win32_blocking_enable (connection->session)) {
		__vortex_connection_set_not_connected (connection, "unable to set blocking I/O");
		return FALSE;
	}
#else
	if ((flags = fcntl (connection->session, F_GETFL, 0)) < -1) {
		__vortex_connection_set_not_connected (connection, 
						       "unable to get socket flags to set non-blocking I/O");
		return FALSE;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "actual flags state before setting blocking: %d", flags);
	flags &= ~O_NONBLOCK;
	if (fcntl (connection->session, F_SETFL, flags) < -1) {
		__vortex_connection_set_not_connected (connection, "unable to set non-blocking I/O");
		return FALSE;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "actual flags state after setting blocking: %d", flags);
#endif
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "setting connection as blocking");
	return TRUE;
}

/** 
 * \brief Allows to change connection semantic to nonblocking.
 *
 * Sets a connection to be non-blocking while sending and receiving
 * data. This function should not be useful for Vortex Library
 * consumers.
 * 
 * @param connection the connection to set as nonblocking.
 * 
 * @return TRUE if nonblocking state was set or FALSE if not.
 */
gboolean vortex_connection_set_nonblocking_socket (VortexConnection * connection)
{
#ifdef G_OS_UNIX
	gint flags;
#endif
	g_return_val_if_fail (connection, FALSE);
	g_return_val_if_fail (connection->is_connected, FALSE);

#ifdef G_OS_WIN32
	if (!vortex_win32_nonblocking_enable (connection->session)) {
		__vortex_connection_set_not_connected (connection, "unable to set non-blocking I/O");
		return FALSE;
	}
#else
	if ((flags = fcntl (connection->session, F_GETFL, 0)) < -1) {
		__vortex_connection_set_not_connected (connection, "unable to get socket flags to set non-blocking I/O");
		return FALSE;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "actual flags state before setting nonblocking: %d", flags);
	flags |= O_NONBLOCK;
	if (fcntl (connection->session, F_SETFL, flags) < -1) {
		__vortex_connection_set_not_connected (connection, "unable to set non-blocking I/O");
		return FALSE;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "actual flags state after setting nonblocking: %d", flags);
#endif
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "setting connection as non-blocking");
	return TRUE;
}


/** 
 * \internal Support function to vortex_connection_new. 
 *
 * This function actually does the work for the vortex_connection_new.
 * 
 * @param data To perform vortex connection creation process
 * 
 * @return on thread model NULL on non-thread model the connection
 * created (connected or not connected). 
 */
gpointer __vortex_connection_new (VortexConnectionNewData * data)
{
	VortexConnection   * connection = data->connection;
	struct hostent     * he;
        struct in_addr     * haddr;
        struct sockaddr_in   saddr;
	VortexFrame        * frame;
	VortexChannel      * channel;


	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "executing connection new in %s mode to %s:%s id=%d",
	       (data->threaded == TRUE) ? "thread" : "blocking", 
	       connection->host, connection->port,
	       connection->id);

	// standard tcp socket voodo connection (I would like to know who was the great mind designer of this
	// api) 
        he = gethostbyname(connection->host);
        if (he == NULL) {
		connection->message      = g_strdup ("unable to get host name by using gethostbyname");
		goto __vortex_connection_new_finalize;
	}

        haddr               = ((struct in_addr *) (he->h_addr_list)[0]);
        connection->session = socket(AF_INET, SOCK_STREAM, 0);
	
	if (connection->session == VORTEX_INVALID_SOCKET) {

		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to create socket");
		connection->message = g_strdup ("unable to create socket");
		goto __vortex_connection_new_finalize;
	}

        memset(&saddr, 0, sizeof(saddr));
        memcpy(&saddr.sin_addr, haddr, sizeof(struct in_addr));

        saddr.sin_family    = AF_INET;
        saddr.sin_port      = htons(g_ascii_strtod (connection->port, NULL));

        if (connect(connection->session, 
		    (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
                    
      
		shutdown (connection->session, SHUT_RDWR);
		vortex_close_socket (connection->session);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unable to connect to remote host");
		connection->message = g_strdup ("unable to connect to remote host");
		goto __vortex_connection_new_finalize;
	}

	connection->is_connected = TRUE;

	// activate vortex writer for this connection because all
	// sending process is done throught vortex sequencer plus
	// vortex writer
	vortex_writer_watch_connection      (connection);
	goto __vortex_connection_new_invoke;

 __vortex_connection_new_finalize:
	// common case when everyrthing goes wrong
	connection->is_connected = FALSE;
	

 __vortex_connection_new_invoke:

	if (connection->is_connected) {

		// create channel 0 (virtually allways is created but,
		// is necessary to have a representation for channel
		// 0, in order to make channel managment function to
		// be consistent).
		channel = vortex_channel_empty_new (0, "not applicable", connection);
		vortex_connection_add_channel  (connection, channel);

		// now we have to send greetings and process them
		if (! vortex_greetings_client_send (connection)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, vortex_connection_get_message (connection));
			goto __vortex_connection_new_invoke_caller;
		}
		
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "greetings sent, waiting for reply");

		
		// block thread until received remote greetings
		frame = vortex_greetings_client_process (connection);

		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "greetings received, process reply frame");

		// process frame response
		if (frame != NULL) {
			// now, we have to read remote site supported profiles
			if (!__vortex_connection_parse_greetings (connection, frame)) {
				// something wrong have happend while parsing XML greetings
				goto __vortex_connection_new_finalize;
			}
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "greetings parsed..");
			
			// parse ok, free frame and establish new message
			vortex_frame_free (frame);
			connection->message = g_strdup ("session established and ready");
			
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "new connection created to %s:%s", connection->host, connection->port);
			
			// now, every initial message have been sent, we need to send
			// this to the reader manager
			vortex_reader_watch_connection (connection);
		}

	}
 __vortex_connection_new_invoke_caller: 
	// notify on callback or simply return
	if (data->threaded) {
		data->on_connected (connection, data->user_data);
		g_free (data);			
		return NULL;
	}
	g_free (data);
	return connection;
}

/** 
 * \brief Allows you to create a new beep session (or connection).
 *
 *
 * Creates a new connection to remove host:port using de default
 * profile defined or letting peers to decide which profile to use.
 *
 * You need to create a new vortex connection and over it create
 * vortex channels which will allow you to send and receive data. Keep
 * in mind connection doesn't send and receive data, this is actually
 * done by channels.
 *
 * This function will block you until a connection can be create with
 * remote site or until timeout mechanism is reached. You can define
 * vortex timeout for connections creation by using
 * vortex_connection_timeout.
 *
 * Optionally you can define an <i>on_connected</i> handler to process
 * response and to avoid to get blocked on vortex_connection_new call.
 *
 * Request and process will be executed on a separeted thread from
 * caller's thread.  Inside the connection process, or session
 * negociation, beep rfc defines that remote server peer must send its
 * supported profiles. Due to this, VortexConnection will hold all
 * remote supported profiles to be enable to manage channel creation
 * in a way you don't need to pay attention on supported profiles for
 * remote peer.
 * 
 * 
 * @param host the remote peer to connect to
 * @param port the peer's port to connect to
 * @param on_connected optional handler to process connection new status
 * @param user_data optional handler to process connection new status
 * 
 * @return a new VortexConnection. You should use vortex_connection_is_ok
 * to check if you have already connected.
 */
VortexConnection * vortex_connection_new (gchar * host, gchar * port, 
					  VortexConnectionNew on_connected,
					  gpointer user_data)
{

	VortexConnectionNewData * data;

	g_return_val_if_fail (host && (* host), NULL);
	g_return_val_if_fail (port && (* port), NULL);

	data                                  = g_new0 (VortexConnectionNewData, 1);
	data->connection                      = g_new0 (VortexConnection, 1);
	data->connection->id                  = __vortex_connection_get_next_id ();
	data->connection->host                = g_strdup (host);
	data->connection->port                = g_strdup (port);
	data->connection->channels            = vortex_hash_new_full (NULL, NULL, NULL,
								      (GDestroyNotify) vortex_channel_free);
	data->connection->channel_mutex       = g_mutex_new ();
	data->connection->ref_count           = 1;
	data->connection->ref_mutex           = g_mutex_new ();
	data->connection->op_mutex            = g_mutex_new ();
	data->connection->channel_pool_mutex  = g_mutex_new ();
	data->connection->data                = vortex_hash_new_full (g_str_hash, g_str_equal,
								      NULL, NULL);
	data->connection->channel_pools       = vortex_hash_new_full (NULL, NULL,
								      NULL, 
								      (GDestroyNotify) vortex_channel_pool_close);
	data->on_connected                    = on_connected;
	data->user_data                       = user_data;
	data->threaded                        = (on_connected != NULL);

	if (data->threaded) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "invoking connection_new threaded mode");
		vortex_thread_pool_new_task ((GThreadFunc) __vortex_connection_new, data);
		return NULL;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "invoking connection_new non-threaded mode");
	return __vortex_connection_new (data);
}

typedef struct _VortexConnectionCloseData {
	GList     * list;
}VortexConnectionCloseData;

void __vortex_connection_close_channels (gpointer key, gpointer value, gpointer user_data)
{
	
	VortexChannel             * channel = value;
	VortexConnectionCloseData * data    = user_data;
	
	// save channel
	data->list = g_list_append (data->list, channel);

	return;
}


/** 
 * @brief Tries to close properly a connection and all channels inside it.
 * 
 * This function close a connection and unref it throught \ref vortex_connection_unref.
 *
 * Because there can be channels created and still working this
 * function will try to close them by executing vortex_channel_close
 * on them. If vortex_channel_close fails for one of them, the
 * vortex_connection_close will be stoped and the connection will not
 * be closed.
 *
 * Do not call this function twice because a unref operations is
 * performed over the connection if the function returns TRUE. This
 * means after calling this function the connection reference must not
 * be used until a new conneciton is created. Otherwise a segfault
 * may happen.
 * 
 * @param connection the connection to close properly.
 * 
 * @return TRUE if connection was closed and FALSE if not. If there
 * are channels still working, the connection will not be closed.
 */
gboolean               vortex_connection_close                  (VortexConnection * connection)
{
	VortexConnectionCloseData * result;
	VortexChannel             * channel0 = NULL;;
	VortexChannel             * a_channel;
	GList                     * list_aux;
	gboolean                    error;
	
	g_return_val_if_fail (connection, FALSE);

	// block connection operations during channel closing for this
	// session
	g_mutex_lock   (connection->op_mutex);
	switch (vortex_hash_size (connection->channels)) {
	case 0:
		// no channels to close, going to close connection
		goto close_connection;
	case 1:
		// only the channel 0 is available, close channel 0.
		goto close_channel0;
	default:
		// many channels to close..
		break;
	}

	// create a list of opened channels
	result         = g_new0 (VortexConnectionCloseData, 1);
	vortex_hash_foreach (connection->channels, __vortex_connection_close_channels, result);
	
	// for each channel found, try to close, letting channel 0
	// closing to the end.
	list_aux = g_list_first (result->list);
	error    = FALSE;
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		// get a reference to a channel found
		a_channel = list_aux->data;
		
		// check this channel is not the administrative one.
		if (vortex_channel_get_number (a_channel) == 0)
			continue;

		// close the channel, 
		if (!vortex_channel_close (a_channel, NULL)) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to close a channel during vortex connection closing id=%d",
			       connection->id);
			error = TRUE;
		}
	} // end for

	g_list_free (result->list);
	g_free (result);
	if (error) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unable to close connection, there are channels still working");
		return FALSE;
	}

 close_channel0:
	// now we have closed all channels we have to close channel 0
	channel0 = vortex_connection_get_channel (connection, 0);
	if (channel0 && !vortex_channel_close (channel0, NULL)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "unable to close the channel 0..");
		return FALSE;
	}
	
 close_connection:
	// release lock and free connection resources
	g_mutex_unlock (connection->op_mutex);

	// set de connection to be not connected
	__vortex_connection_set_not_connected (connection, "close connection called");

	vortex_connection_unref (connection, "vortex_connection_close");

	return TRUE;
}

/** 
 * @brief Increase internal vortex connection reference counting.
 * 
 * Inside the vortex normal function there are three threads allways
 * working plus the main thread which actually run the
 * application. This three threads are the vortex reader, the vortex
 * sequencer and the vortex writer. 
 * 
 * Because each of them have references to the same connection used on
 * every thread they need a mechanism to free the connection resources
 * is a coperative manner. This allows the vortex library to free the
 * connection resources when reference counting goes to 0.
 *
 * This function should not be used by vortex library consumer because
 * is actually called by the vortex_connection_new. 
 *
 * @param connection the connection to operate.
 * @param who who have increased the reference.
 */
void               vortex_connection_ref                    (VortexConnection * connection, gchar * who)
{
	g_return_if_fail (connection);

	g_mutex_lock   (connection->ref_mutex);
	connection->ref_count++;
	g_mutex_unlock (connection->ref_mutex);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "increased reference count for this connection to %d by %s",
	       connection->ref_count, who ? who : "??" );

	return;
}

/** 
 * @brief Decreate vortex connection reference counting 
 *
 * Allows to decrease connection reference counting. If this reference
 * counting goes under 0 the connection resources will be freed
 * 
 * @param connection the connection to operate.
 * @param who who have decreased the reference.
 */
void               vortex_connection_unref                  (VortexConnection * connection, gchar * who)
{
	g_return_if_fail (connection);

	g_mutex_lock   (connection->ref_mutex);
	connection->ref_count--;
	g_mutex_unlock (connection->ref_mutex);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "decreased connection reference count to %d decreased by %s", 
	       connection->ref_count, who ? who : "??");
	if (connection->ref_count == 0) {
		vortex_connection_free (connection);
	}
	return;
}


/** 
 * @brief Allows to tweak vortex internal timeouts
 * 
 * This function allows you to set the timeout to use on new
 * connection creation.  Default timeout is defined to 10 seconds (10
 * x 1000 = 10000 miliseconds).
 *
 * So, if you call to create a new connection with
 * vortex_connection_new and remote peer doesn't responds within tha
 * period, vortex_connection_new will return with a non-connected
 * vortex connection.
 *
 * At this moment is not implemented.
 *
 * @param miliseconds_to_wait timeout value to be used.
 */
void               vortex_connection_timeout (glong miliseconds_to_wait)
{
	
}


/** 
 * @brief Allows to get current connection status
 *
 * This function will allow you to check if your vortex connection is
 * actually connected. You must use this function before calling
 * vortex_connection_new to check what have actually happen.
 *
 * You can also use vortex_connection_get_message to check the message
 * returned by the vortex layer. This may be useful on connection
 * errors.  The free_on_fail parameter can be use to free vortex
 * connection resources if this vortex connection is not
 * connected. This operation will be done by using \ref vortex_connection_close.
 *
 * 
 * @param connection the connection to get current status.
 *
 * @param free_on_fail if TRUE the connection will be closed using
 * vortex_connection_close on not connected status.
 * 
 * @return current connection status for the given connection
 */
gboolean           vortex_connection_is_ok (VortexConnection * connection, gboolean free_on_fail)
{
	// check connection null referencing.
	if  (connection == NULL) 
		return FALSE;

	// implement free_on_fail flag
	if (free_on_fail && !connection->is_connected) {
		vortex_connection_close (connection);
		return FALSE;
	}

	// return current connection status.
	return connection->is_connected;
}

/** 
 * @brief Returns actual message status for the given connection
 *
 * Return message associated to this vortex connection. The message associated may
 * be useful on connection errors. On an successfull connected vortex connection it
 * will return "already connected".
 * 
 * @param connection the connection used to get message status.
 * 
 * @return a message about vortex connection status. You must not free
 * this value. Use vortex_connection_free.
 */
gchar            * vortex_connection_get_message (VortexConnection * connection)
{
	g_return_val_if_fail (connection, NULL);
	return connection->message;
}


/** 
 * @brief Frees vortex connection resources
 * 
 * Free all resources allocated by the vortex connection. You have to
 * keep in mind if <i>connection</i> is already connected,
 * vortex_connection_free will close all channels running on this and
 * also close the connection.
 *
 * Generally is not a good a idea to call this function. This is
 * because every connection created using the vortex api is registered
 * at some internal process (the vortex reader, sequencer and writer)
 * so they have references to created connection to do its job. 
 *
 * To close a connection properly call close_connection.
 * 
 * @param connection the connection to free
 */
void               vortex_connection_free (VortexConnection * connection)
{
	GList * list;
	
	g_return_if_fail (connection);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "freeing connection");

        // free all resources
	if (connection->message)
		g_free (connection->message);
	g_free (connection->host);
	g_free (connection->port);

	// free profiles
	if (connection->remote_supported_profiles) {
		list = g_list_first (connection->remote_supported_profiles);
		for (;list; list = g_list_next (list)) {
			xmlFree (list->data);
		}
		g_list_free (list);
	}

	// close connection
	if (connection->session != -1) {
		// it seems that this connection is 
		shutdown (connection->session, SHUT_RDWR);
		vortex_close_socket (connection->session);
		connection->session = -1;
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "session socket closed");
	}

	// free channel resources
	if (connection->channels) {
		vortex_hash_destroy (connection->channels);
		connection->channels = NULL;
	}
	// free connection data hash
	if (connection->data) {
		vortex_hash_destroy (connection->data);
		connection->data = NULL;
	}

	if (connection->channel_pools) {
		vortex_hash_destroy (connection->channel_pools);
		connection->channel_pools = NULL;
	}
	
	// free mutex
	g_mutex_free (connection->channel_mutex);
	g_mutex_free (connection->ref_mutex);
	g_mutex_free (connection->op_mutex);
	g_mutex_free (connection->channel_pool_mutex);

	// free connection
	g_free (connection);

	return;
}

/** 
 * @brief Returns the remote peer supported profiles
 * 
 * When a Vortex connection is opened, remote server sends a list of
 * BEEP supported profiles.
 *
 * This is necessary to be able to create new channels. The profile
 * selected for the channel to be created must be supported for both
 * sides.
 *
 * This function allows you to get remote peer supported
 * profiles. This can be helpful to avoid connection to a remote BEEP
 * peers that actually doesn't support your profile.  
 *
 * You must not free the returned GList. As a example, you can use
 * this function as follows:
 * 
 * \code
 *  GList * profiles = NULL;
 *  int     iterator = 0;
 *
 *  profiles = vortex_connection_get_remote_profiles (connection);
 *  g_print ("profiles for this peer: %d\n", g_list_length (profiles));
 *  list_aux = g_list_first (profiles);
 *  for (; list_aux; list_aux = g_list_next (list_aux)) {
 *      g_print ("  %d) %s\n", iterator, (gchar*) list_aux->data);
 *      iterator++;
 *  }
 * \endcode
 * 
 * @param connection the connection to get remote peer profiles.
 * 
 * @return A GList containing each element a uri identifying a remote peer profile.
 */
GList            * vortex_connection_get_remote_profiles (VortexConnection * connection)
{
	g_return_val_if_fail (connection, NULL);
	g_return_val_if_fail (connection->is_connected, NULL);

	return connection->remote_supported_profiles;
	
}

/** 
 * @brief Allows to check if the given profile is supported by the
 * remote peer.
 *
 * This function allows to check if the profile identified by
 * <i>uri</i> is soported by remote peer connected to by <i>connection</i>.
 *
 * You can use this function to check, before creating a channel, if
 * remote peer will support this profile.
 * 
 * @param connection the connection to check.
 * @param uri the profile identified by the string uri to check.
 * 
 * @return TRUE if the profile is supported by remote peer or FALSE if not.
 */
gboolean           vortex_connection_is_profile_supported (VortexConnection * connection, gchar * uri)
{
	GList * list_aux;

	g_return_val_if_fail (connection, FALSE);
	g_return_val_if_fail (uri, FALSE);

	if (!connection->is_connected) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "trying to get supported profile in a non-connected session");
		return FALSE;
	}

	// lock the connectoin while getting the list
	g_mutex_lock (connection->op_mutex);
	list_aux = g_list_first (connection->remote_supported_profiles);
	for (; list_aux; list_aux = g_list_next (list_aux)) {
		if (!g_strcasecmp (list_aux->data, uri)) {
			g_mutex_unlock (connection->op_mutex);
			return TRUE;
		}
	}

	g_mutex_unlock (connection->op_mutex);
	return FALSE;
}

/** 
 * @brief Check if a channel is already created on the given connection. 
 *
 * This function allows to check if a channel is already created
 * over this connection (or session).  Channel number must be a
 * positive integer starting from 0. Channel 0 is allways opened and
 * exists until connection (or session) is closed.
 * 
 * @param connection vortex connection to check channel existence
 * @param channel_num  channel number to check
 * 
 * @return TRUE if channel identified by <i>channel_num</i> exists,
 * otherwise FALSE.
 */
gboolean           vortex_connection_channel_exists       (VortexConnection * connection, gint channel_num)
{
	gboolean result;

	g_return_val_if_fail (channel_num >= 0,         FALSE);
	g_return_val_if_fail (connection,               FALSE);
	g_return_val_if_fail (connection->is_connected, FALSE);

	// channel 0 allways exists, and cannot be closed. It's closed when connection (or session) is closed
	if (channel_num == 0) 
		return TRUE;
	
	result = vortex_hash_lookup (GINT_TO_POINTER (connection->channels), 
				     GINT_TO_POINTER (channel_num)) != NULL;
	return result;
}

/** 
 *
 * @brief Allows to perform an iterator over all channels created
 * inside the given connnection.
 *
 * Allows an application to iterate over all channel created over this
 * session. This allows to check or do some operation over the session
 * channels.  
 *
 * The channel iteration is made following the channel number
 * order. First low-numbered channel following high-numbered ones.
 *
 * Example:
 *
 * \code
 * void my_function_foreach (gpointer key, gpointer value, gpointer user_data)
 * {
 *      VortexChannel * channel = value;
 *    
 *      // do some operation with the channel.
 * }
 *
 * void my_function (VortexConnection * connection) 
 * {
 *
 *      // do a channel foreach on this connection
 * 	vortex_connection_foreach_channel (connection, my_function_foreach, NULL);
 *
 *      return;
 * }
 * \endcode
 * 
 * @param connection the connection where channels will be iterated.
 * @param func the function to apply for each channel found
 * @param user_data user data to be passed into the foreach function.
 */
void            vortex_connection_foreach_channel        (VortexConnection * connection,
							  GHFunc func,
							  gpointer user_data)
{
	g_return_if_fail (connection);
	g_return_if_fail (func);
	
	vortex_hash_foreach (connection->channels, func, user_data);

	return;
}


/** 
 * 
 * @brief Returns the next channel number free to be used over this
 * session.
 *
 * This function iterates over the allowed channel number range, that
 * is: 1..2147483647 including both limits, steping over channel
 * number 0, which allways exists during the connection live.
 *
 *
 * @param connection the connection where the channel is going to be
 * created
 * 
 * @return the channel number to use or -1 if fail.
 */
gint               vortex_connection_get_next_channel     (VortexConnection * connection)
{
	gint channel_num = -1;
	g_return_val_if_fail (connection, -1);
	g_return_val_if_fail (connection->is_connected, -1);
	
	// create a critical section until found next channel to use
	g_mutex_lock (connection->channel_mutex);
	while (TRUE) {
		// get the next channel number to use
		connection->last_channel = ((connection->last_channel + 1) % MAX_CHANNELS_NO);

		// avoid to return 0 as valid channel number to be used
		if (connection->last_channel == 0)
			connection->last_channel++;

		// check if channel exists
		if (!vortex_connection_channel_exists (connection, connection->last_channel)) {
			channel_num = connection->last_channel;
			break;
		}
	}
	// unblock critical section until found next channel to use
	g_mutex_unlock (connection->channel_mutex);
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "returning as next channel to create: %d",
	       channel_num);
	return channel_num;
}


/** 
 * @brief Returns a reference to the channel identified by <i>channel_num</i> on this
 * connection (or vortex session).
 * 
 * @param connection the connection to look for the channel.
 * @param channel_num the channel_num identifier.
 * 
 * @return the VortexChannel reference or NULL if it doesn't exists.
 */
VortexChannel    * vortex_connection_get_channel          (VortexConnection * connection, gint channel_num)
{
	g_return_val_if_fail (channel_num >= 0,         NULL);
	g_return_val_if_fail (connection,               NULL);
	g_return_val_if_fail (connection->is_connected, NULL);

	// channel 0 allways exists, and cannot be closed. It's closed when connection (or session) is closed
	return vortex_hash_lookup (GINT_TO_POINTER (connection->channels), GINT_TO_POINTER(channel_num));
}

/** 
 * @brief Returns the socket used by this VortexConnection object.
 * 
 * @param connection the connection to get the socket.
 * 
 * @return the socket used or -1 if fail
 */
gint               vortex_connection_get_socket           (VortexConnection * connection)
{
	g_return_val_if_fail (connection,               -1);
	g_return_val_if_fail (connection->is_connected, -1);

	return connection->session;
}

/** 
 * 
 * @brief Adds a VortexChannel into an existing VortexConnection. 
 *
 * The channel to be added must not exists inside <i>connection</i>,
 * otherwise an error will be produced and channel will not be added.
 *
 * This function only adds the data structure which represents a
 * channel. It doesn't make any work about beep channel starting or
 * something similar.  
 *
 * Mainly, this function is only useful for internal vortex library
 * purposes.
 * 
 * @param connection the connection where channel will be added.
 * @param channel the channel to add.
 */
void               vortex_connection_add_channel          (VortexConnection * connection, 
							   VortexChannel * channel)
{
	VortexChannel * _channel;
	g_return_if_fail (connection);
	g_return_if_fail (connection->is_connected);
	g_return_if_fail (channel);
	g_return_if_fail (connection->channels);

	// lock channel hash
	g_mutex_lock (connection->channel_mutex);

	// check if channel doesn't existis on this connection
	_channel = vortex_hash_lookup (connection->channels, GINT_TO_POINTER (vortex_channel_get_number (channel)));
	if (_channel != NULL) {
		g_mutex_unlock (connection->channel_mutex);
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "trying to add a channel on a connection which already have this channel");
		return;
	}

	// insert new channel on this connection
	vortex_hash_replace (connection->channels, 
			      GINT_TO_POINTER (vortex_channel_get_number (channel)),  
			      channel);

	// make channel to be on state connected
	__vortex_channel_set_connected (channel);

	// unlock channel hash
	g_mutex_unlock (connection->channel_mutex);

	return;
}

/** 
 * @brief Removes the given channel from this connection. 
 * 
 * @param connection the connection where the channel will be removed.
 * @param channel the channel to remove from the connection.
 */
void               vortex_connection_remove_channel       (VortexConnection * connection, 
							   VortexChannel    * channel)
{
	g_return_if_fail (connection);
	g_return_if_fail (connection->is_connected);
	g_return_if_fail (channel);
	g_return_if_fail (connection->channels);

	vortex_hash_remove (connection->channels,
			    GINT_TO_POINTER (vortex_channel_get_number (channel)));
	return;
}

/** 
 * @brief Returns the actual host this connection is connected to.
 *
 * You must not free returned value.  If you do so, you will get
 * unexpected behaviours.
 * 
 * 
 * @param connection the connection to get host value.
 * 
 * @return the host the given connection is connected to or NULL if something fail.
 */
gchar            * vortex_connection_get_host             (VortexConnection * connection)
{
	g_return_val_if_fail (connection, NULL);

	return connection->host;
}

/**
 * vortex_connection_get_id:
 * @connection: 
 * 
 * Returns the connection identificer. The connection identifier is a
 * unique integer asigned to all connection created. This identifier
 * allows user to use this identifier for its own application purposes
 * 
 * Return value: the connection id or -1 if fails
 **/

/** 
 * @brief  Returns the connection unique identifier.
 *
 * The connection identifier is a unique integer asigned to all
 * connection created under Vortex Library. This allows Vortex programmer to
 * use this identifier for its own application purposes
 *
 * @param connection the unique integer identifier for the given connection.
 * 
 * @return the unique identifier.
 */
gint               vortex_connection_get_id               (VortexConnection * connection)
{
	g_return_val_if_fail (connection, -1);

	return connection->id;
}

/** 
 * @brief Returns the actual port this connection is connected to.
 *
 * You must not free returned value.  If you do so, you will get
 * unexpected behaviours.
 * 
 * @param connection the connection to get the port value.
 * 
 * @return the port or NULL if something fails.
 */
gchar            * vortex_connection_get_port             (VortexConnection * connection)
{
	g_return_val_if_fail (connection, NULL);

	return connection->port;
}

/** 
 * @internal
 * @brief Sets the given connection the not connected status.
 *
 * This internal vortex function allows library to set connection
 * status to false for a given connection. This function should not be
 * used by vortex library consumers.
 *
 * This function is callable over and over again on the same
 * conection. The first the function time is called sets to FALSE
 * connection state and its error message. It also close the beep
 * session and sets its socket to -1 in order to make it easily to
 * recognize for other functions.
 *
 * NOTE: next calls to function will lose error message.
 * 
 * @param connection the connection to set as.
 * @param message the new message to set.
 */
void           __vortex_connection_set_not_connected (VortexConnection * connection, gchar * message)
{
	g_return_if_fail (connection);
	g_return_if_fail (message);
	
	// set connection status to false if weren't
	g_mutex_lock (connection->op_mutex);

	if (connection->is_connected) {
		if (connection->message)
			g_free (connection->message);
		connection->message      = g_strdup (message);
	}
	connection->is_connected = FALSE;
	
	// close socket connection if weren't
	if (connection->session != -1) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "closing connection id=%d to %s:%s", 
		       connection->id,
		       connection->host, connection->port);
		shutdown (connection->session, SHUT_RDWR);
		vortex_close_socket (connection->session);
		connection->session      = -1;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "closing session id=%d and set to be not connected",
	       connection->id);
	g_mutex_unlock (connection->op_mutex);
	return;
}


/** 
 * @internal
 * @brief The function which actually send the data
 *
 * This function helps the vortex writer to do a sending round over
 * this connection. Because there are several channels over the same
 * connection sending frames, this function ensure a package for every
 * channel is sent on every round. This allows to avoid channel or
 * session starvation. 
 *
 * This function is executed from \ref
 * vortex_connection_do_a_sending_round as a foreach channel for a
 * given connection.
 * 
 * @param key   
 * @param value a channel to check if packets to be sent are waiting
 * @param user_data 
 */
void  __vortex_connection_one_sending_round (gpointer key,
					     gpointer value,
					     gpointer user_data)
{
	// the channel
	VortexChannel    * channel   = value;
	gint             * messages;
	VortexWriterData * packet;
	VortexConnection * connection;
	gchar            * rpy_num;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sending message..");
	
	// if channel msg queue is empty do nothing
	if (vortex_channel_queue_is_empty (channel)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "no message queue waiting for channel=%d..",
		       vortex_channel_get_number (channel));
		return;
	}

	// check connection status
	connection = vortex_channel_get_connection (channel);
	if (!vortex_connection_is_ok (connection, FALSE)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to send message, connection is not ok");
		return;
	}
	
	// it seems channel queue is not empty send next one message
	packet = vortex_channel_queue_next_msg (channel);
	g_return_if_fail (packet);

	// send the frame
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Sending message, size (%d):\n%s",
	       packet->the_size,  packet->the_frame);

	if (vortex_frame_send_raw (vortex_channel_get_connection (channel), 
				   packet->the_frame, packet->the_size)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "frame on channel %d sent..",
		       vortex_channel_get_number (channel));
	}else {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to sent the frame");
		__vortex_connection_set_not_connected (vortex_channel_get_connection (channel),
						       "unable to write frame");
	}

	// increase message sent
	messages = user_data;
	(* messages) ++;
	
	// signal the message have been sent
	if (packet->type == VORTEX_FRAME_TYPE_RPY) {

		// unblock waiting thread for replies sent
		vortex_channel_signal_reply_sent_on_close_blocked (channel);
		
		rpy_num = g_strdup_printf ("%d", packet->msg_no);
		vortex_channel_signal_rpy_sent (channel, rpy_num);
		g_free (rpy_num);
	}

	// free packet resources
	g_free (packet->the_frame);
	packet->the_frame = NULL;
	g_free (packet);
	packet            = NULL;
	

	return;
}

/**
 * @internal
 * @brief Helper function to Vortex Writer process
 * 
 * This function must be not used by vortex library consumer. This
 * function is mainly used by the vortex writer process to signal a
 * connection to do a sending round. 
 *
 * A sending round means to check every channel queue to see if have
 * pending frames to send. If have pending frames to be sent, the next
 * one to is sent. This tries to avoid a channel consuming all the
 * vortex connection bandwidth.
 * 
 * @param connection a connection where a sending round robin will
 * performed.
 * 
 * @return  it returns how message frame have been sent or 0 if
 * no message was sent.
 */
gint               vortex_connection_do_a_sending_round (VortexConnection * connection)
{
	gint messages = 0;
	g_return_val_if_fail (vortex_connection_is_ok (connection, FALSE), 0);

	// foreach channel, check if we have something to send
	vortex_hash_foreach (connection->channels, 
			     __vortex_connection_one_sending_round, &messages);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "message sent for this connection id=%d: %d", 
	       vortex_connection_get_id (connection),
	       messages);

	return messages;
}

/** 
 * @brief Sets user defined data associated with the given connection.
 *
 * This function allows you to store arbitrary data inside the given
 * connection. This data is store in a hash string indexed. 
 *
 * If value is NULL, this will be considered as a removing petition
 * for the given key.
 *
 * @param connection the connection to set data.
 * @param key the string key indexing the data.
 * @param value the value to be stored.
 */
void               vortex_connection_set_data               (VortexConnection * connection,
							     gchar            * key,
							     gpointer           value)
{
	g_return_if_fail (connection);

	if (value == NULL) {
		vortex_hash_remove (connection->data, key);
		return;
	}
	vortex_hash_replace (connection->data, key, value);
	
	return;
}

/** 
 * @brief Gets stored value indexed by the given key inside the given connection.
 *
 * Returns value indexed by key inside connection. This function does
 * not retreive data from remote peer, it just returns stored data
 * using vortex_connection_set_data.
 * 
 * @param connection the connection where the value will be looked up.
 * @param key the key to look up.
 * 
 * @return the value or NULL if fails.
 */
gpointer           vortex_connection_get_data               (VortexConnection * connection,
							     gchar            * key)
{
	g_return_val_if_fail (connection, NULL);
	// g_return_val_if_fail (vortex_connection_is_ok (connection, FALSE), NULL);

	return vortex_hash_lookup (connection->data, key);
}

/** 
 * @brief Returns the channel pool identified by <i>pool_id</i>.
 * 
 * @param connection the connection where the channel pool is found.
 * @param pool_id the channel pool id to look up.
 * 
 * @return the channel pool or NULL if fails.
 */
VortexChannelPool * vortex_connection_get_channel_pool       (VortexConnection * connection,
							      gint               pool_id)
{
	VortexChannelPool * pool = NULL;

	g_return_val_if_fail (connection,  NULL);
	g_return_val_if_fail (pool_id > 0, NULL);

	pool = vortex_hash_lookup (connection->channel_pools, GINT_TO_POINTER (pool_id));

	return pool;
}


/** 
 * @internal
 * @brief Locks the channel pool mutex for the given connection.
 *
 * This function is for internal Vortex purposes. This function is
 * actually used by the vortex_channel_pool module to lock the
 * channel_pool_mutex this connection have while operating with
 * channel pool over this connection.
 * 
 * @param connection the connection to lock.
 */
void                vortex_connection_lock_channel_pool      (VortexConnection * connection)
{
	g_return_if_fail (connection);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "locking pool channel..");
	g_mutex_lock (connection->channel_pool_mutex);
	
	return;
}

/** 
 * @internal
 * @brief Unlocks the channel pool mutex for the given connection.
 *
 * This function is for internal vortex purposes. This function is
 * actually used by the vortex_channel_pool module to unlock the
 * channel_pool_mutex this connection have while operating with
 * channel pool over this connection.
 * 
 * @param connection the connection to unlock.
 */
void                vortex_connection_unlock_channel_pool    (VortexConnection * connection)
{
	g_return_if_fail (connection);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unlocking pool channel..");

	g_mutex_unlock (connection->channel_pool_mutex);

	return;
}

/** 
 * @internal
 * @brief Return the next channel pool identifier.
 *
 * This function is for internal vortex purposes. This function is
 * actually used by the vortex_channel_pool module to get the next
 * channel pool id to be used.
 * 
 * @param connection the connection where the channel pool will be created.
 * 
 * @return the next id to use or -1 if fails
 */
gint                vortex_connection_next_channel_pool_id   (VortexConnection * connection)
{
	g_return_val_if_fail (connection, -1);

	connection->next_channel_pool_id++;
	return connection->next_channel_pool_id;
}

/** 
 * @internal
 * @brief Adds a new channel pool on the given connection.
 * 
 * This function is for internal vortex purposes. This function allows
 * vortex_channel_pool module to add new channel pool created over the
 * given connection.  
 *
 * @param connection the connection to operate.
 * @param pool the channel pool to add.
 */
void                vortex_connection_add_channel_pool       (VortexConnection  * connection,
							      VortexChannelPool * pool)
{
	g_return_if_fail (connection);
	g_return_if_fail (pool);

	vortex_hash_replace (connection->channel_pools, 
			     GINT_TO_POINTER (vortex_channel_pool_get_id (pool)),
			     pool);

	return;
}

/** 
 * @brief
 * @internal Removes the given channel pool on the given connection.
 *
 * This function is for internal vortex purposes. This function allows
 * vortex_channel_pool module to remove channel pools from the given
 * connection. 
 *
 * @param connection the connection where the pool will be removed.
 * @param pool the pool to remove.
 */
void                vortex_connection_remove_channel_pool    (VortexConnection  * connection,
							      VortexChannelPool * pool)
{
	g_return_if_fail (connection);
	g_return_if_fail (pool);

	vortex_hash_remove (connection->channel_pools, 
			    GINT_TO_POINTER (vortex_channel_pool_get_id (pool)));

	return;
}


/** 
 * @internal
 * @brief Support function for \ref vortex_connection_get_pending_msgs
 *
 * This function adds the message pending to be send over a given
 * channel.
 * 
 * @param key 
 * @param value the channel to retrive how many messages are waiting
 * @param user_data 
 */
void __vortex_connection_get_pending_msgs (gpointer key, gpointer value, gpointer user_data)
{
	VortexChannel * channel  = value;
	gint          * messages = user_data;
	gint            msgs     = vortex_channel_queue_length (channel);
	
	(* messages ) = (* messages) + msgs;

	return;
}

/** 
 * @brief Allows to get current frames waiting to be sent on the given connection.
 *
 * This function will iterate over all channels the connection have
 * checking if there are frames pending to be sent. The cummulative
 * result for this iteration will be returned as the pending message
 * to be sent over this connection or session.
 *
 * This function is actually used by the vortex writer to be able to
 * normalize its pending task to be processed situation while
 * unrefering connections.
 * 
 * @param connection a connection to know how many message are pending
 * to be sent.
 * 
 * @return the number or message pending to be sent.
 */
gint                vortex_connection_get_pending_msgs       (VortexConnection * connection)
{
	gint messages = 0;

	vortex_hash_foreach (connection->channels, 
			     __vortex_connection_get_pending_msgs, &messages);	
	return messages;
}

//@}
