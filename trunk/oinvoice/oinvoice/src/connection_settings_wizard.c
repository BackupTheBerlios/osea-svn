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

#include "connection_settings_wizard.h"
#include "connection_settings_window.h"
#include "settings.h"
#include "login_window.h"
#include "widgets.h"

ConnectionSettingsWizardTab active_tab = CONNECTION_SETTINGS_WIZARD_START_TAB;
GladeXML *connection_settings_wizard_xml = NULL;
GtkNotebook *connection_settings_wizard_notebook = NULL;

static gchar *connection_settings_wizard_hostname = NULL;
static gchar *connection_settings_wizard_portnumber = NULL;


// Functión that creates the connection configuration wizard
void connection_settings_wizard_show()
{
	gchar *xml_file = NULL;
	GtkWidget *widget = NULL;

	if (! connection_settings_wizard_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", 
					     "connection_settings", NULL );
		
		connection_settings_wizard_xml = widgets_load (xml_file, "connection_settings_wizard", NULL);
		
	}

	widget = glade_xml_get_widget (connection_settings_wizard_xml, "connection_settings_wizard");

	gtk_widget_show (widget);

	connection_settings_wizard_notebook = GTK_NOTEBOOK (glade_xml_get_widget (connection_settings_wizard_xml, 
										  "connection_settings_wizard_notebook"));

	connection_settings_wizard_activate_tab (CONNECTION_SETTINGS_WIZARD_START_TAB);

}


gboolean __connection_data_is_valid ()
{
	return connection_settings_window_is_valid (connection_settings_wizard_hostname, connection_settings_wizard_portnumber);
}

void connection_settings_wizard_prepare (GnomeDruid *druid, gchar *tab)
{
	if (!strcmp (tab, "start_tab"))
		gnome_druid_set_buttons_sensitive(druid, FALSE, TRUE, TRUE, FALSE);
	if (!strcmp (tab, "data_tab"))
		gnome_druid_set_buttons_sensitive(druid, TRUE, __connection_data_is_valid(), TRUE, FALSE);
	if (!strcmp (tab, "finish_tab"))
		gnome_druid_set_buttons_sensitive(druid, TRUE, TRUE, TRUE, FALSE);	
}


void connection_settings_wizard_activate_tab (ConnectionSettingsWizardTab next_tab)
{
	GtkButton *forward, *apply, *backward;
	active_tab = next_tab;

	gtk_notebook_set_current_page (connection_settings_wizard_notebook, active_tab);

	forward = GTK_BUTTON (glade_xml_get_widget (connection_settings_wizard_xml, "connection_settings_wizard_forward"));
	backward = GTK_BUTTON (glade_xml_get_widget (connection_settings_wizard_xml, "connection_settings_wizard_backward"));
	apply = GTK_BUTTON (glade_xml_get_widget (connection_settings_wizard_xml, "connection_settings_wizard_apply"));

	switch (next_tab) {
	case CONNECTION_SETTINGS_WIZARD_START_TAB:
		gtk_widget_set_sensitive (GTK_WIDGET (backward), FALSE);
		gtk_widget_set_sensitive (GTK_WIDGET (forward), TRUE);
		g_object_set (G_OBJECT (forward), "visible", TRUE, NULL);
		g_object_set (G_OBJECT (apply), "visible", FALSE, NULL);
		break;
	case CONNECTION_SETTINGS_WIZARD_DATA_TAB:
		gtk_widget_set_sensitive (GTK_WIDGET (backward), TRUE);
		gtk_widget_set_sensitive (GTK_WIDGET (forward), __connection_data_is_valid() );
		g_object_set (G_OBJECT (forward), "visible", TRUE, NULL);
		g_object_set (G_OBJECT (apply), "visible", FALSE, NULL);
		break;
	case CONNECTION_SETTINGS_WIZARD_FINISH_TAB:
		gtk_widget_set_sensitive (GTK_WIDGET (backward), TRUE);
		g_object_set (G_OBJECT (apply), "visible", TRUE, NULL);	
		g_object_set (G_OBJECT (forward), "visible", FALSE, NULL);
		break;
	default:
		// We should never enter here
		g_assert(NULL);
	}

}



// Function that process the wizard close window event

gboolean connection_settings_wizard_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	gtk_main_quit();
	return FALSE;

}

// Function that process the wizard cancel event

void connection_settings_wizard_apply_clicked (GtkWidget *widget, gpointer user_data)
{
	GtkWidget *connection_settings_wizard;
	
	settings_set ("hostname", connection_settings_wizard_hostname, SETTINGS_STRING);
	settings_set ("portnumber", connection_settings_wizard_portnumber, SETTINGS_INT);

	// We want to destroy the whole window

	connection_settings_wizard = glade_xml_get_widget (connection_settings_wizard_xml, "connection_settings_wizard");

	g_object_unref (G_OBJECT (connection_settings_wizard_xml));
	connection_settings_wizard_xml = NULL;
	
	gtk_object_destroy (GTK_OBJECT (connection_settings_wizard));
	connection_settings_wizard = NULL;

	g_free (connection_settings_wizard_hostname);
	connection_settings_wizard_hostname = NULL;
	g_free (connection_settings_wizard_portnumber);
	connection_settings_wizard_portnumber = NULL;

	// Now, we show the login window
	login_window_show();
}

void connection_settings_wizard_backward_clicked (GtkWidget *widget, gpointer user_data)
{
	connection_settings_wizard_activate_tab (active_tab - 1);
}

void connection_settings_wizard_cancel_clicked (GtkWidget *widget, gpointer user_data)
{
	gtk_main_quit();
}

void connection_settings_wizard_forward_clicked (GtkWidget *widget, gpointer user_data)
{
	connection_settings_wizard_activate_tab (active_tab + 1);
}


void connection_settings_wizard_hostname_entry_activate (GtkEntry *hostname_entry, gpointer user_data)
{
	GtkButton *forward = NULL;

	forward = GTK_BUTTON (glade_xml_get_widget (connection_settings_wizard_xml, "connection_settings_wizard_forward"));

	if (connection_settings_wizard_hostname) {
		g_free (connection_settings_wizard_hostname);	
		connection_settings_wizard_hostname = NULL;
	}


	if (strcmp (gtk_entry_get_text (hostname_entry), "") ) {
		connection_settings_wizard_hostname = g_strdup (gtk_entry_get_text (hostname_entry));
	}
	
	gtk_widget_set_sensitive (GTK_WIDGET (forward), __connection_data_is_valid() );
	
}

void connection_settings_wizard_port_entry_activate (GtkEntry *port_entry, gpointer user_data)
{
	
	GtkButton *forward = NULL;

	forward = GTK_BUTTON (glade_xml_get_widget (connection_settings_wizard_xml, "connection_settings_wizard_forward"));

	if (connection_settings_wizard_portnumber) {
		g_free (connection_settings_wizard_portnumber);
		connection_settings_wizard_portnumber = NULL;
	}

	if (strcmp (gtk_entry_get_text (port_entry), "") ) {

		connection_settings_wizard_portnumber = g_strdup (gtk_entry_get_text (port_entry));
	}

	gtk_widget_set_sensitive (GTK_WIDGET (forward), __connection_data_is_valid() );

}
