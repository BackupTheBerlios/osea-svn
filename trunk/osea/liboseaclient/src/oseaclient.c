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
#include "oseaclient.h"
#include <time.h>
#include <config.h>


/**
 * oseaclient_nul_free:
 * @data: 
 * 
 * Frees OseaClientNulData @data.
 **/
void    oseaclient_nul_free    (OseaClientNulData * data)
{
	g_free (data->text_response);
	g_free (data);
	return;
}

/**
 * oseaclient_simple_free:
 * @data: 
 * 
 * Frees OseaClientSimpleData @data.
 **/
void    oseaclient_simple_free (OseaClientSimpleData * data)
{
	g_free (data->text_response);
	g_free (data);
	return;
}

/**
 * oseaclient_data_free:
 * @data: 
 * @destroy_list: 
 *
 * Frees OseaClientData @data. If @destroy_list is FALSE, it won't destroy
 * the OseaClientList contained in @data.
 **/
void    oseaclient_data_free   (OseaClientData * data, gboolean destroy_list)
{
	g_free (data->text_response);
	if (destroy_list)
		oseaclient_list_destroy (data->data);

	g_free (data);
	return;
}


void __destroy_list (gpointer data, gpointer user_data)
{
	oseaclient_list_destroy ( (OseaClientList *) data );	
	return;
}

/**
 * oseaclient_multi_free:
 * @data: 
 * @destroy_trees: 
 * 
 * Frees OseaClientMultiData @data and the GList contained in @data. If
 * @destroy_trees is FALSE, it won't destroy the OseaClientLists contained in
 * the GList, but it will do destroy the GList.
 **/
void    oseaclient_multi_free  (OseaClientMultiData * data, gboolean destroy_lists)
{
	g_free (data->text_response);
	if (destroy_lists)
		g_list_foreach (data->multi_data, __destroy_list, NULL);

	g_list_free (data->multi_data);
	g_free (data);
	return;
}

void oseaclient_print_version_info ()
{
	glong cd = COMPILATION_DATE;

	g_print ("\tLibOseaClient version %s, compiled %s", VERSION, ctime(&cd));
}

void oseaclient_init ()
{
	if (!g_thread_supported ()) g_thread_init (NULL);
}
