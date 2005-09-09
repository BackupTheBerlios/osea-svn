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
#define LOG_DOMAIN "vortex-channel-pool"


/**
 * \defgroup vortex_channel_pool Vortex Channel Pool: Channel Pool management function.
 */

/// \addtogroup vortex_channel_pool
//@{

// the channel pool type
struct _VortexChannelPool {
	// the unique channel pool identifier
	gint                     id;
	// the list of channel this pool have
	GList                  * channels;
	// the connection where the pool was created
	VortexConnection       * connection;
	
	// the profile this pool have and all channel to be created on
	// the future. The following vars will be used to be able to
	// create new channel setting the same data.
	gchar                  * profile;
	VortexOnCloseChannel     close;
	gpointer                 close_user_data;
	VortexOnFrameReceived    received;
	gpointer                 received_user_data;

};


typedef struct _VortexChannelPoolData {
	gboolean                    threaded;
	VortexConnection          * connection;
	gchar                     * profile;
	gint                        max_num;
	VortexOnCloseChannel        close;
	gpointer                    close_user_data;
	VortexOnFrameReceived       received;
	gpointer                    received_user_data;
	VortexOnChannelPoolCreated  on_channel_pool_created;
	gpointer                    user_data;
}VortexChannelPoolData;


/** 
 * @internal
 * Support function for this module which returns if a channel is
 * ready to be used.
 * 
 * @param channel the channel to operate
 * 
 * @return TRUE if ready to be used or FALSE if not
 */
gboolean __vortex_channel_pool_is_ready (VortexChannel * channel) 
{

	return (vortex_channel_is_opened (channel)          && 
		(!vortex_channel_is_being_closed (channel)) && 
		vortex_channel_is_up_to_date (channel)      &&
		(vortex_channel_get_data (channel, "status_busy") == NULL));
}

/** 
 * @internal
 * 
 * Support function for __vortex_channel_pool_new and
 * vortex_channel_pool_add. This function actually creates new
 * channels and adds them to the channel pool.
 *
 * @param pool the pool over channels will be created
 * @param max_num the maximum number of channels to create.
 * 
 * @return a reference to the last channel created.
 */
VortexChannel * __vortex_channel_pool_add_channels (VortexChannelPool * pool, gint max_num) 
{
	gint             iterator = 0;
	VortexChannel  * channel  = NULL;;

	// start channels
	while (iterator < max_num) {
		// create the channel 
		channel = vortex_channel_new (pool->connection, 
					      0,
					      pool->profile,
					      // close handler stuff
					      pool->close, pool->close_user_data,
					      // received handler stuff
					      pool->received, pool->received_user_data,
					      // on created handler not defined
					      NULL, NULL);
		
		// check if the channel have been created if not break-the-loop
		if (channel == NULL) {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
			       "unable to create a channel inside the channel pool creation, this pool will have fewer channel than requested");
			break;
		}
		
		// so the channel have been created 
		pool->channels = g_list_append (pool->channels, channel);

		// set a reference to the pool this channel belongs to
		vortex_channel_set_pool (channel, pool);

		// update iterator
		iterator++;
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channels added %d to the pool id=%d", iterator, pool->id);
	
	return channel;
}

/** 
 * @internal
 *
 * Support function for vortex_channel_pool_new which actually does the work. 
 * 
 * @param data 
 * 
 * @return 
 */
