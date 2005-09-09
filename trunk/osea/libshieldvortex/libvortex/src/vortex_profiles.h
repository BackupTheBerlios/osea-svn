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
#ifndef __VORTEX_PROFILES_H__
#define __VORTEX_PROFILES_H__

#include <vortex.h>


void     vortex_profiles_init ();

void     vortex_profiles_register        (gchar                 * uri,
					  VortexOnStartChannel    start,
					  gpointer                start_user_data,
					  VortexOnCloseChannel    close,
					  gpointer                close_user_data,
					  VortexOnFrameReceived   received,
					  gpointer                received_user_data);

gboolean vortex_profiles_invoke_start     (gchar * uri,
					   gint channel_num, 
					   VortexConnection * connection);

gboolean vortex_profiles_is_defined_start (gchar * uri);

gboolean vortex_profiles_invoke_close     (gchar * uri,
					   gint channel_nu,
					   VortexConnection * connection);

gboolean vortex_profiles_is_defined_close (gchar * uri);

gboolean vortex_profiles_invoke_frame_received (gchar            * uri,
						gint               channel_num,
						VortexConnection * connection,
						VortexFrame      * frame);

gboolean vortex_profiles_is_defined_received (gchar * uri);


GList  * vortex_profiles_get_actual_list ();

gint     vortex_profiles_registered      ();

gboolean vortex_profiles_is_registered   (gchar * uri);

#endif
