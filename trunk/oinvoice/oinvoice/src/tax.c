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

#include <config.h>
#include <glib.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>
#include <oseaclienttax/aos_tax.h>
#include "tax.h"
#include "tax_vat_customer.h"
#include "tax_vat_item.h"
#include "tax_vat_value.h"
#include "main_window.h"
#include "dialog.h"
#include "login_window.h"
#include "widgets.h"

#define LOG_DOMAIN "tax"

static GladeXML     * tax_vat_menubar_xml = NULL;
static GladeXML     * tax_vat_toolbar_xml = NULL;
static GladeXML     * tax_vat_component_xml = NULL;


GtkWidget * tax_vat_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (tax_vat_component_xml, NULL);

	return glade_xml_get_widget (tax_vat_component_xml, widget_name);
}

GtkWidget * tax_vat_menubar_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (tax_vat_menubar_xml, NULL);

	return glade_xml_get_widget (tax_vat_menubar_xml, widget_name);
}

GtkWidget *tax_vat_toolbar_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (tax_vat_toolbar_xml, NULL);

	return glade_xml_get_widget (tax_vat_toolbar_xml, widget_name);
}

GtkWidget * tax_vat_menu_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! tax_vat_menubar_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "vat", NULL );
		
		tax_vat_menubar_xml = widgets_load (xml_file, "vat_menubar", NULL);
		
	}

	widget = glade_xml_get_widget (tax_vat_menubar_xml, "vat_menubar");

	gtk_widget_show (widget);

	return widget;

}

void tax_vat_menu_destroy (GtkWidget *widget)
{
	g_object_unref (tax_vat_menubar_xml);
	tax_vat_menubar_xml = NULL;

	return;
}
	

GtkWidget * tax_vat_toolbar_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! tax_vat_toolbar_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "vat", NULL );
		
		tax_vat_toolbar_xml = widgets_load (xml_file, "vat_toolbar", NULL);
		
	}

	widget = glade_xml_get_widget (tax_vat_toolbar_xml, "vat_toolbar");

	gtk_widget_show (widget);

	return widget;

}
	
void tax_vat_toolbar_destroy (GtkWidget *widget) 
{
	if (tax_vat_toolbar_xml)
		g_object_unref (tax_vat_toolbar_xml);
	tax_vat_toolbar_xml = NULL;


	return;
}



GtkWidget * tax_vat_component_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! tax_vat_component_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "vat", NULL );
		
		tax_vat_component_xml = widgets_load (xml_file, "vat_component", NULL);
		
	}

	widget = glade_xml_get_widget (tax_vat_component_xml, "vat_component");

	gtk_widget_show (widget);

	tax_vat_customer_update_selection_actions ();

	tax_vat_item_update_selection_actions ();

	tax_vat_customer_listmodel_create ();
	tax_vat_item_listmodel_create ();
	
	return widget;

}
	



void tax_vat_component_destroy (GtkWidget *widget)
{
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Freeing vat component");

	// Finally, free the GladeXML object.
	if (tax_vat_component_xml && G_IS_OBJECT(tax_vat_component_xml))
		g_object_unref (tax_vat_component_xml);
	tax_vat_component_xml = NULL;

	// Free all local module data
	tax_vat_customer_free_data ();
	tax_vat_item_free_data ();
	tax_vat_value_free_data ();
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Freed vat component");
	return;
}


gboolean tax_vat_customer_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	static gint selected_id = 0;

	if (event->button == 1) {

		if (event->type == GDK_BUTTON_PRESS) {
			selected_id = tax_vat_customer_get_id_selected();
			return FALSE;
		}
		
		if (event->type == GDK_2BUTTON_PRESS) {
			if ((selected_id == tax_vat_customer_get_id_selected ())  && 
			    (oseaclient_session_get_permission ("os-tax", "edit_vat_customer")))
				tax_vat_customer_edit ();
			return TRUE;
		}
	}
	return FALSE;
}


gboolean tax_vat_item_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event,  gpointer user_data)
{
	static gint selected_id = 0;

	if (event->button == 1) {

		if (event->type == GDK_BUTTON_PRESS) {
			selected_id = tax_vat_item_get_id_selected();
			return FALSE;
		}
		
		if (event->type == GDK_2BUTTON_PRESS) {
			if ((selected_id == tax_vat_item_get_id_selected ()) && 
			    (oseaclient_session_get_permission ("os-tax", "edit_vat_item")))
				tax_vat_item_edit ();
			return TRUE;
		}
	}

	return FALSE;
}



// Callbacks for File menu

void tax_new_vat_customer_type1_activate ()
{ 
	GtkWidget * widget;

	g_return_if_fail (tax_vat_component_xml);
	
	widget = glade_xml_get_widget (tax_vat_component_xml, "vat_managing_notebook");
	gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 0);
	
	tax_vat_customer_new ();

	return;
}

void tax_new_vat_item_type1_activate () 
{ 
	GtkWidget * widget;

	g_return_if_fail (tax_vat_component_xml);

	widget = glade_xml_get_widget (tax_vat_component_xml, "vat_managing_notebook");
	gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 1);
	
	tax_vat_item_new ();

	return;
}




// Callbacks for Edit menu

void tax_preferences1_activate () 
{ 
	dialog_close_run (main_window_get (), GTK_MESSAGE_INFO, 
			  _("There are no preferences yet. \n\nIf you want some preferences, you can write down them at tax_preferences1_activate: tax.c: line 259 \n\n(Our litle homenage to abiword project ;-)"));
	return;
}

