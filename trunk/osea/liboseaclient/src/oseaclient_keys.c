//
//  LibOseaClient: common functions to liboseaclient* level and architectural functions.
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

#include "oseaclient_keys.h"
#include <stdlib.h>
#include <string.h>

OseaClientAfKey * ___oseaclient_afkeys_parse   (gchar * af_key)
{
	OseaClientAfKey  * result_key;
	gchar      ** splited_string;
	gchar      ** argv_permission;
	gint          int_time_stamp;
	gchar       * aux_string;
	gint          iterator;

	g_return_val_if_fail (af_key, NULL);

	// Get all fields contained at af_key.

	splited_string = g_strsplit (af_key, ";", 4);
	
	// Check af-key sintax
	g_return_val_if_fail (splited_string[0], NULL);
	g_return_val_if_fail (splited_string[1], NULL);
	g_return_val_if_fail (splited_string[2], NULL);
	g_return_val_if_fail (splited_string[3], NULL);
	
	// All fields seems to be good

	// Get af-key time_stamp
	int_time_stamp = strtol (splited_string[2], &aux_string, 10);
	if (strlen (aux_string) != 0) {
		g_strfreev (splited_string);
		return FALSE;
	}
	
	// Get af-key user and associated server
	result_key = g_new0 (OseaClientAfKey, 1);

	result_key->user = g_strdup (splited_string[0]);
	result_key->satellite_server = g_strdup (splited_string[1]);
	result_key->time_stamp = int_time_stamp;
	
	// Get permission list
	argv_permission = g_strsplit (splited_string[3],",", 0);

	for (iterator = 0; argv_permission[iterator]; iterator++) {
		result_key->permission_list = g_list_append (result_key->permission_list, 
							     g_strdup(argv_permission[iterator]));
	}
	
	
	// Free memory
	g_strfreev (splited_string);
	g_strfreev (argv_permission);

	return result_key; 
}

void         ___oseaclient_afkeys_destroy (OseaClientAfKey * af_key, 
				      gboolean     free_permission_list)
{
	GList *cursor;

	g_return_if_fail (af_key);

	g_free (af_key->user);
	g_free (af_key->satellite_server);

	if (free_permission_list) {
		cursor = g_list_first (af_key->permission_list);
		while (cursor) {
			g_free (cursor->data);

			cursor = g_list_next(cursor);
		}
		
		g_list_free (af_key->permission_list);
	}
	
	g_free (af_key);

	return;
	
}
