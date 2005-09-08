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

static VortexHash * registered_profiles = NULL;
#define LOG_DOMAIN "vortex-profiles"

typedef struct _VortexProfile
{
	gchar                 * profile_name;
	VortexOnStartChannel    start;
	gpointer                start_user_data;
	VortexOnCloseChannel    close;
	gpointer                close_user_data;
	VortexOnFrameReceived   received;
	gpointer                received_user_data;
	
} VortexProfile;

/**
 * \defgroup vortex_profiles Vortex Profiles: Related functions to manage profiles inside Vortex Library.
 */

/// \addtogroup vortex_profiles
//@{

/** 
 * @brief Init vortex profiles module. 
 *
 * Init vortex profiles internal state. This is actually called by
 * \ref vortex_init so this function is not useful for vortex library consumers.
 * 
 */
void     vortex_profiles_init ()
{
	// initialize hash table if weren't
	if (registered_profiles == NULL)
		registered_profiles = vortex_hash_new (g_str_hash, g_str_equal);	

	return;
}

gboolean __vortex_profiles_default_start (gint               channel_num,
					  VortexConnection * connection,
					  gpointer           user_data)
{
	return TRUE;
}

gboolean __vortex_profiles_default_close (gint               channel_num,
					  VortexConnection * connection,
					  gpointer           user_data)
{
	return TRUE;
}


/** 
 * @brief Allows to register a new profile inside the Vortex Library.
 *
 * Register a profile to be used on channel creation. Profiles are
 * used by Vortex channels to know which message to exchange and the
 * channel semantics. To be able to create vortex channels and to
 * create a vortex servers you must register at least one profile.
 *
 * On vortex session establishment, vortex peer acting as server sends
 * to vortex peer acting as client (or initiator) its registered
 * profiles. This enable both sides to know if they can talk together.
 *
 * In order to get an idea about profile names to use you can see actual reserved profiles name
 * defined by BEEP RFC. Some on them are: 
 *  <ul>
 *   <li>The one time password profile: <i>"http://iana.org/beep/SASL/OTP"</i>
 *   <li>The TLS profile: <i>"http://iana.org/beep/TLS"</i>
 *   <li>The profile used by Coyote layer at Af-Arch: <i>"http://fact.aspl.es/profiles/coyote_profile"</i>
 *  </ul>
 *
 * For <i>uri</i> argument you must not free its memory. This function
 * does not make a copy so, if you free <i>uri</i> you will get unexpected
 * behaviours. It's recommended to use static values to avoid
 * problems.
 *
 * Associated to the profile been registered are the handlers. There are
 * three handlers to be executed during the profile life.
 * 
 * When a remote peer wants to create a channel sends a start channel
 * message. On that event <i>start</i> handler will be executed. 
 * 
 * When a remote peer wants to close an already created channel it
 * sends the close channel message. The <i>close</i> handler is executed on that
 * event. 
 * 
 * For all frames received, <i>received</i> handler will be executed.
 * You can get more info about this handlers \ref vortex_handlers "here".
 * 
 * If you don't provide handlers for <i>start</i> and <i>close</i>, a
 * default handler will be used.
 * 
 * This default handlers allways return TRUE, so, on channel creation
 * it will accept allways and on channel closing it will accept
 * allways.
 *
 * If you don't provide a received handler, all data received will be
 * acknoledged but dropped.
 *
 * There are some exception to previous information. There are two
 * levels of handlers to be executed on events for channels with a
 * profile. The first level is defined by previous ones handlers. But
 * a second level of handlers exists on per-channel basis.
 *
 * This second level of handlers are executed for the same events
 * before the first level is executed.  If a handler for a particular
 * event is not found on second level, then the first handler is
 * executed.
 *
 * If a handler for a particular event is found on second level, the
 * is executed and first level handler not.
 *
 * This allows you to have several levels of handlers to be executed
 * in a general way and handlers to be executed for a particular
 * channel.
 *
 * You can check \ref dispatch_schema "this section" to know more
 * about how second level, first level and wait reply method are
 * implemented on Vortex Library to receive frames.
 *
 * You can also check \ref profile_example "this tutorial" about
 * creating new profiles for your application. 
 * 
 * @param uri                 a profile name to register
 * @param start               a handler to control channel creation
 * @param start_user_data     user defined data to be passed in to start handler
 * @param close               a handler to control channel termination
 * @param close_user_data     user defined data to be passed in to close handler
 * @param received            a handler to control incoming frames
 * @param received_user_data  user defined data to be passed in to received handler
 */
