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

#include "login_window.h"
#include "settings.h"
#include "dialog.h"
#include "connection_settings_window.h"
#include "connection_progress_window.h"
#include "event_source.h"
#include "main_window.h"
#include "widgets.h"
#include "refresh_session_dialog.h"
#include <liboseaclient/oseaclient.h>

GladeXML * login_window_xml = NULL;
gint       connection_try = 0;
gchar    * last_connection_event = NULL;
gchar    * last_ok_connection_event = NULL;


// Function that creates the login window

void login_window_show()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL;

	if (! login_window_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "login", NULL );
		
		login_window_xml = widgets_load (xml_file, "login_window", NULL);
		
	}

	widget = glade_xml_get_widget (login_window_xml, "login_window");

	gtk_widget_show (widget);

	return;
}


// Function that destroys the login window
void login_window_destroy ()
{
	GtkWidget * login_window = NULL;

	login_window = glade_xml_get_widget (login_window_xml, "login_window");

	g_object_unref (G_OBJECT (login_window_xml));
	login_window_xml = NULL;
	
	gtk_object_destroy (GTK_OBJECT (login_window));
	login_window = NULL;	

	return;
}


// Function that process close window event.
gboolean login_window_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	gtk_main_quit();

	return FALSE;
}

void login_window_connection_settings_button_clicked (GtkButton *connection_settings_button, gpointer user_data)
{
	GtkWindow * login_window = NULL;

	login_window = GTK_WINDOW (glade_xml_get_widget (login_window_xml, "login_window"));

	connection_settings_window_show (login_window);
	
	return;
}

void __login_window_logged_in (OseaClientNulData * data)
{
	GtkWindow         * login_window = NULL;

	connection_progress_window_destroy ();

	event_source_remove_signal (last_ok_connection_event);
	g_free (last_ok_connection_event);
	last_ok_connection_event = NULL;

 	login_window = GTK_WINDOW (glade_xml_get_widget (login_window_xml, "login_window"));


	if (!data) {
		dialog_close_run (login_window, GTK_MESSAGE_ERROR,
				  _("Error obtaining data from kernel server"));
		oseaclient_nul_free (data);

		return;
	} 
	
	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (login_window, GTK_MESSAGE_ERROR,
				  data->text_response);
		
		oseaclient_nul_free (data);			

	} else {		
		widgets_set_permission_disabling (TRUE);

		oseaclient_password_set_callback (refresh_session_dialog_show);

		main_window_show ();
		
		login_window_destroy ();
		
		oseaclient_nul_free (data);
	}
	
	
	return;
}


gboolean __login_window_aos_kernel_event (OseaClientNulData * session, gpointer signal_aux)
{
	gchar              * signal = (gchar *) signal_aux;
	
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, _("Entering login_aos_kernel_event function with signal '%s'\n"), signal);

	if (event_source_exist (signal)) {
		event_source_emit_signal (signal, session);
		last_ok_connection_event = signal;
		return TRUE;

	} else {
		g_free (signal);
		return FALSE;
	}
	
}

void login_window_ok_button_clicked (GtkButton *connection_settings_button, gpointer user_data)
{
	GtkEntry  * user_entry = NULL;
	GtkEntry  * passwd_entry = NULL;
	GtkWindow * login_window = NULL;
	gchar     * user = NULL;
	gchar     * pwd = NULL;
	gchar     * host = NULL;
	gchar     * port = NULL;
	GString   * connection_event = NULL;


	user_entry = GTK_ENTRY (glade_xml_get_widget (login_window_xml, "login_user_entry"));
	passwd_entry = GTK_ENTRY (glade_xml_get_widget (login_window_xml, "login_password_entry"));
	login_window = GTK_WINDOW (glade_xml_get_widget (login_window_xml, "login_window"));

	user = g_strdup (gtk_entry_get_text (user_entry));
	pwd = g_strdup (gtk_entry_get_text (passwd_entry));

	if ( (! strcmp (user, "")) ||
	     (! strcmp (pwd, ""))) {
		dialog_close_run (login_window, GTK_MESSAGE_ERROR,
				  _("You must enter a username and a password") );
		g_free (user);
		g_free (pwd);
		return;

	} 
		
	host = settings_get ("hostname", SETTINGS_STRING);
	port = settings_get ("portnumber", SETTINGS_INT);

	connection_progress_window_show (login_window);
	// Ask for login to our server
		
	connection_event = g_string_new (NULL);
	g_string_sprintf (connection_event, "logged_in %d", ++connection_try);
	last_connection_event = connection_event->str;
	g_string_free (connection_event, FALSE);
		
	event_source_add_signal (last_connection_event);
	event_source_set_callback (last_connection_event, (GSourceFunc) __login_window_logged_in);
	
	if (!oseaclient_session_login (user, pwd, host, port , __login_window_aos_kernel_event, 
					 last_connection_event)) {
		
		connection_progress_window_destroy ();
		dialog_close_run (login_window, GTK_MESSAGE_ERROR,
				  _("There was a problem while trying to log \ninto server %s with user %s"),
				  host, user);
		
		
	}

	g_free (user);
	g_free (pwd);
	g_free (host);
	g_free (port);
	return;
}


gchar * login_window_get_last_connection_event ()
{
	return last_connection_event;
}

