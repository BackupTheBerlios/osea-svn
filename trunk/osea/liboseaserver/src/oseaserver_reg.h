//  ASPL Fact Server Register Connection: 
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

#ifndef __OSEA_SERVER_REG_H__
#define __OSEA_SERVER_REG_H__

#include <glib.h>
#include "oseaserver_config.h"
#include <liboseaclient/oseaclient.h>

gboolean oseaserver_reg_register   (const gchar *server_name, gint version_number, OseaClientNulFunc usr_function);

gboolean oseaserver_reg_unregister ();

#endif

