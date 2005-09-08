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
#ifndef __VORTEX_H__
#define __VORTEX_H__

#include <glib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <vortex_types.h>
#include <vortex_thread_pool.h>
#include <vortex_semaphore.h>
#include <vortex_queue.h>
#include <vortex_hash.h>
#include <vortex_handlers.h>
#include <vortex_connection.h>
#include <vortex_listener.h>
#include <vortex_frame_factory.h>
#include <vortex_greetings.h>
#include <vortex_profiles.h>
#include <vortex_channel.h>
#include <vortex_channel_pool.h>
#include <vortex_reader.h>
#include <vortex_dtds.h>
#include <vortex_sequencer.h>
#include <vortex_writer.h>
#include <vortex_channel_pool.h>
#include <vortex_errno.h>

#ifdef G_OS_UNIX
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#endif


#ifdef G_OS_WIN32
#include <vortex_win32.h>
#else
#include <errno.h>
#endif

#ifdef G_OS_UNIX
#define VORTEX_INVALID_SOCKET -1
#define VORTEX_SOCKET_ERROR   -1
#define vortex_close_socket close
#define vortex_is_disconnected (errno == EPIPE)
#endif
#ifdef G_OS_WIN32
#define VORTEX_INVALID_SOCKET INVALID_SOCKET
#define VORTEX_SOCKET_ERROR   SOCKET_ERROR
#define vortex_close_socket closesocket
#define vortex_is_disconnected ((errno == WSAESHUTDOWN) || (errno == WSAECONNABORTED) || (errno == WSAECONNRESET))
#endif


void     vortex_init ();

void     vortex_exit ();

gboolean vortex_log_is_enabled ();

#endif
