//
//  LibAfdalKernel: interface library to the kernel daemon
//  Copyright (C) 2002  Advanced Software Production Line, S.L.
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
#ifndef __AOS_KERNEL_USER_INTERFACE_H__
#define __AOS_KERNEL_USER_INTERFACE_H__

#include "aos_kernel.h"

gboolean           aos_kernel_user_list        (gint           initial_user, 
						  gint           max_row_number,
						  OseaClientDataFunc  usr_function, 
						  gpointer       usr_data);

gboolean           aos_kernel_user_new         (gchar           * nick, 
						  gchar           * password, 
						  gchar           * description,
						  OseaClientSimpleFunc   usr_function, 
						  gpointer          usr_data);

gboolean           aos_kernel_user_remove      (gint id,
						  OseaClientNulFunc   usr_function,
						  gpointer       usr_data);

gboolean           aos_kernel_user_edit        (gint           id,
						  gchar        * new_nick, 
						  gchar        * new_password, 
						  gchar        * new_description,
						  OseaClientNulFunc   usr_function, 
						  gpointer       usr_data);

gboolean          aos_kernel_user_list_group   (gint           initial_group, 
						  gint           max_row_number,
	                                          gint           user_id,
						  OseaClientDataFunc  usr_function, 
						  gpointer       usr_data);
#endif