void vortex_profiles_register (gchar                 * uri,
			       VortexOnStartChannel    start,
			       gpointer                start_user_data,
			       VortexOnCloseChannel    close,
			       gpointer                close_user_data,
			       VortexOnFrameReceived   received,
			       gpointer                received_user_data)
{
	VortexProfile * profile;

	// find out if we already have registered this profile
	profile = vortex_hash_lookup (registered_profiles, uri);
	if (profile != NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_WARNING, "attempting to register a profile already registered: %s",
		       uri);
		return;
	}
	
	// create a new profile
	profile                     = g_new0 (VortexProfile, 1);
	profile->profile_name       = uri;
	profile->start              = start ? start : __vortex_profiles_default_start;
	profile->start_user_data    = start_user_data;
	profile->close              = close ? close : __vortex_profiles_default_close;
	profile->close_user_data    = close_user_data;
	profile->received           = received;
	profile->received_user_data = received_user_data;

	// it seens this profile hasn't been registered yet
	vortex_hash_replace (registered_profiles, uri, profile);
	
	return;
}

/** 
 * @internal
 *
 * Invoke start handler for selected profile. As defined for
 * vortex_profiles_register, the start handler must be defined or a
 * default will be provided. start handler is invoked in order to know
 * if a new channel using selected profile can be created.
 * 
 * @param uri 
 * @param channel_num 
 * @param connection 
 * 
 * @return 
 */
gboolean vortex_profiles_invoke_start (gchar * uri, gint channel_num, VortexConnection * connection)
{
	VortexProfile * profile;

	g_return_val_if_fail (connection,          FALSE);
	g_return_val_if_fail (channel_num >= 0,    FALSE);
	g_return_val_if_fail (uri,                 FALSE);
	g_return_val_if_fail (registered_profiles, FALSE);

	profile = vortex_hash_lookup (registered_profiles, uri);

	if (profile == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "requeiring to invoke start handler on a not registered profile");
		return FALSE;
	}

	return profile->start (channel_num, connection, profile->start_user_data);
}

/** 
 * @brief Returns if the given profiles have actually defined the start
 * handler.
 *
 * It also returns no start handler defined if <i>uri</i> is not a
 * registered profile.
 * 
 * @param uri 
 * 
 * @return TRUE if close handler is defined or FALSE if not
 */
gboolean vortex_profiles_is_defined_start (gchar * uri)
{
	VortexProfile * profile;

	g_return_val_if_fail (uri && (* uri),      FALSE);
	g_return_val_if_fail (registered_profiles, FALSE);

	profile = vortex_hash_lookup (registered_profiles, uri);
	if (profile == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "looking up for a start handler on a non-registered profile");
		return FALSE;
	}

	return (profile->start != NULL);
}

/** 
 * @internal
 * 
 * Invokes the first level close handler. This handler is allways defined due to a close
 * handler provided by the user or due to the default handler provided by the vortex
 * library. 
 * 
 * @param uri 
 * @param channel_num 
 * @param connection 
 * 
 * @return 
 */
gboolean vortex_profiles_invoke_close (gchar            * uri, 
				       gint               channel_num,
				       VortexConnection * connection)
{
	VortexProfile * profile;

	g_return_val_if_fail (connection,          FALSE);
	g_return_val_if_fail (channel_num >= 0,    FALSE);
	g_return_val_if_fail (uri,                 FALSE);
	g_return_val_if_fail (registered_profiles, FALSE);

	profile = vortex_hash_lookup (registered_profiles, uri);

	if (profile == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "requeiring to invoke close handler on a not registered profile");
		return FALSE;
	}

	return profile->close (channel_num, connection, profile->start_user_data);
}

/** 
 * @brief Returns if the given profiles have actually defined the close handler.
 *
 * It also returns no close handler defined if <i>uri</i> is not a
 * registered profile. 
 * 
 * @param uri the uri profile to check.
 * 
 * @return TRUE if close handler is defined or FALSE if not
 */
gboolean vortex_profiles_is_defined_close (gchar * uri)
{
	VortexProfile * profile;

	g_return_val_if_fail (uri && (* uri),      FALSE);
	g_return_val_if_fail (registered_profiles, FALSE);

	profile = vortex_hash_lookup (registered_profiles, uri);
	if (profile == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "looking up for a close handler on a non-registered profile");
		return FALSE;
	}

	return (profile->close != NULL);
}

typedef struct _VortexProfileReceivedData {
	VortexProfile    * profile;
	gint               channel_num;
	VortexConnection * connection;
	VortexFrame      * frame;
}VortexProfileReceivedData;

