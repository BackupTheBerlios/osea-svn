//  ASPL Fact Log facilities: 
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

#ifndef __OSEA_SERVER_LOG_H__
#define __OSEA_SERVER_LOG_H__

#include <glib.h>
#include <liboseaclient/oseaclient.h>
#include "oseaserver_config.h"

void oseaserver_log_write (gchar * log_msg, ...);

void oseaserver_log_vwrite (gchar * log_msg, va_list args);

void oseaserver_log_check_and_disable_glog ();

#endif 
