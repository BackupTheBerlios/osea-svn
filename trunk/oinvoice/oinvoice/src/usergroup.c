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

#include <config.h>
#include <glib.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>
#include <oseaclienttax/aos_tax.h>
#include "usergroup.h"
#include "usergroup_user.h"
#include "usergroup_group.h"
#include "usergroup_permission.h"
#include "main_window.h"
#include "dialog.h"
#include "login_window.h"
#include "widgets.h"

#define LOG_DOMAIN "usergroup"

static GladeXML     * usergroup_menubar_xml = NULL;
static GladeXML     * usergroup_toolbar_xml = NULL;
static GladeXML     * usergroup_component_xml = NULL;


GtkWidget * usergroup_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (usergroup_component_xml, NULL);

	return glade_xml_get_widget (usergroup_component_xml, widget_name);
}

GtkWidget * usergroup_menubar_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (usergroup_menubar_xml, NULL);

	return glade_xml_get_widget (usergroup_menubar_xml, widget_name);
}

GtkWidget *usergroup_toolbar_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (usergroup_toolbar_xml, NULL);

	return glade_xml_get_widget (usergroup_toolbar_xml, widget_name);
}


GtkWidget * usergroup_menu_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! usergroup_menubar_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "user", NULL );
		
		usergroup_menubar_xml = widgets_load (xml_file, "usergroup_menubar", NULL);
		
	}

	widget = glade_xml_get_widget (usergroup_menubar_xml, "usergroup_menubar");

	gtk_widget_show (widget);

	return widget;

}

void usergroup_menu_destroy (GtkWidget *widget)
{
	g_object_unref (usergroup_menubar_xml);
	usergroup_menubar_xml = NULL;

	return;
}
	

GtkWidget * usergroup_toolbar_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! usergroup_toolbar_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "user", NULL );
		
		usergroup_toolbar_xml = widgets_load (xml_file, "usergroup_toolbar", NULL);
		
	}

	widget = glade_xml_get_widget (usergroup_toolbar_xml, "usergroup_toolbar");

	gtk_widget_show (widget);

	return widget;

}
	
void usergroup_toolbar_destroy (GtkWidget *widget) 
{
	if (usergroup_toolbar_xml)
		g_object_unref (usergroup_toolbar_xml);
	usergroup_toolbar_xml = NULL;


	return;
}



GtkWidget * usergroup_component_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! usergroup_component_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "user", NULL );
		
		usergroup_component_xml = widgets_load (xml_file, "usergroup_component", NULL);
		
	}

	widget = glade_xml_get_widget (usergroup_component_xml, "usergroup_component");

	gtk_widget_show (widget);

	// Disable group menu options
	usergroup_group_disable_selection_actions ();

	// Disable group menu options
	usergroup_user_enable_selection_actions ();

	usergroup_refresh1_activate ();

	return widget;

}
	



void usergroup_component_destroy (GtkWidget *widget)
{
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Freeing usergroup component");

	// Finally, free the GladeXML object.
	if (usergroup_component_xml && G_IS_OBJECT(usergroup_component_xml))
		g_object_unref (usergroup_component_xml);
	usergroup_component_xml = NULL;
	

	// Free all local module data
	usergroup_user_free_data ();
	usergroup_group_free_data ();
	usergroup_permission_free_data ();

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Freed usergroup component");
	return;
}


gboolean usergroup_user_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer usergroup_data)
{
	static gint selected_id = 0;

	if (event->button == 1) {

		if (event->type == GDK_BUTTON_PRESS) {
			selected_id = usergroup_user_get_id_selected();
			return FALSE;
		}
		
		if (event->type == GDK_2BUTTON_PRESS) {
			if ((selected_id == usergroup_user_get_id_selected ()) && 
			    (oseaclient_session_get_permission ("os-kernel", "edit_user")))
				usergroup_user_edit ();
			return TRUE;
		}
	}
	return FALSE;
}


