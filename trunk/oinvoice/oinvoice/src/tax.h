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

#ifndef __TAX_H_
#define __TAX_H_

#include <gtk/gtk.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>
#include "tax_vat_customer.h"

GtkWidget    * tax_vat_get_widget                           (const gchar *widget_name);

GtkWidget    * tax_vat_toolbar_get_widget                   (const gchar *widget_name);

GtkWidget    * tax_vat_menubar_get_widget                   (const gchar *widget_name);

GtkWidget    * tax_vat_menu_create                          ();

GtkWidget    * tax_vat_toolbar_create                       ();

GtkWidget    * tax_vat_component_create                     ();

void           tax_vat_menu_destroy                         (GtkWidget *widget);

void           tax_vat_toolbar_destroy                      (GtkWidget *widget);

void           tax_vat_component_destroy                    (GtkWidget *widget);

gboolean       tax_vat_customer_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data);

gboolean       tax_vat_item_treeview_button_press_event     (GtkWidget *widget, GdkEventButton *event,  gpointer user_data);

// Menu callbacks

void           tax_new_vat_customer_type1_activate          ();

void           tax_new_vat_item_type1_activate              ();

void           tax_preferences1_activate                    ();

void           tax_vat_client_types1_activate               (GtkCheckMenuItem *checkmenuitem, gpointer user_data);

void           tax_vat_item_types1_activate                 (GtkCheckMenuItem *checkmenuitem, gpointer user_data);

void           tax_vat_values1_activate                     (GtkCheckMenuItem *checkmenuitem, gpointer user_data);

void           tax_refresh1_activate                        ();

void           tax_edit_vat_customer_type1_activate         ();

void           tax_remove_vat_customer_type1_activate       ();

void           tax_edit_vat_item_type1_activate             ();

void           tax_remove_vat_item_type1_activate           ();

#endif
