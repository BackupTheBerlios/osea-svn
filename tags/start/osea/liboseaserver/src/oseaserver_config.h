//  Configuration file reading module. 
//  Copyright (C) 2003  Advanced Software Production Line, S.L.
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

#include <glib.h>

#ifndef __AFGS_CONFIG_H__
#define __AFGS_CONFIG_H__

typedef struct _oseaserver_configuration AfgsConfiguration;

AfgsConfiguration * oseaserver_config_load      (gchar *filename, gchar ** accepted_keys);

gchar             * oseaserver_config_get       (AfgsConfiguration *config, gchar *key);

void                oseaserver_config_destroy   (AfgsConfiguration *config);

void  	            oseaserver_config_check_permissions (gchar * config_file);

#endif
