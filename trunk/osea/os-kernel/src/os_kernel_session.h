//  Af-Kernel server: 
//  Copyright (C) 2002, 2003  Advanced Software Production Line, S.L.
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

#ifndef __OS_KERNEL_SESSION_H__
#define __OS_KERNEL_SESSION_H__

#include <glib.h>

gchar *  os_kernel_session_new (gint user, gboolean expire);

void     os_kernel_session_remove (gchar *id);

gboolean os_kernel_session_is_current (gchar *id);

gboolean os_kernel_session_refresh (gchar *id);

gboolean os_kernel_session_belongs (gchar *id, gchar *user);

#endif
