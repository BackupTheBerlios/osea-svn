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

#ifndef __USERGROUP_H_
#define __USERGROUP_H_

#include <gtk/gtk.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>

GtkWidget    * usergroup_get_widget                           (const gchar *widget_name);

GtkWidget    * usergroup_toolbar_get_widget                   (const gchar *widget_name);

GtkWidget    * usergroup_menubar_get_widget                   (const gchar *widget_name);

GtkWidget    * usergroup_menu_create                          ();

GtkWidget    * usergroup_toolbar_create                       ();

GtkWidget    * usergroup_component_create                     ();

void           usergroup_menu_destroy                         (GtkWidget *widget);

void           usergroup_toolbar_destroy                      (GtkWidget *widget);

void           usergroup_component_destroy                    (GtkWidget *widget);

gboolean       usergroup_user_treeview_button_press_event     (GtkWidget *widget, GdkEventButton *event, gpointer usergroup_data); 

gboolean       usergroup_group_treeview_button_press_event    (GtkWidget *widget, GdkEventButton *event,  gpointer usergroup_data); 

// Menu callbacks

void           usergroup_new_user1_activate          ();

/* void           usergroup_new_group1_activate              (); */

/* void           usergroup_preferences1_activate                    (); */

void           usergroup_users_list1_activate               (GtkCheckMenuItem *checkmenuitem, gpointer usergroup_data);

/* void           usergroup_groups_list1_activate                 (GtkCheckMenuItem *checkmenuitem, gpointer usergroup_data); */

void           usergroup_refresh1_activate                        ();

void           usergroup_edit_user1_activate         ();

void           usergroup_remove_user1_activate       ();

/* void           usergroup_edit_group1_activate             (); */

/* void           usergroup_remove_group1_activate           (); */

void           usergroup_new_clicked ();
void           usergroup_delete_clicked ();

typedef enum {USERGROUP_USER_TAB, USERGROUP_GROUP_TAB, USERGROUP_PERMISSION_TAB} UsergroupTab;

UsergroupTab   usergroup_get_active_notebook_tab ();

void           usergroup_managing_notebook_switch_page (GtkNotebook *notebook, GtkNotebookPage *page,
							guint page_num, gpointer usergroup_data);

#endif
