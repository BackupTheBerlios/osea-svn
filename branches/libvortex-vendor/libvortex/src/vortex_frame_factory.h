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
#ifndef __VORTEX_FRAME_FACTORY_H__
#define __VORTEX_FRAME_FACTORY_H__

#include <vortex.h>

gchar *        vortex_frame_build_up_from_params (VortexFrameType   type,
						  gint32            channel,
						  gint32            msgno,
						  gboolean          more,
						  guint32           seqno,
						  gint32            size,
						  gint32            ansno,
						  gchar           * payload);

gchar *       vortex_frame_get_raw_frame         (VortexFrame * frame);

VortexFrame * vortex_frame_get_next     (VortexConnection * connection);

gboolean      vortex_frame_send_raw     (VortexConnection * connection, gchar * a_frame, gint frame_size);

void          vortex_frame_free         (VortexFrame * frame);

VortexFrame * vortex_frame_join         (VortexFrame * a, VortexFrame * b);

VortexFrameType vortex_frame_get_type   (VortexFrame * frame);

gchar       * vortex_frame_get_content_type (VortexFrame * frame);

gint          vortex_frame_get_channel  (VortexFrame * frame);

gint          vortex_frame_get_msgno    (VortexFrame * frame);

gint          vortex_frame_get_more_flag (VortexFrame * frame);

gint          vortex_frame_get_seqno    (VortexFrame * frame);

gint          vortex_frame_get_payload_size (VortexFrame * frame);

gint          vortex_frame_get_ansno    (VortexFrame * frame);

gchar       * vortex_frame_get_payload  (VortexFrame * frame);



#endif