gpointer __vortex_channel_pool_new (VortexChannelPoolData * data)
{

	// parameters from main function
	gboolean                      threaded                = data->threaded;
	VortexConnection            * connection              = data->connection;
	gchar                       * profile                 = data->profile;
	gint                          max_num                 = data->max_num;
	VortexOnCloseChannel          close                   = data->close;
	gpointer                      close_user_data         = data->close_user_data;
	VortexOnFrameReceived         received                = data->received;
	gpointer                      received_user_data      = data->received_user_data;
	VortexOnChannelPoolCreated    on_channel_pool_created = data->on_channel_pool_created;
	gpointer                      user_data               = data->user_data;

	// function local parameters
	VortexChannelPool           * channel_pool;

	// free data
	g_free (data);

	// lock the mutex
	vortex_connection_lock_channel_pool (connection);

	// init channel pool type
	channel_pool                     = g_new0 (VortexChannelPool, 1);
	channel_pool->id                 = vortex_connection_next_channel_pool_id (connection);
	channel_pool->connection         = connection;
	channel_pool->profile            = g_strdup (profile);
	channel_pool->close              = close;
	channel_pool->close_user_data    = close_user_data;
	channel_pool->received           = received;
	channel_pool->received_user_data = received_user_data;

	// create channels for the pool
	__vortex_channel_pool_add_channels (channel_pool, max_num);

	// now have have created the channel pool install it inside the connection
	channel_pool->connection = connection;
	vortex_connection_add_channel_pool (connection, channel_pool);

	// unlock the mutex
	vortex_connection_unlock_channel_pool (connection);

	// return the data
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel pool created id=%d over connection id=%d", channel_pool->id,
	       vortex_connection_get_id (connection));
	if (threaded) {
		on_channel_pool_created (channel_pool, user_data);
		return NULL;
	}

	return channel_pool;
}

/**
 * vortex_channel_pool_new:

 * 

 * 
 * Return value: 
 **/

/** 
 * @brief Creates a new pool of VortexChannels. 
 * 
 * This module was created to allow having a pool of pre-created
 * channels to be used over and over again during the application
 * life.
 * 
 * Because channel creation and channel close is a expensive process
 * (especially due to all requierements to be meet during channel
 * close) this pool can help to improve greatly your application
 * comunication throutput.
 * 
 * Keep in mind the following facts which may help you to chose using
 * vortex channel pool instead of controling yourself the channels.
 *   <ul>
 * 
 *   <li>If your application needs to not be blocked while sending
 *   messages you have to use a new channel or a channel that is not 
 *   waiting for a reply. 
 * 
 *   BEEP RFC definition says: <i>"any message sent will be delivered in
 *   order at the remote peer and remote peer <b>must</b> reply the message
 *   receive in the same order."</i>
 *
 *   As a consecuence if message a and b are sent over channel 1, the
 *   message reply for b won't be received until message reply for a
 *   is received.
 *
 *   In other words your applications needs to have a and b message to
 *   be independent you'll have to use diferent channel and those
 *   channels must not be waiting for a reply.</li>
 *
 *   <li>To create a channel needs two message to be exchanged between
 *   peers and, of course, the channel creation can be denied by remot
 *   peer. This have a considerable overhead.</li>
 *   
 *   <li>To close a channel needs two message to be exchanged between
 *   peers and remote peers may accept channel to be closed at any
 *   time. This is not only a overhead problem but also great
 *   performance impact problem if your application close channels
 *   frecuently.</li>
 * </ul>
 *
 * The VortexChannelPool allows you to create a pool and then keep on
 * query to use the next channel ready to be used. This notion of
 * "ready to be used" means this channel have no pending replies to be
 * received so if you application send a message it will receive the
 * reply as soon as posible. 
 * 
 * Actually the reply can be really late due to remote peer processing
 * but it will not be delayed because previously message sent awaiting
 * for replies.
 *
 * Once you create a pool you can get its actual size, add or remove
 * channel from the pool or attach new channel that weren't created by
 * this module.
 *
 * All channel will be created over the given session
 * (<i>connection</i>) and will be created under the semanthics
 * defined by <i>profile</i>. The function will block you until all
 * channels are created. During the channel creation process one
 * channel may fail to be created so you should check how many
 * channels have been really created.
 *
 * You can create several channel pools over the same
 * connection. Every channel pool is identified by an id. This Id can
 * be used to lookup certain channel pool into a given
 * connection. 
 * 
 * Once a channel pool is created, it is associated with the given
 * connection so calls to \ref vortex_connection_get_channel_pool over a
 * connection will return the pool created. You can of course hold a
 * reference to your channel pool and avoid using previous function.
 *
 * Finaly the function \ref vortex_channel_pool_get_next_ready will return
 * the next channel ready to be used. But this function may return
 * NULL if no channel is ready to be used. You can optionlly set the
 * auto_inc paramter to increase channel pool size to resolv your
 * petition.
 * 
 * @param connection              the session were channels will be created.
 * @param profile                 the profile the channels will use.
 * @param max_num                 the number of channels this pool should have.
 * @param close                   handler to manage channel closing.
 * @param close_user_data         user data to be passed in to close handler.
 * @param received                handler to manage frame reception on channel.
 * @param received_user_data      data to be passed in to <i>received</i> handler.
 * @param on_channel_pool_created a callback to be able to make channel process to be async.
 * @param user_data               user data to be passed in to <i>on_channel_created</i>.
 * 
 * @return a newly created VortexChannelPool
 */
