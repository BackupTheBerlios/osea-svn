//  Af-Kernel server: 
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

#ifndef __OS_KERNEL_LOGIN_REQUEST_H__
#define __OS_KERNEL_LOGIN_REQUEST_H__

#include <glib.h>
#include <liboseacomm/oseacomm.h>
#include "os_kernel_register_request.h"

gboolean os_kernel_login_check (gchar * user, gchar * password);

gboolean os_kernel_login_request_build_active_servers (gchar * key,  
						       AfKernelActiveServer * value, 
						       gpointer data);

gboolean os_kernel_login_request                      (OseaCommXmlServiceData *data, 
						       gpointer user_data, 
						       RRChannel * channel, 
						       gint msg_no);

#endif