// Callbacks for View menu


void tax_vat_client_types1_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{ 
	GtkWidget * widget;

	g_return_if_fail (tax_vat_component_xml);

	widget = glade_xml_get_widget (tax_vat_component_xml, "vat_managing_notebook");
	
	if (gtk_check_menu_item_get_active (checkmenuitem)) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 0);
	}

	return;
}

void tax_vat_item_types1_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{ 
	GtkWidget * widget;

	g_return_if_fail (tax_vat_component_xml);

	widget = glade_xml_get_widget (tax_vat_component_xml, "vat_managing_notebook");
	
	if (gtk_check_menu_item_get_active (checkmenuitem)) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 1);

	}
	return;
}

void tax_vat_values1_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{ 
	GtkWidget *widget;

	g_return_if_fail (tax_vat_component_xml);

	widget = glade_xml_get_widget (tax_vat_component_xml, "vat_managing_notebook");
	
	if (gtk_check_menu_item_get_active (checkmenuitem)) {
		gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 2);
		
	}

	return;
}


void tax_refresh1_activate () 
{ 
	gint current_page;
	GtkNotebook *notebook = NULL;

	g_return_if_fail (tax_vat_component_xml);

	notebook = GTK_NOTEBOOK (glade_xml_get_widget (tax_vat_component_xml,
						       "vat_managing_notebook"));
	
	current_page = gtk_notebook_get_current_page (notebook);

	switch (current_page) {
	case 0: 
		if (oseaclient_session_get_permission("os-tax", "list_vat_customer"))
			tax_vat_customer_refresh ();
		break;
	case 1: 
		if (oseaclient_session_get_permission("os-tax", "list_vat_item"))
			tax_vat_item_refresh ();
		break;
	case 2: 
		if (oseaclient_session_get_permission("os-tax", "get_vat_value"))
			tax_vat_value_refresh ();
		break;
	default:
		break;
	}

	return;
					      
}


// Callbacks for Actions menu

void tax_edit_vat_customer_type1_activate ()
{
	tax_vat_customer_edit ();
}

void tax_remove_vat_customer_type1_activate ()
{
	tax_vat_customer_remove ();
}

void tax_edit_vat_item_type1_activate ()
{
	tax_vat_item_edit ();
}

void tax_remove_vat_item_type1_activate ()
{
	tax_vat_item_remove ();
}

// Callbacks for Toolbar

void tax_new_vat_type_clicked ()
{
	gint          current_page;
	GtkNotebook * notebook = NULL;

	g_return_if_fail (tax_vat_component_xml);

	notebook = GTK_NOTEBOOK (glade_xml_get_widget (tax_vat_component_xml,
						       "vat_managing_notebook"));
	
	current_page = gtk_notebook_get_current_page (notebook);

	switch (current_page) {

	case 0: 
		tax_vat_customer_new ();
		break;

	case 1:
		tax_vat_item_new ();
		break;

	case 2:
		break;

	default:
		g_assert_not_reached ();
		break;
	}

	return;
}

void tax_delete_vat_type_clicked ()
{
	gint current_page;

	GtkNotebook *notebook = NULL;

	g_return_if_fail (tax_vat_component_xml);

	notebook = GTK_NOTEBOOK (glade_xml_get_widget (tax_vat_component_xml, "vat_managing_notebook"));
	current_page = gtk_notebook_get_current_page (notebook);

	switch (current_page) {

	case 0: 
		tax_vat_customer_remove ();
		break;

	case 1:
		tax_vat_item_remove ();
		break;

	case 2:
		break;

	default:
		g_assert_not_reached ();
		break;
	}

	return;
}




void tax_vat_managing_notebook_switch_page (GtkNotebook *notebook,
					   GtkNotebookPage *page,
					   guint page_num,
					   gpointer user_data)
{
	g_return_if_fail (tax_vat_component_xml);
	
	switch (page_num) {
	case 0:
		if ((! tax_vat_customer_listmodel_get ()) &&
		    (oseaclient_session_get_permission("os-tax", "list_vat_customer")))
			tax_vat_customer_refresh ();

		tax_vat_item_disable_selection_actions ();
		tax_vat_customer_update_selection_actions ();
		gtk_check_menu_item_set_active  (
			GTK_CHECK_MENU_ITEM(glade_xml_get_widget (tax_vat_menubar_xml, "vat_client_types1")),
			TRUE);
		break;

	case 1:
		if ((! tax_vat_item_listmodel_get ()) &&
		    (oseaclient_session_get_permission("os-tax", "list_vat_customer")))
			tax_vat_item_refresh ();

		tax_vat_customer_disable_selection_actions ();
		tax_vat_item_update_selection_actions ();
		gtk_check_menu_item_set_active  (
			GTK_CHECK_MENU_ITEM(glade_xml_get_widget (tax_vat_menubar_xml, "vat_item_types1")),
			TRUE);
		break;

	case 2:
		if ((! tax_vat_value_tree_store_get ()) &&
		    (oseaclient_session_get_permission("os-tax", "get_vat_value")))
			tax_vat_value_refresh ();

		tax_vat_customer_disable_selection_actions ();
		tax_vat_item_disable_selection_actions ();
		gtk_check_menu_item_set_active  (
			GTK_CHECK_MENU_ITEM(glade_xml_get_widget (tax_vat_menubar_xml, "vat_values1")),
			TRUE);
		break;

	default:
		g_assert_not_reached ();
		break;
	}
}





