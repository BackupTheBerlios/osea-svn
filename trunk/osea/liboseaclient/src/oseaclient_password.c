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

#include "oseaclient_password.h"
#include <unistd.h>

gboolean    oseaclient_password_get_default_callback (AfDalPasswordData *pwd_data);

static AfDalPasswordCallback __oseaclient_password_callback = NULL;

void    oseaclient_password_set_callback (AfDalPasswordCallback cb)
{
	__oseaclient_password_callback = cb;

	return;
}

gboolean    oseaclient_password_get (AfDalPasswordNotifyCallback notify_cb, gpointer user_data)
{
	AfDalPasswordData *pwd_data = g_new0 (AfDalPasswordData, 1);
	
	pwd_data->notify_cb = notify_cb;
	pwd_data->user_data = user_data;

	if (__oseaclient_password_callback) {
		oseaclient_event_source_launch ((GSourceFunc) __afdal_password_callback, pwd_data);
		return TRUE;
	}
	
	return FALSE;
}