gboolean usergroup_group_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event,  gpointer usergroup_data)
{
	static gint selected_id = 0;

	if (event->button == 1) {

		if (event->type == GDK_BUTTON_PRESS) {
			selected_id = usergroup_group_get_id_selected();
			return FALSE;
		}
		
		if (event->type == GDK_2BUTTON_PRESS) {
			if ((selected_id == usergroup_group_get_id_selected ()) &&
			    (oseaclient_session_get_permission("os-kernel", "edit_group")))
				usergroup_group_edit ();
			return TRUE;
		}
	}

	return FALSE;
}



/* // Callbacks for File menu */

void usergroup_new_user1_activate ()
{
	GtkWidget * widget;

	g_return_if_fail (usergroup_component_xml);
	
	widget = glade_xml_get_widget (usergroup_component_xml, "usergroup_managing_notebook");
	gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 0);
	
	usergroup_user_new ();

	return;
}

void usergroup_new_group1_activate ()  
{  
 	GtkWidget * widget; 

	g_return_if_fail (usergroup_component_xml);

	widget = glade_xml_get_widget (usergroup_component_xml, "usergroup_managing_notebook");
	gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 1);
	
	usergroup_group_new ();

	return;
}




// Callbacks for Edit menu

void usergroup_preferences1_activate ()
{
	dialog_close_run (main_window_get (), GTK_MESSAGE_INFO,
			  _("There are no preferences yet. \n\nIf you want some preferences, you can write down them at usergroup_preferences1_activate: usergroup.c: line 249 \n\n(Our litle homenage to abiword project ;-)"));
	return;
}

/* // Callbacks for View menu */


void usergroup_users_list1_activate (GtkCheckMenuItem *checkmenuitem, gpointer usergroup_data)
{
	GtkWidget * widget;

	g_return_if_fail (usergroup_component_xml);

	widget = glade_xml_get_widget (usergroup_component_xml, "usergroup_managing_notebook");
	
	if (gtk_check_menu_item_get_active (checkmenuitem)) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 0);
	}

	return;
}

void usergroup_groups_list1_activate (GtkCheckMenuItem *checkmenuitem, gpointer usergroup_data)
{
	GtkWidget * widget;

	g_return_if_fail (usergroup_component_xml);

	widget = glade_xml_get_widget (usergroup_component_xml, "usergroup_managing_notebook");
	
	if (gtk_check_menu_item_get_active (checkmenuitem)) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 1);

	}
	return;
}

void usergroup_permissions_list1_activate (GtkCheckMenuItem *checkmenuitem, gpointer usergroup_data)
{
	GtkWidget * widget;

	g_return_if_fail (usergroup_component_xml);

	widget = glade_xml_get_widget (usergroup_component_xml, "usergroup_managing_notebook");
	
	if (gtk_check_menu_item_get_active (checkmenuitem)) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 2);

	}
	return;
}

void usergroup_refresh1_activate ()
{
	gint current_page;
	GtkNotebook *notebook = NULL;

	g_return_if_fail (usergroup_component_xml);

	notebook = GTK_NOTEBOOK (glade_xml_get_widget (usergroup_component_xml,
						       "usergroup_managing_notebook"));
	
	current_page = gtk_notebook_get_current_page (notebook);

	switch (current_page) {
	case 0: 
		if (oseaclient_session_get_permission("os-kernel", "list_user"))
			usergroup_user_refresh ();
		break;
	case 1: 
		if (oseaclient_session_get_permission("os-kernel", "list_group"))
			usergroup_group_refresh ();
		break;
	case 2: 
		usergroup_permission_refresh ();
		break;
	default:
		break;
	}

	return;
					      
}


/* // Callbacks for Actions menu */

void usergroup_edit_user1_activate ()
{
	usergroup_user_edit ();
}

void usergroup_remove_user1_activate ()
{
	usergroup_user_remove ();
}

void usergroup_edit_group1_activate () 
{ 
 	usergroup_group_edit (); 
} 

void usergroup_remove_group1_activate () 
{ 
 	usergroup_group_remove (); 
} 

/* // Callbacks for Toolbar */

