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
#ifndef __VORTEX_CHANNEL_POOL_H__
#define __VORTEX_CHANNEL_POOL_H__

#include <vortex.h>

VortexChannelPool * vortex_channel_pool_new               (VortexConnection           * connection,
							   gchar                      * profile,
							   gint                         max_num,
							   VortexOnCloseChannel         close,
							   gpointer                     close_user_data,
							   VortexOnFrameReceived        received,
							   gpointer                     received_user_data,
							   VortexOnChannelPoolCreated   on_channel_pool_created,
							   gpointer                     user_data);

gint                vortex_channel_pool_get_num           (VortexChannelPool * pool);

void                vortex_channel_pool_add               (VortexChannelPool * pool,
							   gint num);

void                vortex_channel_pool_remove            (VortexChannelPool * pool,
							   gint num);
						 
void                vortex_channel_pool_close             (VortexChannelPool * pool);

void                vortex_channel_pool_attach            (VortexChannelPool * pool,
							   VortexChannel     * channel);
  
void                vortex_channel_pool_deattach          (VortexChannelPool * pool,
							   VortexChannel     * channel);

VortexChannel     * vortex_channel_pool_get_next_ready    (VortexChannelPool * pool,
							   gboolean auto_inc);

void                vortex_channel_pool_release_channel   (VortexChannelPool * pool,
							   VortexChannel     * channel);

gint                vortex_channel_pool_get_id            (VortexChannelPool * pool);

VortexConnection  * vortex_channel_pool_get_connection    (VortexChannelPool * pool);

#endif
