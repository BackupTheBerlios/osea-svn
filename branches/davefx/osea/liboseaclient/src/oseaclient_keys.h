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

#ifndef __AFDAL_AFKEYS_H__
#define __AFDAL_AFKEYS_H__
#include "oseaclient.h"



typedef struct __AfDalAfKey {
	gchar * user;
	gchar * satellite_server;
	gint    time_stamp;
	GList * permission_list;
}AfDalAfKey;


AfDalAfKey * ___oseaclient_afkeys_parse   (gchar * af_key);

void         ___oseaclient_afkeys_destroy (AfDalAfKey * af_key, 
				      gboolean     free_permission_list);

#endif