void usergroup_new_clicked ()
{
	GtkNotebook * notebook     = NULL;
	gint          current_page;


	g_return_if_fail (usergroup_component_xml);

	notebook = GTK_NOTEBOOK (glade_xml_get_widget (usergroup_component_xml,
						       "usergroup_managing_notebook"));
	
	current_page = gtk_notebook_get_current_page (notebook);

	switch (current_page) {

	case 0:
		usergroup_user_new ();
		break;

	case 1:
		usergroup_group_new ();
		break;

	case 2:
		// There no way to create a permission from a client. Therefore
		// we do nothing.
		break;

	default:
		g_assert_not_reached ();
		break;
	}

	return;
}

void usergroup_delete_clicked ()
{
	GtkNotebook * notebook = NULL;
	gint          current_page;

	g_return_if_fail (usergroup_component_xml);

	notebook = GTK_NOTEBOOK (glade_xml_get_widget (usergroup_component_xml, "usergroup_managing_notebook"));
	current_page = gtk_notebook_get_current_page (notebook);

	switch (current_page) {

	case 0:
		usergroup_user_remove ();
		break;

	case 1:
		usergroup_group_remove ();
		break;

	case 2:
		// This is for permission case. But cannot delete permissions. Therefore we do nothing
		break;

	default:
		g_assert_not_reached ();
		break;
	}

	return;
}


UsergroupTab usergroup_get_active_notebook_tab ()
{
	gint current_page;

	GtkNotebook *notebook = NULL;

	g_return_val_if_fail (usergroup_component_xml, -1);

	notebook = GTK_NOTEBOOK (glade_xml_get_widget (usergroup_component_xml, "usergroup_managing_notebook"));
	current_page = gtk_notebook_get_current_page (notebook);

	return current_page;
}


void usergroup_managing_notebook_switch_page (GtkNotebook *notebook,
					   GtkNotebookPage *page,
					   guint page_num,
					   gpointer usergroup_data)
{
	GtkWidget *widget;

	g_return_if_fail (usergroup_component_xml);
	
	switch (page_num) {
	case 0:
		if ((! usergroup_user_list_store_get ()) &&
		    (oseaclient_session_get_permission("os-kernel", "list_user")))
			usergroup_user_refresh ();

		// Disable group menu options
		usergroup_group_disable_selection_actions ();

		// Enable user menu option
		usergroup_user_update_selection_actions ();
		
		gtk_check_menu_item_set_active  (
			GTK_CHECK_MENU_ITEM(glade_xml_get_widget (usergroup_menubar_xml, "users_list1")),
			TRUE);
		break;

	case 1:
		if ((! usergroup_group_list_store_get ()) &&
		    (oseaclient_session_get_permission("os-kernel", "list_group")))
			usergroup_group_refresh ();

		// Disable user menu options
		usergroup_user_disable_selection_actions ();

		// Enable group menu options
		usergroup_group_update_selection_actions ();

		gtk_check_menu_item_set_active  (
			GTK_CHECK_MENU_ITEM(glade_xml_get_widget (usergroup_menubar_xml, "groups_list1")), TRUE);
		break;


	case 2: 

		if (oseaclient_session_get_permission ("os-kernel", "permission_list")) {

			if ((!oseaclient_session_get_permission ("os-kernel", "list_user")) &&
			    (oseaclient_session_get_permission ("os-kernel", "list_group"))) {

				widget = glade_xml_get_widget (usergroup_component_xml, "optionmenu1");
				gtk_option_menu_set_history (GTK_OPTION_MENU (widget), 1);
				usergroup_permission_group_activate ();
			}

			if (! usergroup_permission_list_store_get ())
				usergroup_permission_refresh ();
			
			usergroup_permission_userlist_refresh (FALSE);
		}
		
                // Disable user and group menu options
		usergroup_user_disable_selection_actions ();
		usergroup_group_disable_selection_actions ();

		gtk_check_menu_item_set_active  (
			GTK_CHECK_MENU_ITEM(glade_xml_get_widget (usergroup_menubar_xml, "permissions_list1")),
			TRUE);
		break;
	
	default:
		g_assert_not_reached ();
		break;
	}
}





