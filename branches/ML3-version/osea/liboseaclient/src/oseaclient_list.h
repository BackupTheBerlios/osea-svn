//
//  LibOseaClient: common functions to liboseaclient* level and architectural functions.
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//  Copyright (C) 2004 David Marín Carreño
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

#ifndef __OSEACLIENT_LIST_H__
#define __OSEACLIENT_LIST_H__

#include <glib.h>

typedef struct __OseaClientList OseaClientList;

OseaClientList * oseaclient_list_new      (GCompareFunc compare_func);

OseaClientList * oseaclient_list_new_full (GCompareFunc compare_func, 
				 GDestroyNotify key_destroy_func,
				 GDestroyNotify value_destroy_func);

void        oseaclient_list_insert   (OseaClientList *list, gpointer key, gpointer value);

gint        oseaclient_list_index    (OseaClientList *list, gpointer key);

gpointer    oseaclient_list_lookup   (OseaClientList *list, gpointer key);

gpointer    oseaclient_list_nth_data (OseaClientList *list, gint n);

guint       oseaclient_list_length   (OseaClientList *list);

void        oseaclient_list_remove   (OseaClientList *list, gpointer key);

void        oseaclient_list_foreach  (OseaClientList *list, GTraverseFunc func, gpointer user_data);

void        oseaclient_list_replace  (OseaClientList *list, gpointer key, gpointer new_value);

void        oseaclient_list_destroy  (OseaClientList *list);

void        oseaclient_list_first    (OseaClientList *list);

gpointer    oseaclient_list_data     (OseaClientList *list);

gboolean    oseaclient_list_next     (OseaClientList *list);

gboolean    oseaclient_list_set_cursor_position (OseaClientList *list, gint cursor_position);

gint        oseaclient_list_get_cursor_position (OseaClientList *list);

#endif
