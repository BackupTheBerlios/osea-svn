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
#ifndef __VORTEX_HANDLERS_H__
#define __VORTEX_HANDLERS_H__

#include <vortex.h>

/**
 * \defgroup vortex_handlers Vortex Handlers: Handlers used across Vortex Library for async notifications.
 */

/// \addtogroup vortex_handlers
//@{


/** 
 * @brief Async notification for listener creation.
 *
 * Functions using this handler:
 * - \ref vortex_listener_new
 *
 * Optional handler defined to report which host and port have
 * actually allocated a listener peer. If host and port is null means
 * listener have failed to run.
 *
 * You should not free any parameter received, vortex system will do
 * this for you.  If you want to actually keep a copy you should use
 * g_strdup.
 * 
 * @param host the final host binded
 * @param port the final port binded
 * @param status the listener creation status.
 * @param message the message reporting the listener status creation.
 * @param user_data user data passed in to this async notifier.
 */
typedef void (*VortexListenerReady)           (gchar * host, gint port, VortexStatus status, 
					       gchar * message, gpointer user_data);



// VortexConnectionNew:
// ~~~~~~~~~~~~~~~~~~~~
// Functions using this handler:
//   vortex_connection_new 
// 
// Optional handler defined to report the final status for a
// \ref vortex_connection_new process. This handler allows you to create a
// connection to a vortex server in a non-blocking (or asynchronously
// way). Launch the connection new process defining a handler of this
// type and process connection creation status on that handler.

/** 
 * @brief Async notification for creation connection process.
 *
 * Functions using this handler:
 *  - \ref vortex_connection_new 
 * 
 * Optional handler defined to report the final status for a
 * vortex_connection_new process. This handler allows you to create a
 * connection to a vortex server in a non-blocking (or asynchronously
 * way). Launch the connection new process defining a handler of this
 * type and process connection creation status on that handler.
 *
 * @param connection the connection created.
 * @param user_data user defined data passed in to this async notifier.
 */
typedef void (*VortexConnectionNew) (VortexConnection * connection, gpointer user_data);


/** 
 * @brief Async notification for start channel message received for a given profile.
 *
 * Functions using this handler:
 *  - \ref vortex_profiles_register
 *
 * This handler is used to notify listener role that a channel is
 * asked to be created. The channel to be created is associated with
 * connection reference. If listener agree to create the channel TRUE
 * must be returned. Otherwise, FALSE must be returned if channel
 * creation is not wanted. 
 * 
 * Vortex library ensure no petition to create new channels will
 * conflict with actual channel created to actual connection passed
 * Keep in mind BEEP protocol defines a connection as a session holder
 * of channels. But, channels on different session (or connection) are
 * independent.
 * 
 * On again, at the moment OnStartChannel is invoked, this channel is
 * not already created. So you can't get a reference to this channel
 * until you return TRUE.
 *
 * You can also provied a user_data pointer to functions with accept
 * this type of handlers to get access to some user defined data.
 * 
 * @param channel_num the channel num for the new channel attempted to be created.
 * @param connection the connection where the channel is going to be created.
 * @param user_data user defined data passed in to this async notifier.
 * 
 * @return TRUE if the new channel can be created or FALSE if not.
 */
typedef  gboolean (*VortexOnStartChannel)      (gint channel_num,
						VortexConnection * connection,
						gpointer user_data);



/** 
 * @brief Async notification for incoming close channel petition.
 * 
 * Functions using this handler:
 *   - \ref vortex_profiles_register
 *   - \ref vortex_channel_new
 * 
 * This handler allow you to control what to do on close channel
 * notification. Close notification is received when remote peer
 * wants to close some channel. Vortex library will ensure to 
 * handle automatically wrong close notification. 
 * 
 * Once again, the handler must return TRUE if channel can be close or
 * FALSE if not.  Once the handler is executed, Vortex library will
 * respond to remote peer using the BEEP protocol definition.
 * 
 * @param channel_num the channel num requesting to be closed.
 * @param connection the connection where the channel resides.
 * @param user_data user defined data passed in to this Async notifier.
 * 
 * @return TRUE if channel can be closed or FALSE if not.
 */