VortexChannelPool * vortex_channel_pool_new            (VortexConnection          * connection,
							gchar                     * profile,
							gint                        max_num,
							VortexOnCloseChannel        close,
							gpointer                    close_user_data,
							VortexOnFrameReceived       received,
							gpointer                    received_user_data,
							VortexOnChannelPoolCreated  on_channel_pool_created,
							gpointer user_data)
{
	VortexChannelPoolData * data;

	// check input data
	g_return_val_if_fail (connection,             NULL);
	g_return_val_if_fail (profile && (* profile), NULL);
	g_return_val_if_fail (max_num > 0,            NULL);

	// prepare data
	data                          = g_new0 (VortexChannelPoolData, 1);
	data->connection              = connection;
	data->profile                 = profile;
	data->max_num                 = max_num;
	data->close                   = close;
	data->close_user_data         = close_user_data;
	data->received                = received;
	data->received_user_data      = received_user_data;
	data->on_channel_pool_created = on_channel_pool_created;
	data->threaded                = (on_channel_pool_created != NULL);

	// invoke threaded mode if defined on_channel_pool_created
	if (data->threaded) {
		vortex_thread_pool_new_task ((GThreadFunc) __vortex_channel_pool_new, data);
		return NULL;
	}

	// invoke channel_pool_new in non-threaded mode due to not defining on_channel_pool_created
	return __vortex_channel_pool_new (data);
}

/** 
 * @brief Retuns the number of channel this pool have. 
 * 
 * @param pool the vortex channel pool to operate on.
 * 
 * @return the channel number or -1 if fail
 */
gint                vortex_channel_pool_get_num        (VortexChannelPool * pool)
{
	gint num;
	g_return_val_if_fail (pool, -1);

	vortex_connection_lock_channel_pool   (pool->connection);

	num = g_list_length (pool->channels);

	vortex_connection_unlock_channel_pool (pool->connection);

	return num;
}


/** 
 * @brief Adds more channel to channel pool already created.
 *
 * This function allows to add more channels to the actual pool. The
 * new channels added to the pool will use the same profile
 * configuration, close handler and received handler which was used at
 * \ref vortex_channel_pool_new. 
 *
 * @param pool the vortex channel pool to operate on.
 * @param num the number of channels to add.
 */
void                vortex_channel_pool_add            (VortexChannelPool * pool,
						        gint num)
{
	g_return_if_fail (pool);
	g_return_if_fail (num > 0);
	
	vortex_connection_lock_channel_pool   (pool->connection);

	__vortex_channel_pool_add_channels (pool, num);

	vortex_connection_unlock_channel_pool (pool->connection);
	
	return;
}

/** 
 * @internal
 *
 * Support function for vortex_channel_pool_remove. This function is
 * also used by vortex_channel_pool_close. Actually close @num channel
 * from the pool.
 * 
 * @param pool the pool were channels will be closed.
 * @param num the number of channels to close.
 */
