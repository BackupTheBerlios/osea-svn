//
//  LibAosKernel: interface library to the kernel daemon
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//  Copyright (C) 2004 David Mar�n Carre�o
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

#ifndef __AOS_KERNEL_H__
#define __AOS_KERNEL_H__

#include <glib.h>
#include <liboseaclient/oseaclient.h>

#include "aos_kernel_register.h"
#include "aos-kernel-user.h"
#include "aos_kernel_user.h"
#include "aos-kernel-group.h"
#include "aos_kernel_group.h"
#include "aos_kernel_permission.h"
#include "aos_kernel_server.h"

void aos_kernel_print_version_info ();

#endif
