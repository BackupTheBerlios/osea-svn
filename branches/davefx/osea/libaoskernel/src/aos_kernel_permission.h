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

#ifndef __AFDAL_KERNEL_PERMISSION_INTERFACE_H__
#define __AFDAL_KERNEL_PERMISSION_INTERFACE_H__

#include "aos_kernel.h"

typedef struct __AfDalKernelPermission {
	gint id;
	gchar   * name;
	gchar   * description;
	gchar   * server_name;
	gchar   * server_version;
	gchar   * depends;
}AfDalKernelPermission;

gboolean           aos_kernel_permission_list                      (gint           initial_permission, 
								      gint           max_row_number,
								      AfDalDataFunc  usr_function, 
								      gpointer       usr_data);

gboolean           aos_kernel_permission_list_by_user              (gint           initial_permission, 
								      gint           max_row_number,
								      gint           user_id,
								      AfDalDataFunc  usr_function, 
								      gpointer       usr_data);

gboolean           aos_kernel_permission_actual_list_by_user       (gint           initial_permission, 
								      gint           max_row_number,
								      gint           user_id,
								      AfDalDataFunc  usr_function, 
								      gpointer       usr_data);

gboolean           aos_kernel_permission_from_group_list_by_user   (gint           initial_permission, 
								      gint           max_row_number,
								      gint           user_id,
								      AfDalDataFunc  usr_function, 
								      gpointer       usr_data);


gboolean           aos_kernel_permission_list_by_group             (gint           initial_permission, 
								      gint           max_row_number,
								      gint           group_id,
								      AfDalDataFunc  usr_function, 
								      gpointer       usr_data);

gboolean           aos_kernel_permission_user_set                  (gint           id,
								      gint           user_id,
								      AfDalNulFunc   usr_function, 
								      gpointer       usr_data);

gboolean           aos_kernel_permission_user_unset                (gint           id,
								      gint           user_id,
								      AfDalNulFunc   usr_function, 
								      gpointer       usr_data);

gboolean            aos_kernel_permission_user_set_value           (gint           id, 
								      gint           user_id, 
								      gboolean       value, 
								      AfDalNulFunc   usr_func, 
								      gpointer       usr_data);
                   
gboolean           aos_kernel_permission_group_set                 (gint           id,
								      gint           group_id,
								      AfDalNulFunc   usr_function, 
								      gpointer       usr_data);

gboolean           aos_kernel_permission_group_unset               (gint           id,
								      gint           group_id,
								      AfDalNulFunc   usr_function, 
								      gpointer       usr_data);

gboolean            aos_kernel_permission_group_set_value          (gint           id, 
								      gint           user_id, 
								      gboolean       value, 
								      AfDalNulFunc   usr_func, 
								      gpointer       usr_data);
                   

#endif 
