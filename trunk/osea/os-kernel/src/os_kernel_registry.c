//  Af-Kernel server: 
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <liboseacomm/oseacomm.h>
#include <liboseaserver/oseaserver.h>
#include "os_kernel_keys.h"

#define LOG_DOMAIN "os-kernel_registry"


gboolean       os_kernel_registry_insert (gchar * name, gchar * host, gchar *port) 
{
	gboolean result;

	oseaserver_command_execute_non_query ("DELETE FROM Kernel_Registry WHERE name='%s'", name);

	result = oseaserver_command_execute_non_query ("INSERT INTO Kernel_Registry \
                 (name, host, port, stime) VALUES \
                 ('%s', '%s', %s, current_timestamp);", name, host, port);

	if (! result)
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't insert a new server into database");

	return TRUE;
}


OseaCommDataSet * os_kernel_registry_list ()
{
	OseaCommDataSet    * result = NULL;

	result = oseaserver_command_execute_single_query ("SELECT name, host, port FROM Kernel_Registry");	
	
	return result;
}

OseaCommDataSet * os_kernel_registry_generate_session_info_table (gchar *user, gchar **kernel_afkey)
{
	OseaCommDataSet * result;
	gchar         * af_key  = NULL;
	gchar         * server_name;
	gint            i;

	result = oseaserver_command_execute_single_query ("select name, host, port, 'afkey' AS afkey from kernel_registry");

	for (i=0; i < oseacomm_dataset_get_height (result); i++) {
		server_name = (gchar *) oseacomm_dataset_get (result, i, 0);
		af_key = os_kernel_keys_generate (user, server_name);
		oseacomm_dataset_set (result, af_key, i, 3);

		// Check if af_key is os-kernel key for user
		if (!g_strcasecmp (oseacomm_dataset_get (result, i,0), "os-kernel")) {
			// Store os-kernel af-key generated to use it before
			*kernel_afkey = g_strdup(af_key);
		}

		g_free (af_key);

	}

	return result;
}


gboolean os_kernel_registry_remove (gchar *name)
{
	return 	oseaserver_command_execute_non_query ("DELETE FROM Kernel_Registry WHERE name='%s'", name);
}
