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

#include <glade/glade.h>
#include "refresh_session_dialog.h"
#include "event_source.h"
#include "widgets.h"
#include "main_window.h"
#include "dialog.h"

GladeXML * refresh_session_dialog_xml = NULL;

OseaClientPasswordData * password_data = NULL;

gboolean __refresh_session_dialog_show (OseaClientPasswordData * pwd_data)
{
	gchar         * xml_file = NULL;
	GtkWindow     * window = NULL;

	password_data = pwd_data;

	if (! refresh_session_dialog_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "main", NULL );
		
		refresh_session_dialog_xml = widgets_load (xml_file, "session_expired_window", NULL);
		
	}

	window = GTK_WINDOW (glade_xml_get_widget (refresh_session_dialog_xml, 
						   "session_expired_window"));

	gtk_window_set_transient_for (window, main_window_get_active_window ());
	main_window_set_active_window (window);

	gtk_widget_show (GTK_WIDGET(window));

	return FALSE;
}

gboolean refresh_session_dialog_show (OseaClientPasswordData * pwd_data)
{
	gchar         * signal = NULL;

	signal = event_source_arm_signal ("refresh_session_dialog", (GSourceFunc) __refresh_session_dialog_show);
	event_source_emit_signal (signal, pwd_data);

	return FALSE;
}


void refresh_session_dialog_ok_button_clicked (GtkButton *connection_settings_button, gpointer user_data)
{
	GtkEntry  * passwd_entry = NULL;
	GtkWindow * window = NULL;
	gchar     * pwd = NULL;

	passwd_entry = GTK_ENTRY (glade_xml_get_widget (refresh_session_dialog_xml, "session_expired_password_entry"));
	window = GTK_WINDOW (glade_xml_get_widget (refresh_session_dialog_xml, "session_expired_window"));

	pwd = g_strdup (gtk_entry_get_text (passwd_entry));

	if (! strcmp (pwd, "")) {
		dialog_close_run (window, GTK_MESSAGE_ERROR,
				  _("You must enter a username and a password") );
		g_free (pwd);
		return;

	} 

	password_data->notify_cb (pwd, password_data->user_data);

	g_free (password_data);

	g_object_unref (G_OBJECT (refresh_session_dialog_xml));
	refresh_session_dialog_xml = NULL;
	
	gtk_object_destroy (GTK_OBJECT (window));
	window = NULL;	

	main_window_unset_active_window ();
	
	return;	
}

void refresh_session_dialog_on_log_out_activate (gpointer widget, gpointer user_data) 
{
	GtkWindow *window = GTK_WINDOW (glade_xml_get_widget (refresh_session_dialog_xml, "session_expired_window"));

	password_data->notify_cb (NULL, password_data->user_data);

	g_free (password_data);

	g_object_unref (G_OBJECT (refresh_session_dialog_xml));
	refresh_session_dialog_xml = NULL;
	
	gtk_object_destroy (GTK_OBJECT (window));
	window = NULL;	

	main_window_unset_active_window ();
	
	main_window_on_log_out_activate (widget, user_data);
}


gboolean refresh_session_dialog_on_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{

	return TRUE;
}
