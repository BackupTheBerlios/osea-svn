//  LibOseaComm:  Support library with xml and communication functions.
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//  Copyright (C) 2004 David Marín Carreño
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or   
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef __OSEACOMM_H__
#define __OSEACOMM_H__
#include <glib.h>

#include "oseacomm_partial_message.h"
#include "oseacomm_peer_info.h"

gboolean oseacomm_init                      (gint *argc, gchar ***argv, GError **error);

gboolean oseacomm_set_properties            (gchar *property_name,
					     gpointer value, ...);

gboolean oseacomm_connect                   (gchar *peer_name);
gboolean oseacomm_disconnect                (gchar *peer_name);

gboolean oseacomm_set_connection_properties (gchar *peer_name,
					     gchar *property_name,
					     gpointer value, ...);


gint     oseacomm_send                      (gchar *peer_name,
			                     OseaCommMessage *message);

typedef  gboolean (*OseaCommReplyReceive)   (gint message_id,
					     gint error_code,
					     OseaCommMessage *message,
					     gboolean is_last);


gboolean oseacomm_set_listener_properties   (gchar *property_name,
					     gpointer value, ...);

gboolean oseacomm_wait                      (gint port);

typedef  gboolean (*OseaCommMessageReceive) (gint message_id,
					     OseaCommPeerInfo *peer_info,
					     OseaCommMessage *message,
					     gboolean is_last);

gboolean oseacomm_reply                     (gint message_id,
					     gint error_code,
					     OseaCommMessage *message);

gboolean oseacomm_exit (GError **error);

#endif
