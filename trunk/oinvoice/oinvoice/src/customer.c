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
#include "customer.h"
#include "customer_actions.h"
#include "main_window.h"
#include "dialog.h"
#include "login_window.h"
#include "widgets.h"

#define LOG_DOMAIN "customer"

static GladeXML     * customer_menubar_xml = NULL;
static GladeXML     * customer_toolbar_xml = NULL;
static GladeXML     * customer_component_xml = NULL;


GtkWidget * customer_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (customer_component_xml, NULL);

	return glade_xml_get_widget (customer_component_xml, widget_name);
}

GtkWidget * customer_menubar_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (customer_menubar_xml, NULL);

	return glade_xml_get_widget (customer_menubar_xml, widget_name);
}

GtkWidget *customer_toolbar_get_widget (const gchar *widget_name)
{
	g_return_val_if_fail (widget_name, NULL);
	g_return_val_if_fail (customer_toolbar_xml, NULL);

	return glade_xml_get_widget (customer_toolbar_xml, widget_name);
}

GtkWidget * customer_menu_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! customer_menubar_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "customer", NULL );
		
		customer_menubar_xml = widgets_load (xml_file, "customer_menubar", NULL);
		
	}

	widget = glade_xml_get_widget (customer_menubar_xml, "customer_menubar");

	gtk_widget_show (widget);

	return widget;

}

void customer_menu_destroy (GtkWidget *widget)
{
	g_object_unref (customer_menubar_xml);
	customer_menubar_xml = NULL;

	return;
}
	

GtkWidget * customer_toolbar_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! customer_toolbar_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "customer", NULL );
		
		customer_toolbar_xml = widgets_load (xml_file, "customer_toolbar", NULL);
		
	}

	widget = glade_xml_get_widget (customer_toolbar_xml, "customer_toolbar");

	gtk_widget_show (widget);

	return widget;

}
	
void customer_toolbar_destroy (GtkWidget *widget) 
{
	if (customer_toolbar_xml)
		g_object_unref (customer_toolbar_xml);
	customer_toolbar_xml = NULL;


	return;
}



GtkWidget * customer_component_create ()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! customer_component_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "customer", NULL );
		
		customer_component_xml = widgets_load (xml_file, "customer_component", NULL);
		
	}

	widget = glade_xml_get_widget (customer_component_xml, "customer_component");

	gtk_widget_show (widget);

	customer_actions_update_selection_actions ();

	customer_actions_listmodel_create ();

	return widget;

}
	



void customer_component_destroy (GtkWidget *widget)
{
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Freeing customer component");

	// Finally, free the GladeXML object.
	if (customer_component_xml && G_IS_OBJECT(customer_component_xml))
		g_object_unref (customer_component_xml);
	customer_component_xml = NULL;

	// Free all local module data
	customer_actions_free_data ();
	
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Freed customer component");
	return;
}


gboolean customer_treeview_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	static gint selected_id = 0;

	if (event->button == 1) {

		if (event->type == GDK_BUTTON_PRESS) {
			selected_id = customer_actions_get_id_selected();
			return FALSE;
		}
		
		if (event->type == GDK_2BUTTON_PRESS) {
			if ((selected_id == customer_actions_get_id_selected ())  && 
			    (oseaclient_session_get_permission ("os-customer", "edit_customer")))
				customer_actions_edit ();
			return TRUE;
		}
	}
	return FALSE;
}





// Callbacks for File menu

void customer_new1_activate ()
{ 
	GtkWidget * widget;

	g_return_if_fail (customer_component_xml);
	
	widget = glade_xml_get_widget (customer_component_xml, "customer_managing_notebook");
	gtk_notebook_set_current_page (GTK_NOTEBOOK(widget), 0);
	
	customer_actions_new ();

	return;
}


// Callbacks for Edit menu

void customer_preferences1_activate () 
{ 
	dialog_close_run (main_window_get (), GTK_MESSAGE_INFO, 
			  _("There are no preferences yet. \n\nIf you want some preferences, you can write down them at customer_preferences1_activate: customer.c: line 259 \n\n(Our litle homenage to abiword project ;-)"));
	return;
}

// Callbacks for View menu


void customer_refresh1_activate () 
{ 
	if (oseaclient_session_get_permission("os-customer", "list_customer_item"))
		customer_actions_refresh ();

	return;
					      
}


// Callbacks for Actions menu

void customer_edit1_activate ()
{
	customer_actions_edit ();
}

void customer_remove1_activate ()
{
	customer_actions_remove ();
}


// Callbacks for Toolbar

void customer_new_customer_type_clicked ()
{
	customer_actions_new ();

	return;
}

void customer_delete_customer_type_clicked ()
{
	customer_actions_remove ();

	return;
}





