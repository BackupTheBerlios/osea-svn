//
//  LibAosKernel: interface library to the kernel daemon
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

#ifndef __AOS_KERNEL_REGISTER_INTERFACE_H__
#define __AOS_KERNEL_REGISTER_INTERFACE_H__

#include "aos_kernel.h"

typedef struct __AosKernelServerInfo {
	gchar * name;
	gchar * host;
	gchar * port;
}AosKernelServerInfo;


gboolean aos_kernel_register_get_servers  (OseaClientDataFunc  usr_function, 
					     gpointer       usr_data);

#endif
