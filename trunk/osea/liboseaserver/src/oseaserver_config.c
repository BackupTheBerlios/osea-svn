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

#include "oseaserver_config.h"
#include "oseaserver_main.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define LOG_DOMAIN "oseaserver-config"

gchar             * default_accepted_keys[] = { "listening port",
						"listening hostname",
						"kernel server",
						"kernel port",
						"database server",
						"database port",
						"database name",
						"database user",
						"database password",
						"log file", 
						"log active",
						"afkey expiration",
						NULL
};


struct _oseaserver_configuration {
	GTree *tree;
};

static GTree *valid_keys = NULL;

static AfgsConfiguration *last_cfg = NULL;

gchar *__read_line (FILE *f)
{
	gchar c[2];
	GString *r = NULL;
	gchar *result = NULL;

	c[1]='\0';

	r = g_string_new (NULL);

	do {
		c[0] = fgetc (f);

		if ( strcspn (c, "#\n") == 1) {
			// If it is not end of line or sharp, add it to buffer.
			g_string_sprintfa (r, "%c", c[0]);
		} else {
			// If it is, finish line.
			result = r->str;
			g_string_free (r, FALSE);
		}
					
	} while ( (! result) && (! feof(f)) );

	// If it's a sharp, read until end of line
	if (! strcmp (c,"#"))
		while ((strcspn (c, "\n") ))
			c[0] = fgetc(f);
	
	return result;

}


gint __tree_compare (gconstpointer a, gconstpointer b, gpointer data)
{
	return g_ascii_strcasecmp (a, b);
}

gboolean __tree_print (gpointer a, gpointer b, gpointer data)
{
	g_print ("%s = %s\n", (gchar *)a, (gchar*) b);
	return FALSE;
}


void __init ( gchar ** accepted_keys)
{
	gint i = 0;
	gchar *aux;
	aux = accepted_keys[i];

	valid_keys = g_tree_new_full (__tree_compare, NULL, NULL, NULL);

	while (aux) {
		g_tree_insert (valid_keys, aux, aux);
		aux = accepted_keys[++i];
	}

	i = 0;
	aux = default_accepted_keys[i];
	
	while (aux) {
		g_tree_insert (valid_keys, aux, aux);
		aux = default_accepted_keys[++i];
	}

}


/**
 * oseaserver_config_load:
 * @filename: full path to an oseaserver configuration file.
 * @accepted_keys: a string list of accepted keys
 * 
 * Loads an oseaserver configuration file pointed by
 * @filename. @accepted_keys is used to set up which keys are
 * considered while parsing @filename. So is you want to consider two
 * keys such as: example1 and foo2 you have to do a call as follow:
 *
 * gchar ** accepted_keys = {"example1", "foo2", NULL};
 * oseaserver_config_load (wherever_config_file_is, accepted_keys)
 * 
 * Return value: A configuration object
 **/
AfgsConfiguration *oseaserver_config_load (gchar *filename, gchar ** accepted_keys) 
{
	AfgsConfiguration *cfg = NULL;
	FILE *file = NULL;
	gchar *line = NULL;
	gchar *aux = NULL;
	gchar **str_array = NULL; 
	gchar *key = NULL;
	gchar *value = NULL;

	if (! valid_keys)
		__init(accepted_keys);

	file = fopen (filename, "r");

	if (! file) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't open config file %s", filename);		
		return NULL;
	}

	cfg = g_new (AfgsConfiguration, 1);
	cfg->tree = g_tree_new_full (__tree_compare, NULL, g_free, g_free);

	while (! feof(file)) {
		line = __read_line (file);
		if ((line) && (strcmp(line,""))) {
			aux = line;
			
			str_array = g_strsplit (aux, "=", 2);
			
			key = g_strdup (str_array[0]);
			key = g_strstrip (key);

			if (strcmp (key, "")) {
				if (! g_tree_lookup (valid_keys, key)) {
					g_log (LOG_DOMAIN,
					       G_LOG_LEVEL_WARNING,
					       "Invalid key in config file: '%s'", key);
				} else {
					
					value = g_strdup (str_array[1]);
					value = g_strstrip (value);
					
					g_tree_insert (cfg->tree, 
						       key, value);
				}
			}

			g_strfreev (str_array);
		}
	}
	
	fclose (file);
	// g_tree_foreach (cfg->tree, tree_print, NULL);

	last_cfg = cfg;

	return cfg;
}



/**
 * oseaserver_config_get:
 * @config: an oseaserver configuration object or NULL
 * @key: key to search for
 * 
 * Gets the value associted to the given @key. @config object must be
 * a valid oseaserver configuration object obtained from afgs_config_load function. 
 * If you want to have access to your configuration you can simply do:
 * 
 * string = oseaserver_config_get (NULL, "a key");
 * 
 * By passing NULL to first param you are telling to oseaserver_config_get to use
 * the last config created.
 * You must free the result after use it.
 *  
 * Return value: A value or NULL if @key weren't defined.
 **/
gchar *oseaserver_config_get (AfgsConfiguration *config, gchar *key)
{
	gchar *result;

	g_return_val_if_fail (key, NULL);

	if (!config)
		config = last_cfg;

	g_return_val_if_fail (config, NULL);

	result =  g_tree_lookup (config->tree, key);

	if (result)
		return g_strdup(result);
	else
		return NULL;
}


/**
 * oseaserver_config_destroy:
 * @config: 
 * 
 * Frees memory allocated by @config object.
 **/
void oseaserver_config_destroy (AfgsConfiguration *config) 
{
	if (!config)
		config = last_cfg;

	g_return_if_fail (config);
	
	g_tree_destroy (config->tree);
	g_free (config);

}

void  	            oseaserver_config_check_permissions (gchar * config_file)
{
	struct stat file_info;
	
	if (!config_file) 
		oseaserver_main_abort ("config file not defined");

	// Others and groups must be 0
	if (stat (config_file, &file_info) == -1) 
		oseaserver_main_abort ("unable to access config file %s", config_file);

	
	if (file_info.st_mode & 0077) {
		oseaserver_main_abort ("incorrect permission on config file. Not secure. Try chmod 0600 %s",
				 config_file);
	}

	return;
}