void __vortex_channel_pool_remove (VortexChannelPool * pool, gint num)
{
	GList         * list_aux;
	VortexChannel * channel;
	gint            iterator;
	gint            max_num;	

	g_return_if_fail (pool);
	g_return_if_fail (num > 0);

	max_num  = g_list_length (pool->channels);
	iterator = 0;
 init:
	list_aux = g_list_first (pool->channels);
	for (; list_aux; list_aux = g_list_next (list_aux)) {

		// check if number of channel closed have reached max
		// number of channels to close
		if (iterator == max_num)
			break;

		// check if number of channeld closed have reached max
		// number of channels to close
		if (iterator == num)
			break;
			

		channel = list_aux->data;
		if (__vortex_channel_pool_is_ready (channel)) {
			// increate number of channels closed
			iterator++;
			
			// remove the channel from the channel pool
			pool->channels = g_list_delete_link (pool->channels, list_aux);

			// close the channel
			vortex_channel_close (channel, NULL);

			// init again the loop because we have removed an element from it
			goto init;
		}
	}

	return;
}

/**
 * @brief Removes channels from the given channel pool.
 *
 * 
 * This function allows to remove channels from the pool. This
 * function may block you because the channel to be removed are
 * selected from those which are ready (no message reply waiting). But
 * it may occur all channel from the pool are busy so function will
 * wait until channel gets ready to be removed.
 *
 * If you try to close more channel than the pool have the function
 * will close only those channel the pool already have. No error will
 * be reported on this case.
 *
 * @param pool the vortex channel pool to operate on.
 * @param num the number of channels to remove.
 **/
void                vortex_channel_pool_remove         (VortexChannelPool * pool,
							gint num)
{
	g_return_if_fail (pool);
	g_return_if_fail (num > 0);

	vortex_connection_lock_channel_pool   (pool->connection);

	__vortex_channel_pool_remove (pool, num);

	vortex_connection_unlock_channel_pool (pool->connection);

	return;
}


/**
 * @brief Closes a channel pool.
 * 
 * You should not call this function directly because the channel pool
 * is already registered on a connection which actually will also try
 * to remove the pool registered when the connection is destroyed. So,
 * you can actually leave this function and let the connection
 * destroying process to do the job. 
 *
 * This function will remove the channel pool from the hash the
 * connection have that will activate this function which finally
 * close all channels and deallocated all resources.
 *
 * You may wondering why channel pool destroying is threat this
 * way. This is because a channel pool doesn't exists without the
 * connection concept which means the connection have hard references
 * to channels pools created and must be notified when some pool is
 * not going to be available.
 *
 * This function may block you because a vortex_channel_close will be
 * issued over all channels.
 *
 * This function is also used as a destroy function because it frees
 * all resources allocated by the pool after closing all channels pool.
 * 
 * @param pool the vortex channel pool to operate on.
 **/
void                vortex_channel_pool_close          (VortexChannelPool * pool)
{
	g_return_if_fail (pool);

	vortex_connection_lock_channel_pool   (pool->connection);
	
	// first close all channels from this pool
	__vortex_channel_pool_remove (pool, g_list_length (pool->channels));

	g_free (pool->profile);

	vortex_connection_unlock_channel_pool (pool->connection);

	g_free (pool);

	return;
}

/**
 * @internal
 * 
 * Support function which checks if the given pool and the given
 * channel share the same connection.
 * 
 * Return value: FALSE if both elements have diferent connections and
 * TRUE if share it.
 * 
 * @param pool the pool to check.
 * @param channel the channel to check.
 **/
