//  ASPL Fact: invoicing client program for ASPL Fact System
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

#include "settings.h"
#include <gnome.h>
#include <gconf/gconf-client.h>

gboolean settings_init()
{
	gchar  * aux = NULL;
	gboolean result = FALSE;
	
	aux = settings_get ("hostname", SETTINGS_STRING);

	if (aux)
		result = TRUE;
	else
		result = FALSE;
	
	g_free (aux);
	return result;
}

gchar * settings_get (gchar * key, SettingType type)
{
	GConfClient * g_client = NULL;
	gchar       * data = NULL;
	GError      * error = NULL;
	GString     * string = NULL;
	gint          data_aux;
	GConfValue  * value = NULL;

	g_return_val_if_fail (key, NULL);

	g_client = gconf_client_get_default ();

	if (!g_client) {
		g_printerr (_("Error: cannot obtain connection with gconf\n"));
		return NULL;
	}

	string = g_string_new (NULL);
	g_string_sprintf (string, "/apps/oinvoice/%s", key);
	
	value = gconf_client_get (g_client, string->str, &error);
	if (! value)
		return NULL;

	switch (type) {
	case SETTINGS_STRING:
		data = gconf_client_get_string (g_client, string->str, &error);
		break;
	case SETTINGS_INT:
		data_aux = gconf_client_get_int (g_client, string->str, &error);
		g_string_free (string, TRUE);

		string = g_string_new (NULL);
		g_string_sprintf (string, "%d", data_aux);
		data = g_strdup(string->str);		
		break;
	default:
		g_assert(NULL);
	}

	g_string_free (string, TRUE);

	if (value)
		g_free (value);
	g_object_unref (G_OBJECT (g_client));
	
	return data;
}

gboolean settings_set (gchar * key, gchar *  data, SettingType type)
{
	GConfClient * g_client = NULL;
	gboolean      result = FALSE;
	GError      * error = NULL;
	GString     * string = NULL;


	g_return_val_if_fail (key, FALSE);
	
	g_return_val_if_fail (data, FALSE);

	g_client = gconf_client_get_default ();

	if (!g_client) {
		g_printerr (_("Error: cannot obtain connection with gconf\n"));
		return FALSE;
	}
	
	string = g_string_new (NULL);
	g_string_sprintf (string, "/apps/oinvoice/%s", key);
	

	switch (type) {
	case SETTINGS_STRING:
		result = gconf_client_set_string (g_client, string->str, data,  &error);
		break;
	case SETTINGS_INT:
		result = gconf_client_set_int (g_client, string->str, atoi(data), &error);
		break;
	default:
		g_assert(NULL);
	}

	g_string_free (string, TRUE);

	if (!result) {
		g_printerr (_("Error: Cannot set required data (Key: '%s'. Value: '%s'): %s\n"),
			    string->str, data, error->message);
		g_error_free (error);

		return FALSE;
	}

	g_object_unref (G_OBJECT (g_client));

	return result;
}