gpointer __vortex_profiles_invoke_frame_received (gpointer __data)
{
	VortexProfileReceivedData * data         = __data;
	VortexProfile             * profile      = data->profile;
	VortexConnection          * connection   = data->connection;
	VortexFrame               * frame        = data->frame;
	VortexChannel             * channel      = vortex_connection_get_channel (connection, data->channel_num);
	gboolean                    is_connected = FALSE;

	// get a reference to channel number so we can check after
	// frame received handler if the channel have been closed.
	// Once the frame received have finished this will help us to
	// know if application space have issued a close channel.
	gint                      channel_num  = vortex_channel_get_number (channel);

	// free paramters data
	g_free (data);

	// record actual connection state
	is_connected = vortex_connection_is_ok (connection, FALSE);
	if (! is_connected) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "invoking frame receive on a non-connected session");
		goto free_resources;
	}

	// increase connection reference counting to avoid reference loosing 
	// if connection is closed
	vortex_connection_ref (connection, "first level handler (frame received)");

	// invoke frame received on this channel
	profile->received (channel, connection, frame, profile->received_user_data);

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

	// log a message
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       "invocation frame received handler for channel %d finished (first level: profiles)",
	       channel_num);

 free_resources:
	// decrease reference counting
	vortex_connection_unref (connection, "first level handler (frame recieved)");

	// before invoke profile received frame free frame
	vortex_frame_free (frame);

	// return nothing
	return NULL;
}

/** 
 * @internal
 * 
 * Invoke the first level frame received handler for the given profile
 * into the selected channel. This function also frees the vortex
 * frame passed in.
 *
 * @param uri 
 * @param channel_num 
 * @param connection 
 * @param frame 
 * 
 * @return TRUE if frame was delivered to a handler or FALSE if
 * frame was not delivered.
 */
gboolean vortex_profiles_invoke_frame_received (gchar            * uri,
						gint               channel_num,
						VortexConnection * connection,
						VortexFrame      * frame)
{
	VortexProfile             * profile;
	VortexProfileReceivedData * data;

	g_return_val_if_fail (uri && (*uri), FALSE);
	g_return_val_if_fail (channel_num >= 0, FALSE);
	g_return_val_if_fail (connection, FALSE);
	g_return_val_if_fail (frame, FALSE);
	g_return_val_if_fail (registered_profiles, FALSE);

	profile = vortex_hash_lookup (registered_profiles, uri);

	if ((profile == NULL) || (profile->received == NULL)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "invoking frame received handler on a profile which haven't been defined");
		return FALSE;
	}

	// invoke frame received handler. Because frame must be run
	// inside a separeted thread (as defined in user doc) we have
	// to do that. This contrast with close and start invocation
	// which have to return inmediatly is channel can be close or
	// created.
	//
	// Moreover, the close and start channel are invoked in a
	// separeted thread because the channel 0's frame received
	// handler (the only one channel which can process close and
	// start message is the 0), is run inside a separeted thread. See vortex_frame_

	// invoke frame received
	data              = g_new0 (VortexProfileReceivedData, 1);
	data->profile     = profile;
	data->channel_num = channel_num;
	data->connection  = connection;
	data->frame       = frame;

	vortex_thread_pool_new_task (__vortex_profiles_invoke_frame_received, data);
	
	return TRUE;
}

/** 
 * @brief  Returns if the given profiles have actually defined the received  handler. 
 * 
 * It also returns no received handler defined if <i>uri</i> is not a
 * registered profile.
 * 
 * @param uri 
 * 
 * @return TRUE if received handler is defined or FALSE if not
 */
gboolean vortex_profiles_is_defined_received (gchar * uri)
{
	VortexProfile * profile;

	g_return_val_if_fail (uri && (* uri),      FALSE);
	g_return_val_if_fail (registered_profiles, FALSE);

	profile = vortex_hash_lookup (registered_profiles, uri);
	if (profile == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "looking up for a received handler on a non-registered profile");
		return FALSE;
	}

	return (profile->close != NULL);
}



void __get_actual_list (gpointer       key,
			gpointer       value,
			gpointer       user_data)
{
	GList ** ptr_list = user_data;
	GList *      list = (GList *) (*ptr_list);

	list              = g_list_append (list, key);
	(*ptr_list)       = list;

	return;
}

/** 
 * @brief Return actual profile registered list.
 * 
 * The GList returned contains all uri that have been registered by
 * using \ref vortex_profiles_register. 
 * 
 * You must free returned after using it. Use g_list_free.  Remember
 * not to remove any value inside returned list.
 * 
 * @return the actual profile registered list.
 */
GList * vortex_profiles_get_actual_list ()
{
	GList * result = NULL;
	g_return_val_if_fail (registered_profiles, NULL);

	vortex_hash_foreach (registered_profiles, __get_actual_list, &result);

	return result;
}

/** 
 * @brief Return the actual number of profiles registered. 
 * 
 * @return number of profiles registered.
 */
gint    vortex_profiles_registered ()
{
	if (registered_profiles == NULL)
		return 0;
	return vortex_hash_size (registered_profiles);
}

/** 
 * @brief Return if a profile identifier is registered. 
 * 
 * @param uri 
 * 
 * @return TRUE if the profile is registered or FALSE if not. 
 */
gboolean vortex_profiles_is_registered (gchar * uri)
{
	g_return_val_if_fail (registered_profiles, FALSE);

	return (vortex_hash_lookup (registered_profiles, uri) != NULL);
}

// @}
