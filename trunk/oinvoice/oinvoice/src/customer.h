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

#ifndef __CUSTOMER_H_
#define __CUSTOMER_H_

#include <gtk/gtk.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>

GtkWidget    * customer_get_widget                           (const gchar *widget_name);

GtkWidget    * customer_toolbar_get_widget                   (const gchar *widget_name);

GtkWidget    * customer_menubar_get_widget                   (const gchar *widget_name);

GtkWidget    * customer_menu_create                          ();

GtkWidget    * customer_toolbar_create                       ();

GtkWidget    * customer_component_create                     ();

void           customer_menu_destroy                         (GtkWidget *widget);

void           customer_toolbar_destroy                      (GtkWidget *widget);

void           customer_component_destroy                    (GtkWidget *widget);

gboolean       customer_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);


// Menu callbacks

void           customer_new1_activate          ();

void           customer_preferences1_activate                    ();

void           customer_client_types1_activate               (GtkCheckMenuItem *checkmenuitem, gpointer user_data);

void           customer_values1_activate                     (GtkCheckMenuItem *checkmenuitem, gpointer user_data);

void           customer_refresh1_activate                        ();

void           customer_edit1_activate         ();

void           customer_remove1_activate       ();

#endif
