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


#include "about_window.h"
#include "main_window.h"
#include "settings.h"
#include "widgets.h"
#include <glade/glade.h>
#include <oseaclient/afdal.h>

GladeXML * about_window_xml = NULL;
gchar    * hostname  = NULL;
gchar    * portnumber = NULL;

/**
 * __about_window_print_active_server:
 * @key: 
 * @value: 
 * @data: 
 * 
 * Function which creates about window dialog.
 * 
 * Return value: 
 **/
void __about_window_print_active_server (gpointer value, gpointer data)
{
	OseaClientSessionServer * server = (AfDalSessionServer *) value;
	GString            * string = (GString *) data;

	g_string_sprintfa (string, "  %s (%s:%s)\n", server->name, server->host, server->port);

	oseaclient_session_server_free (server);
	
	return;
}


/**
 * about_window_show:
 * @parent: 
 * 
 * Function which creates about window widget
 **/
void about_window_show (GtkWindow *parent)
{
	gchar              * xml_file = NULL;
	GtkWidget          * window = NULL;
	GtkWidget          * session_info = NULL;
	GtkTextBuffer      * session_text_buffer = gtk_text_buffer_new (NULL);
	GString            * string = NULL;
	GList              * active_servers = oseaclient_session_active_servers ();

	// Obtain GladeXML object
	if (! about_window_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "main", NULL );
		
		about_window_xml = widgets_load (xml_file, "about_window", NULL);
		
	}

	// Get about_window widget
	window = glade_xml_get_widget (about_window_xml, "about_window");

	gtk_window_set_transient_for (GTK_WINDOW(window), parent);
	main_window_set_active_window (GTK_WINDOW(window));
	
	session_info = glade_xml_get_widget (about_window_xml, "session_info_text_view");
	

	// Set up all available servers
	string = g_string_new (NULL);

	g_string_sprintfa (string, _("%d available servers:\n"), g_list_length (active_servers) );
	
	g_list_foreach (active_servers, __about_window_print_active_server, string);
	
	g_list_free (active_servers);

	gtk_text_buffer_set_text (session_text_buffer, string->str, string->len);

	gtk_text_view_set_buffer (GTK_TEXT_VIEW(session_info), session_text_buffer);

	g_string_free (string, TRUE);


	// Launch about window
	gtk_widget_show (window);       	
	
	return;
}


/**
 * about_window_delete_event:
 * @widget: 
 * @event: 
 * @user_data: 
 * 
 * Function which process the close link configuration window event.
 * 
 * Return value: 
 **/

gboolean about_window_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{

	g_object_unref (G_OBJECT (about_window_xml));

	about_window_xml = NULL;

	main_window_unset_active_window ();

	return FALSE;

}


void about_ok_clicked (GtkButton *widget,  gpointer user_data)
{
	GtkWidget * window;

	window = glade_xml_get_widget (about_window_xml, "about_window");	

	g_object_unref (G_OBJECT (about_window_xml));

	about_window_xml = NULL;

	gtk_object_destroy (GTK_OBJECT (window));	

	main_window_unset_active_window ();

	return;

}

