//
//  LibAfdal: common functions to liboseaclient* level and architectural functions.
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

#ifndef __AFDAL_PASSWORD_H__
#define __AFDAL_PASSWORD_H__
#include <glib.h>
#include "oseaclient.h"

typedef void    (* AfDalPasswordNotifyCallback) (gchar *password, gpointer user_data);

typedef struct {
	AfDalPasswordNotifyCallback notify_cb;
	gpointer user_data;
} AfDalPasswordData;

typedef gboolean (* AfDalPasswordCallback)       (AfDalPasswordData * pwd_data);

void      oseaclient_password_set_callback (AfDalPasswordCallback cb);

gboolean  oseaclient_password_get          (AfDalPasswordNotifyCallback notify_cb, gpointer user_data);

#endif