gboolean __vortex_channel_check_same_connections (VortexChannelPool * pool, 
						  VortexChannel     * channel)
{
	VortexConnection * connection;
	VortexConnection * connection2;

	g_return_val_if_fail (pool,    FALSE);
	g_return_val_if_fail (channel, FALSE);
	
	connection  = vortex_channel_pool_get_connection (pool);
	connection2 = vortex_channel_get_connection (channel);
	
	if (vortex_connection_get_id (connection) !=
	    vortex_connection_get_id (connection2)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "trying to add a channel from a different session. Channels from different connections can't be mixed");
		return FALSE;
	}

	return TRUE;
}

/**
 * @internal
 * 
 * Support channel pool function which returns if the given channel
 * actually belongs to the given vortex channel pool.
 *
 * Keep in mind this function actually doesn't check if the given
 * channel to check actuall have the same connection which may be an
 * error even this function returns the channel pool doesn't have a
 * channel with this number.
 * 
 * @param pool the channel pool to check.
 * @param channel the channel to check existence.
 *
 * @return FALSE if channel doesn't exists and TRUE if it does.
 **/
gboolean __vortex_channel_pool_channel_exists (VortexChannelPool * pool, 
					       VortexChannel     * channel) 
{
	VortexChannel    * channel2;
	GList            * list_aux;

	list_aux = g_list_first (pool->channels);
	for (; list_aux ; list_aux = g_list_next (list_aux)) {

		channel2 = list_aux->data;
		
		if (vortex_channel_get_number (channel) == 
		    vortex_channel_get_number (channel2)) {
			return TRUE;
		}
	}

	return FALSE;
}

/** 
 * @brief Adds a channel reference to a channel pool.
 *
 * Allows to add an already created channel to an already create
 * pool. 
 * 
 * @param pool the vortex channel pool to operate on.
 * @param channel the vortex channel to attach
 */
void                vortex_channel_pool_attach         (VortexChannelPool * pool,
							VortexChannel     * channel)
{

	g_return_if_fail (pool);
	g_return_if_fail (channel);

	// check new channel belongs to the same connection
	if (!__vortex_channel_check_same_connections (pool, channel))
		return;

	// lock to operate
	vortex_connection_lock_channel_pool   (pool->connection);

	// check if the channel to add doesn't exist on the pool
	if (__vortex_channel_pool_channel_exists (pool, channel)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "trying to add a channel which already exists on the channel pool");
		vortex_connection_unlock_channel_pool (pool->connection);
		return;
	}

	// it seems the channel wasn't found on the channel pool. Add the channel.
	pool->channels = g_list_append (pool->channels, channel);

	vortex_connection_unlock_channel_pool (pool->connection);

	return;
}

/**
 * @brief Detach a channel reference from the channel pool.
 * 
 * Allows to deattach the given channel from the pool. 
 *
 * This functixon actually doesn't close the channel even the channel
 * is removed from the channel pool. Ensure to close the channel when
 * no longer needed by using vortex_channel_close.
 *
 * @param pool the vortex channel pool to operate on.
 * @param channel the vortex channel to deattach.
 *
 **/
void                vortex_channel_pool_deattach       (VortexChannelPool * pool,
							VortexChannel     * channel)
{
	g_return_if_fail (pool);
	g_return_if_fail (channel);

	// check new channel belongs to the same connection
	if (!__vortex_channel_check_same_connections (pool, channel))
		return;

	// lock to operate
	vortex_connection_lock_channel_pool   (pool->connection);

	// check if the channel to add doesn't exist on the pool
	if (!__vortex_channel_pool_channel_exists (pool, channel)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "trying to remove a channel which doesn't exists on the channel pool");
		vortex_connection_unlock_channel_pool (pool->connection);
		return;
	}

	// it seems the channel wasn't found on the channel pool. Add the channel.
	pool->channels = g_list_remove (pool->channels, channel);

	if (pool->channels == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "this pool id=%d over connection id=%d have no channels this may be a problem",
		       pool->id, vortex_connection_get_id (pool->connection));
	}

	vortex_connection_unlock_channel_pool (pool->connection);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel id=%d dettached from pool id=%d",
	       vortex_channel_get_number (channel), vortex_channel_pool_get_id (pool));

	return;
}