typedef  gboolean (*VortexOnCloseChannel)      (gint channel_num,
						VortexConnection * connection,
						gpointer user_data);

// VortexOnFrameReceived:
// ~~~~~~~~~~~~~~~~~~~~~~

/** 
 * @brief Async notifier for frame received event.
 *
 * Functions using this handler:
 *   - \ref vortex_profiles_register
 *   - \ref vortex_channel_new
 * 
 * This handler allows you to control received frames from remote
 * peers. Among the parameters you will received the frame, which
 * contains all frame definition: frame type, payload, content_type,
 * etc, the channel_num where the frame was received and the
 * connection (or session) where the channel belongs to.
 *
 * You can also pass in a user defined pointer, user_data, to function
 * which make use of this handler.
 *
 * You must not free received frame. Vortex library will do it for you.
 * Remember that, if you want to have permanent access to frame payload,
 * make a copy. After running this handler, Vortex library will free
 * all VortexFrame resources.
 * 
 * The channel parameter is the actual channel where the frame was
 * received. The connection parameter is the connection where the
 * channel exists. The frame parameter is the current frame received
 * and user_data parameter, if defined, holds a user pointer.
 *
 * 
 * @param channel the channel where the frame have been received.
 * @param connection the connection where the channel is running
 * @param frame the frame received
 * @param user_data user defined data passed in to this async notifier.
 */
typedef  void     (*VortexOnFrameReceived)     (VortexChannel    * channel,
						VortexConnection * connection,
						VortexFrame      * frame,
						gpointer user_data);


/** 
 * @brief Async notifier for channel creation process.
 *
 * Function using this handler:
 *  - \ref vortex_channel_new
 *
 * This handler allows you to define a notify function that will be
 * called once the channel is created or something have failed with
 * errors such as timeout reached or channel creation process
 * failures.
 *
 * If something fails, a -1 will be received on channel_num and
 * channel will be NULL.  Otherwise, you will received the channel
 * number created and a reference to channel created.  It is not
 * needed to hold a reference to channel. You can get it by using
 * \ref vortex_connection_get_channel.
 * 
 * 
 * @param channel_num the channel number for the new channel created
 * @param channel the channel created
 * @param user_data user defined data passed in to this async notifier.
 */
typedef void      (*VortexOnChannelCreated)  (gint            channel_num,
					      VortexChannel * channel,
					      gpointer        user_data);

/** 
 * @brief Async notifier for Vortex Channel Pool creation.
 *
 * Function using this handler:
 *  - \ref vortex_channel_pool_new
 *
 * This handler allows to make an async notification once the channel
 * pool process have finished. Because the channel pool creation
 * process can take a long time this handler is really useful to keep
 * on doing other things such as interface updating.
 * 
 * @param pool the channel pool created
 * @param user_data user defined data passed in to this async notifier.
 */
typedef void      (*VortexOnChannelPoolCreated) (VortexChannelPool * pool,
						 gpointer user_data);

/** 
 * @brief Async notifier for the channel close process.
 *
 * Function using this handler:
 *  - \ref vortex_channel_close
 *
 * This handler allows you to defined a notify function that will be
 * called once the channel close indication is received. This is
 * mainly used by \ref vortex_channel_close to avoid caller get
 * blocked by calling that function.
 *
 * Data received is the channel num this notification applies to, the
 * channel closing status and if was_closed is FALSE (the channel was
 * not closed) code hold error code returned and msg the message
 * returned by remote peer. 
 *
 * @param channel_num the channel num identifying the channel closed
 * @param was_closed status for the channel close process.
 * @param code the code representing the channel close process.
 * @param msg the message representing the channel close process.
 */
typedef void     (*VortexOnClosedNotification) (gint            channel_num,
						gboolean        was_closed,
						gchar         * code,
						gchar         * msg);
						

#endif

// @}
