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

#include "connection_progress_window.h"
#include "event_source.h"
#include "login_window.h"
#include "widgets.h"
#include <glade/glade.h>

GladeXML * connection_progress_window_xml = NULL;
int        connection_progress_timer = 0;

/**
 * connection_progress_timeout:
 * @data: 
 * 
 * Function which creates the login window
 * 
 * Return value: 
 **/
gint connection_progress_timeout (gpointer data)
{

	GtkProgressBar * progressbar = NULL;
	
	progressbar = GTK_PROGRESS_BAR (glade_xml_get_widget (connection_progress_window_xml, "connection_progressbar"));
  
	gtk_progress_bar_pulse (progressbar);
  
	return TRUE;
} 



void connection_progress_window_show (GtkWindow *parent)
{
	gchar          * xml_file = NULL;
	GtkWindow      * window = NULL;
	GtkProgressBar * progressbar = NULL;

	if (! connection_progress_window_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "login", NULL );
		
		connection_progress_window_xml = widgets_load (xml_file, "connection_progress_window", NULL);
		
	}

	window = GTK_WINDOW (glade_xml_get_widget (connection_progress_window_xml, "connection_progress_window"));
	progressbar = GTK_PROGRESS_BAR (glade_xml_get_widget (connection_progress_window_xml, "connection_progressbar"));

	gtk_window_set_transient_for (window, parent);
	// main_window_set_active_window (window);

	gtk_progress_bar_set_pulse_step (progressbar, 0.1);
	gtk_progress_bar_pulse (progressbar);

	connection_progress_timer = g_timeout_add (100, connection_progress_timeout, NULL);

	gtk_widget_show (GTK_WIDGET(window));

	return;
}

void connection_progress_window_destroy ()
{
	GtkWindow *window = NULL;

	g_source_remove (connection_progress_timer);
	connection_progress_timer = 0;
	
	window = GTK_WINDOW (glade_xml_get_widget (connection_progress_window_xml, "connection_progress_window"));

	gtk_widget_hide (GTK_WIDGET (window));

	g_object_unref (G_OBJECT (connection_progress_window_xml));
	connection_progress_window_xml = NULL;

	gtk_object_destroy (GTK_OBJECT (window));
	window = NULL;	

	// main_window_unset_active_window ();
}

gboolean connection_progress_window_delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return TRUE;
}

void connection_cancel_clicked (GtkButton *button, gpointer user_data)
{
	gchar *last_connection_event = NULL;
	
	last_connection_event = login_window_get_last_connection_event ();

	if (event_source_exist (last_connection_event))
		event_source_remove_signal (last_connection_event);


	connection_progress_window_destroy ();
	
	return;
}