/**
 * @internal
 *
 * 
 * Support function to print the channel pool status. This function
 * must be used enclosed with lock/unlock pair.
 *
 * @param pool the pool to print out the status.
 **/
void __vortex_channel_pool_print_status (VortexChannelPool * pool, gchar * action) 
{
	GList         * list_aux;
	VortexChannel * channel;

	if (getenv ("AF_DEBUG")) {

		g_print ("[%s] (connection=%d actual pool=%d size: %d channels [", 
			 action, vortex_connection_get_id (pool->connection), pool->id,
			 g_list_length (pool->channels));
		
		list_aux = g_list_first (pool->channels);
		for (; list_aux; list_aux = g_list_next (list_aux)) {
			channel = list_aux->data;
			g_print ("%d", vortex_channel_get_number (channel));
			
			__vortex_channel_pool_is_ready (channel) ? g_print ("(R)") : g_print ("(B)");
			
			if (g_list_next (list_aux)) 
				g_print (" ");
		}
		g_print ("]\n");
	}

	return;
}


/**
 * @brief Returns the next "ready to use" channel from the given pool.
 * 
 * This function returns the next "ready to use" channel from the
 * given pool. Because a vortex channel pool may have no channel ready
 * to be used the function could return NULL. But you can also make
 * this function to add a new channel to the pool if no channel is
 * ready by using auto_inc as TRUE.
 *
 * The channel returned can be used for any operation even close the
 * channel. But, beforek issuing a close operation over a channel
 * which already belongs to a pool this channel must be deattached
 * from the pool using \ref vortex_channel_pool_deattach. 
 *
 * In general a close operation over channels belonging to a pool is
 * not recomended. In fact, the need to avoid channel closing
 * operation was the main reason to produce the vortex_channel_pool
 * module because as we have said the channel close operation is too
 * expensive. 
 * 
 * It's a better approach to let the vortex connection destruction
 * function to close all channels created for a pool. Actually the
 * vortex connection destroy function close and destroy all channel
 * pool created over a given connection.
 *
 * Another recomendation to keep in mind is the startup problem. Due
 * to the initial requierement to create new channels for the pool on
 * the first connections it may slow down the startup giving a
 * sense of slow link connection. A good approach is to create a pool
 * with one channel and use this function with the auto_inc set
 * allways to TRUE. 
 * 
 * This will enforce to create new channels only when needed reducing
 * the perfomance impact of creating an arbitary number of channels
 * (inside the pool) at the startup. Of course this is only a
 * recomendation.
 *
 * @param pool the pool where the channel will be get.
 * @param auto_inc instruct this function to create a new channel if not channel is ready to be used.k 
 *
 * @return the next channel ready to use or NULL if fails. Note NULL may also be returned even seting auto_inc to TRUE.
 **/
VortexChannel     * vortex_channel_pool_get_next_ready (VortexChannelPool * pool,
							gboolean auto_inc)
{
	GList         * list_aux  = NULL;
	VortexChannel * channel   = NULL;

	g_return_val_if_fail (pool, NULL);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "getting new channel before locking..");

	vortex_connection_lock_channel_pool   (pool->connection);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "getting next channel to use");
	
	// for each channel inside the pool check is some one  is ready
	list_aux = g_list_first (pool->channels);
	for (; list_aux ; list_aux = g_list_next (list_aux)) {

		channel = list_aux->data;

		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "checking the channel id=%d", 
		       vortex_channel_get_number (channel));

		if (__vortex_channel_pool_is_ready (channel)) {
			// ok!, we have found a channel that is ready to be use.
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel id=%d is ready to be used, flaged as busy",
			       vortex_channel_get_number (channel));
			goto end;
		}
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel id=%d is not ready",
		       vortex_channel_get_number (channel));
	}

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "it seems there is no channel ready to use, check auto_inc flag");


	// it seems there is no channel available so check auto_inc var to create a new channel or simply return
	if (auto_inc) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "we have auto_inc flag to TRUE, creating a new channel");
		channel = __vortex_channel_pool_add_channels (pool, 1);

	}


 end:
	// flag this channel to be busy
	vortex_channel_set_data (channel, "status_busy", GINT_TO_POINTER (TRUE));

	if (channel != NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "returning channel id=%d for pool id=%d connection id=%d",
		       vortex_channel_get_number (channel), pool->id, 
		       vortex_connection_get_id (pool->connection));
		__vortex_channel_pool_print_status (pool, "get_next_ready");

	}
	
	// unlock operations
	vortex_connection_unlock_channel_pool (pool->connection);	

	return channel;
}

