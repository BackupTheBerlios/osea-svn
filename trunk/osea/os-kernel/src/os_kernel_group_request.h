//  Aspl-rule server: Daemon for server side for rule report system
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

#ifndef __OS_KERNEL_GROUP_REQUEST_H__
#define __OS_KERNEL_GROUP_REQUEST_H__

#include <glib.h>
#include <liboseacomm/oseacomm.h>

gboolean os_kernel_group_list    (OseaCommXmlServiceData * data, 
				  gpointer               group_data, 
				  RRChannel            * channel, 
				  gint                   msg_no);

gboolean os_kernel_group_new     (OseaCommXmlServiceData * data, 
				  gpointer               group_data, 
				  RRChannel            * channel, 
				  gint                   msg_no);

gboolean os_kernel_group_edit    (OseaCommXmlServiceData * data, 
				  gpointer               group_data, 
				  RRChannel            * channel, 
				  gint                   msg_no);

gboolean os_kernel_group_remove  (OseaCommXmlServiceData * data,
				  gpointer               group_data, 
				  RRChannel            * channel, 
				  gint                   msg_no);

gboolean os_kernel_group_add_user  (OseaCommXmlServiceData * data,
				    gpointer               group_data, 
				    RRChannel            * channel, 
				    gint                   msg_no);

gboolean os_kernel_group_remove_user  (OseaCommXmlServiceData * data,
				       gpointer               group_data, 
				       RRChannel            * channel, 
				       gint                   msg_no);

gboolean os_kernel_group_list_user  (OseaCommXmlServiceData * data,
				     gpointer               group_data, 
				     RRChannel            * channel, 
				     gint                   msg_no);




#endif


