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
#ifndef __VORTEX_CONNECTION_H__
#define __VORTEX_CONNECTION_H__

#include <vortex.h>


VortexConnection  * vortex_connection_new                    (gchar * host, gchar * port,
							      VortexConnectionNew on_connected, 
							      gpointer user_data);

gboolean            vortex_connection_close                  (VortexConnection * connection);

void                vortex_connection_ref                    (VortexConnection * connection,
							      gchar            * who);

void                vortex_connection_unref                  (VortexConnection * conneciton,
							      gchar            * who);

VortexConnection  * vortex_connection_new_empty              (gint session);

void                vortex_connection_timeout                (glong miliseconds_to_wait);

gboolean            vortex_connection_is_ok                  (VortexConnection * connection, 
							      gboolean free_on_fail);

gchar             * vortex_connection_get_message            (VortexConnection * connection);

void                vortex_connection_free                   (VortexConnection * connection);

GList             * vortex_connection_get_remote_profiles    (VortexConnection * connection);

gboolean            vortex_connection_is_profile_supported   (VortexConnection * connection, 
							      gchar * uri);

gboolean            vortex_connection_channel_exists         (VortexConnection * connection, 
							      gint channel_num);

void                vortex_connection_foreach_channel        (VortexConnection * connection,
							      GHFunc func,
							      gpointer user_data);

gint                vortex_connection_get_next_channel       (VortexConnection * conection);

VortexChannel     * vortex_connection_get_channel            (VortexConnection * connection, 
							      gint channel_num);

gint                vortex_connection_get_socket             (VortexConnection * connection);

void                vortex_connection_add_channel            (VortexConnection * connection, 
							      VortexChannel * channel);

void                vortex_connection_remove_channel         (VortexConnection * connection, 
							      VortexChannel * channel);

gchar             * vortex_connection_get_host               (VortexConnection * connection);

gchar             * vortex_connection_get_port               (VortexConnection * connection);

gint                vortex_connection_get_id                 (VortexConnection * connection);

gboolean            vortex_connection_set_blocking_socket    (VortexConnection * connection);

gboolean            vortex_connection_set_nonblocking_socket (VortexConnection * connection);

void                vortex_connection_set_data               (VortexConnection * connection,
							     gchar            * key,
							     gpointer           value);

gpointer            vortex_connection_get_data               (VortexConnection * connection,
							      gchar            * key);

VortexChannelPool * vortex_connection_get_channel_pool       (VortexConnection * connection,
							      gint               pool_id);

gint                vortex_connection_get_pending_msgs       (VortexConnection * connection);

// do not use the following functions, internal vortex library purposes
void                __vortex_connection_set_not_connected    (VortexConnection * connection, 
							      gchar            * message);

gint                vortex_connection_do_a_sending_round     (VortexConnection * connection);

void                vortex_connection_lock_channel_pool      (VortexConnection * connection);

void                vortex_connection_unlock_channel_pool    (VortexConnection * connection);

gint                vortex_connection_next_channel_pool_id   (VortexConnection * connection);

void                vortex_connection_add_channel_pool       (VortexConnection  * connection,
							      VortexChannelPool * pool);
#endif
