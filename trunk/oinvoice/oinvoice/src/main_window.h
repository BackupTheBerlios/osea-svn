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


#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <gnome.h>
#include <glade/glade.h>
#include <aoskernel/aos_kernel.h>
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include "osea_datalist.h"

typedef struct __StatusBarMessage {
	guint context_id;
	guint message_id;
} StatusBarMessage;


// Functions to create the login window

void               main_window_show                     ();

void               main_window_destroy                  ();

GtkWindow        * main_window_get                      ();

GtkWidget        * main_window_sidebar_create           (gchar *string1, gchar *string2, gint int1, gint int2);

void               main_window_menubar_create           ();

void               main_window_toolbar_create           ();

void               main_window_component_create         ();

void               main_window_menubar_destroy          ();

void               main_window_toolbar_destroy          ();

void               main_window_component_destroy        ();

gint               main_window_menubar_update           (gint old_component, gint new_component);

gint               main_window_toolbar_update           (gint old_component, gint new_component);

gint               main_window_component_update         (gint old_component, gint new_component);

GtkWindow        * main_window_get_active_window        ();
void               main_window_set_active_window        (GtkWindow *window);
void               main_window_unset_active_window      ();


// Menu callbacks

void               main_window_on_log_out_activate      (gpointer widget, gpointer user_data);

void               main_window_on_toolbar_activate      (GtkCheckMenuItem *checkmenuitem, gpointer user_data);

void               main_window_on_menubar_activate      (GtkCheckMenuItem *checkmenuitem, gpointer user_data);

void               main_window_on_quit_activate         (gpointer widget, gpointer user_data);

void               main_window_on_about_activate        (GtkMenuItem *menuitem, gpointer user_data);

// Sidebar callbacks

void               main_window_sidebar_select           (EShortcutBar   * shortcut_bar,
							 GdkEvent       * event,
							 gint		  group_num,
							 gint		  item_num);

// Window events callbacks

gboolean           main_window_on_delete_event          (GtkWidget *widget, GdkEvent *event, gpointer user_data);

// Functions for writing messages in statusbar

StatusBarMessage * main_window_set_statusbar_message    (const gchar *message, gint timeout);

void               main_window_remove_statusbar_message (StatusBarMessage *message);

void               main_window_set_progressbar          (gboolean active);

#endif
