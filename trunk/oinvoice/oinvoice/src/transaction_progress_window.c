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

#include "main_window.h"
#include "transaction_progress_window.h"
#include "event_source.h"
#include "widgets.h"

static GladeXML * transaction_progress_window_xml = NULL;
static int        transaction_progress_timer      = 0;
/* static gchar    * transaction_event               = NULL;*/

gint transaction_progress_timeout( gpointer data )
{
	GtkProgressBar * progressbar = NULL;
	
	if (transaction_progress_window_xml == NULL)
		return FALSE;

	progressbar = GTK_PROGRESS_BAR (glade_xml_get_widget (transaction_progress_window_xml, "transaction_progressbar"));
	
	gtk_progress_bar_pulse (progressbar);
	
	return TRUE;
} 



void transaction_progress_window_show (GtkWindow *parent, const gchar *text, const gchar *event) 
{
	gchar *xml_file = NULL;
	GtkWindow *window = NULL;
	GtkProgressBar *progressbar = NULL;
	GtkLabel *label = NULL;

	if (! transaction_progress_window_xml) {

		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "main", NULL );
		
		transaction_progress_window_xml = widgets_load (xml_file, "transaction_progress_window", NULL);
		
	}

	window = GTK_WINDOW (glade_xml_get_widget (transaction_progress_window_xml, "transaction_progress_window"));
	progressbar = GTK_PROGRESS_BAR (glade_xml_get_widget (transaction_progress_window_xml, "transaction_progressbar"));
	label = GTK_LABEL (glade_xml_get_widget (transaction_progress_window_xml, "transaction_progress_text"));

	gtk_window_set_transient_for (window, parent);
	main_window_set_active_window (window);

	gtk_label_set_text (label, text);

	gtk_progress_bar_set_pulse_step (progressbar, 0.1);
	gtk_progress_bar_pulse (progressbar);

/*	transaction_event = g_strdup (event); */

	transaction_progress_timer = g_timeout_add (100, transaction_progress_timeout, NULL);

	gtk_widget_show (GTK_WIDGET(window));
	
	return;
}

void transaction_progress_window_destroy ()
{
	GtkWindow *window = NULL;

	g_source_remove (transaction_progress_timer);
	transaction_progress_timer = 0;
	
	window = GTK_WINDOW (glade_xml_get_widget (transaction_progress_window_xml, "transaction_progress_window"));
	
	gtk_widget_hide (GTK_WIDGET (window));

	g_object_unref (G_OBJECT (transaction_progress_window_xml));
	transaction_progress_window_xml = NULL;

	gtk_object_destroy (GTK_OBJECT (window));
	window = NULL;	

/*	g_free (transaction_event);
	transaction_event = NULL; */
	
	main_window_unset_active_window ();

	return;
}

gboolean transaction_progress_window_delete_event (GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return TRUE;
}

void transaction_cancel_clicked (GtkButton *button, gpointer user_data)
{

/*	if (event_source_exist (transaction_event))
	event_source_remove_signal (transaction_event); */

	transaction_progress_window_destroy ();

	return;
}
