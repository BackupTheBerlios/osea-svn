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

#ifndef __OSEACLIENT_EVENT_SOURCE_H__
#define __OSEACLIENT_EVENT_SOURCE_H__

#include <glib.h>

void     oseaclient_event_source_add_signal    (gchar *signal_name);

guint    oseaclient_event_source_set_callback  (gchar *signal_name, GSourceFunc callback);

void     oseaclient_event_source_emit_signal   (gchar *signal_name, gpointer data);

void     oseaclient_event_source_remove_signal (gchar *signal_name);

gboolean oseaclient_event_source_exist         (gchar *signal_name);

gchar  * oseaclient_event_source_arm_signal    (gchar * signal_basename, GSourceFunc func);

void     oseaclient_event_source_launch        (GSourceFunc func, gpointer data);

void     oseaclient_event_source_print_pool    ();

#endif
