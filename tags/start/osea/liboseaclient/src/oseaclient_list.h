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

#ifndef __AFDAL_LIST_H__
#define __AFDAL_LIST_H__

#include <glib.h>

typedef struct __AfDalList AfDalList;

AfDalList * oseaclient_list_new      (GCompareFunc compare_func);

AfDalList * oseaclient_list_new_full (GCompareFunc compare_func, 
				 GDestroyNotify key_destroy_func,
				 GDestroyNotify value_destroy_func);

void        oseaclient_list_insert   (AfDalList *list, gpointer key, gpointer value);

gint        oseaclient_list_index    (AfDalList *list, gpointer key);

gpointer    oseaclient_list_lookup   (AfDalList *list, gpointer key);

gpointer    oseaclient_list_nth_data (AfDalList *list, gint n);

guint       oseaclient_list_length   (AfDalList *list);

void        oseaclient_list_remove   (AfDalList *list, gpointer key);

void        oseaclient_list_foreach  (AfDalList *list, GTraverseFunc func, gpointer user_data);

void        oseaclient_list_replace  (AfDalList *list, gpointer key, gpointer new_value);

void        oseaclient_list_destroy  (AfDalList *list);

void        oseaclient_list_first    (AfDalList *list);

gpointer    oseaclient_list_data     (AfDalList *list);

gboolean    oseaclient_list_next     (AfDalList *list);

gboolean    oseaclient_list_set_cursor_position (AfDalList *list, gint cursor_position);

gint        oseaclient_list_get_cursor_position (AfDalList *list);

#endif