/**
 * @brief Release a channel from the channel pool.
 * 
 * Once a channel from a pool is no longer needed or you may want to
 * flag this channel as ready to use by other thread you must call
 * this function. This function doesn't remove the channel from the
 * pool only release the channel to be used by other thread.
 *
 * @param pool the pool where the channel resides.
 * @param channel the channel to release.
 * 
 **/
void                vortex_channel_pool_release_channel   (VortexChannelPool * pool,
							   VortexChannel     * channel)
{
	g_return_if_fail (pool);
	g_return_if_fail (channel);

	// check new channel belongs to the same connection
	if (!__vortex_channel_check_same_connections (pool, channel))
		return;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "attempting to release the channel id=%d for pool id=%d connection id=%d", 
	       vortex_channel_get_number (channel), pool->id, vortex_connection_get_id (pool->connection));
	
	// unlock operations
	vortex_connection_lock_channel_pool (pool->connection);	

	// check if the channel to add doesn't exist on the pool
	if (!__vortex_channel_pool_channel_exists (pool, channel)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "trying to release a channel which doesn't exists on the channel pool");
		vortex_connection_unlock_channel_pool (pool->connection);
		return;
	}

	// unflag channel to be choosable
	vortex_channel_set_data (channel, "status_busy", NULL);


	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "channel id=%d for pool id=%d connection id=%d was released", 
	       vortex_channel_get_number (channel), pool->id, vortex_connection_get_id (pool->connection));

	__vortex_channel_pool_print_status (pool, "releasing     ");
	// unlock operations
	vortex_connection_unlock_channel_pool (pool->connection);	

	return;
}

/**
 * @brief Retusn the channel pool unique identifier.
 * 
 * Returns the channel pool identification. Every channel pool created
 * have an unique id ranging from 1 to MAX_CHANNEL_NO which is really
 * large. This means you can't create more channel pools than channels
 * allowed over a connection.
 *
 * This unique identifier can be used for several application purposes
 * but from the vortex view it's used to get a given channel pool for
 * a given connection using \ref vortex_connection_get_channel_pool. If you
 * create only one channel pool over a connection the previous
 * function will return this channel pool if 1 is passed in as
 * pool_id.
 *
 * Every channel pool identifier is unique from inside a
 * connection. You can actually create several channel pool which may
 * have as unique id = 1 over different connections.
 * 
 * @param pool the pool to return its id.
 *
 * @return the pool id or -1 if fails
 **/
gint                vortex_channel_pool_get_id         (VortexChannelPool * pool)
{
	g_return_val_if_fail (pool, -1);

	return pool->id;
}

/**
 * @brief Returns the Vortex Connection where this channel pool is created.
 * 
 * Returns the connection where actually the given pool resides. Every
 * pool is actually a set of channels which are created over the same
 * connection.
 *
 * @param pool the pool to get its connection. 
 *
 * @return The connection is pool have.
 **/
VortexConnection  * vortex_channel_pool_get_connection (VortexChannelPool * pool)
{
	g_return_val_if_fail (pool, NULL);

	return pool->connection;
}

// @}
