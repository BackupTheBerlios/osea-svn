//  ASPL Fact: invoicing client program for ASPL Fact System
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


#ifndef __CONNECTION_SETTINGS_WINDOW_H__
#define __CONNECTION_SETTINGS_WINDOW_H__

#include <gnome.h>
#include <glade/glade.h>


void     connection_settings_window_show                   (GtkWindow *parent);

gboolean connection_settings_window_is_valid               (gchar *hostname, gchar *portnumber);

gboolean connection_settings_window_delete_event           (GtkWidget *widget, GdkEvent *event, gpointer user_data);

void     connection_settings_window_cancel_clicked         (GtkButton *widget,  gpointer user_data);

void     connection_settings_window_ok_clicked             (GtkButton *widget,  gpointer user_data);

void     connection_settings_window_hostname_entry_changed (GtkEntry *hostname_entry, gpointer user_data);

void     connection_settings_window_port_entry_changed     (GtkEntry *port_entry, gpointer user_data);


#endif
