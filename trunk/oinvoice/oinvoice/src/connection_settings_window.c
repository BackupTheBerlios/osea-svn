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

#include "connection_settings_window.h"
#include "settings.h"
#include "widgets.h"

GladeXML *connection_settings_window_xml = NULL;
gchar *connection_settings_window_hostname  = NULL;
gchar *connection_settings_window_portnumber = NULL;


//Function that creates the connection configuration window
void connection_settings_window_show (GtkWindow *parent)
{
	gchar *xml_file = NULL;
	GtkWidget *window = NULL, *hostname_entry = NULL, *port_entry = NULL;

	connection_settings_window_hostname = settings_get ("hostname", SETTINGS_STRING);
	connection_settings_window_portnumber = settings_get ("portnumber", SETTINGS_INT);

	if (! connection_settings_window_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "connection_settings", NULL );
		
		connection_settings_window_xml = widgets_load (xml_file, "connection_settings_window", NULL);
		
	}

	window = glade_xml_get_widget (connection_settings_window_xml, "connection_settings_window");

	gtk_window_set_transient_for (GTK_WINDOW(window), parent);
	
	hostname_entry = glade_xml_get_widget (connection_settings_window_xml, "hostname_entry");
	port_entry = glade_xml_get_widget (connection_settings_window_xml, "port_entry");

	gtk_entry_set_text (GTK_ENTRY (hostname_entry), connection_settings_window_hostname);
	gtk_entry_set_text (GTK_ENTRY (port_entry), connection_settings_window_portnumber);

	gtk_widget_show (window);       	
}


gboolean connection_settings_window_is_valid (gchar * host, gchar * port)
{
	gint int_port;
	gchar * end_pointer;


	if (!host || !port)
		return FALSE;

	
	// We must check if port is a valid portnumber
	if (strlen (port) == 0) 
		return FALSE;

	int_port = (gint) strtol (port, &end_pointer, 10);

	if (strlen (end_pointer))
		return FALSE;

	if ((int_port > 65535) || (int_port < 1))
		return FALSE;

	return TRUE;
}


// Function that process the close window event
gboolean connection_settings_window_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{

	g_object_unref (G_OBJECT (connection_settings_window_xml));
	connection_settings_window_xml = NULL;

	return FALSE;

}

void connection_settings_window_cancel_clicked (GtkButton *widget,  gpointer user_data)
{
	GtkWidget *window;

	window = glade_xml_get_widget (connection_settings_window_xml, "connection_settings_window");	

	g_object_unref (G_OBJECT (connection_settings_window_xml));
	connection_settings_window_xml = NULL;

	gtk_object_destroy (GTK_OBJECT (window));	

	g_free (connection_settings_window_hostname);
	connection_settings_window_hostname = NULL;

	g_free (connection_settings_window_portnumber);
	connection_settings_window_portnumber = NULL;

	return;

}

void connection_settings_window_ok_clicked (GtkButton *widget,  gpointer user_data)
{
	GtkWidget *window;

	settings_set ("hostname", connection_settings_window_hostname, SETTINGS_STRING);
	settings_set ("portnumber", connection_settings_window_portnumber, SETTINGS_INT);

	window = glade_xml_get_widget (connection_settings_window_xml, "connection_settings_window");	

	g_object_unref (G_OBJECT (connection_settings_window_xml));
	connection_settings_window_xml = NULL;

	gtk_object_destroy (GTK_OBJECT (window));	

	g_free (connection_settings_window_hostname);
	connection_settings_window_hostname = NULL;

	g_free (connection_settings_window_portnumber);
	connection_settings_window_portnumber = NULL;

	return;

}

void connection_settings_window_hostname_entry_changed (GtkEntry *hostname_entry, gpointer user_data)
{
	GtkButton *ok_button = NULL;

	ok_button = GTK_BUTTON (glade_xml_get_widget (connection_settings_window_xml, "connection_settings_ok"));

	if (connection_settings_window_hostname) {
		g_free (connection_settings_window_hostname);	
		connection_settings_window_hostname = NULL;
	}

	if (strcmp (gtk_entry_get_text (hostname_entry), "") ) {
		connection_settings_window_hostname = g_strdup (gtk_entry_get_text (hostname_entry));
	}
	
	gtk_widget_set_sensitive (GTK_WIDGET (ok_button), connection_settings_window_is_valid(connection_settings_window_hostname, 
											      connection_settings_window_portnumber) );
	
}

void connection_settings_window_port_entry_changed (GtkEntry *port_entry, gpointer user_data)
{
	
	GtkButton *ok_button = NULL;

	ok_button = GTK_BUTTON (glade_xml_get_widget (connection_settings_window_xml, "connection_settings_ok"));

	if (connection_settings_window_portnumber) {
		g_free (connection_settings_window_portnumber);
		connection_settings_window_portnumber = NULL;
	}

	if (strcmp (gtk_entry_get_text (port_entry), "") ) {

		connection_settings_window_portnumber = g_strdup (gtk_entry_get_text (port_entry));
	}

	gtk_widget_set_sensitive (GTK_WIDGET (ok_button), connection_settings_window_is_valid(connection_settings_window_hostname, 
											      connection_settings_window_portnumber) );

}
