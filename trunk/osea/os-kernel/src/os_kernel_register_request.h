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

#ifndef __OS_KERNEL_REGISTER_REQUEST_H__
#define __OS_KERNEL_REGISTER_REQUEST_H__

#include <glib.h>
#include <liboseacomm/oseacomm.h>

typedef struct __AfKernelActiveServer {
	gchar * name;
	gchar * host;
	gchar * port;
} AfKernelActiveServer;


gboolean      os_kernel_register_request               (OseaCommXmlServiceData *data, 
							gpointer user_data, 
							RRChannel * channel, 
							gint msg_no);

/* gboolean      os_kernel_register_list_request          (OseaCommXmlServiceData *data,  */
/* 							gpointer user_data,  */
/* 							RRChannel * channel,  */
/* 							gint msg_no); */

gboolean      os_kernel_unregister_request             (OseaCommXmlServiceData *data, 
							gpointer user_data,
							RRChannel * channel, 
							gint msg_no);

const GTree * os_kernel_register_get                   ();

/* gboolean      os_kernel_register_build_active_servers  (gchar * key,   */
/* 						        AfKernelActiveServer * value,  */
/* 							gpointer data); */

gboolean      os_kernel_register_commit                (gchar * name, 
							gchar * host, 
							gchar *port);
#endif
