//  ASPL Fact: invoicing client program for ASPL Fact System
//  Copyright (C) 2003  Advanced Software Production Line, S.L.
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


#include "dialog.h"

void dialog_close_run (GtkWindow *parent, GtkMessageType message_type, gchar *message_format, ...)
{
	GtkMessageDialog *dialog = NULL;
        gchar * msg = NULL;
	va_list args;


	g_return_if_fail (message_format);

	if (message_format) {
		va_start (args, message_format);
		msg = g_strdup_vprintf (message_format, args);
		va_end (args);			       		
	}
	
	dialog = (GtkMessageDialog *) gtk_message_dialog_new (parent,
							      GTK_DIALOG_MODAL,
							      message_type,
							      GTK_BUTTONS_CLOSE,
							      msg);
	if (msg)
		g_free ((gchar *) msg);

	gtk_dialog_run (GTK_DIALOG (dialog));

	gtk_widget_destroy (GTK_WIDGET (dialog));	

	return;
}

gint dialog_cancel_ok_run (GtkWindow *parent, GtkMessageType message_type, gchar *message_format, ...)
{
	GtkMessageDialog *dialog = NULL;
        gchar * msg = NULL;
	va_list args;
	gint response;
	
	g_return_val_if_fail (message_format, FALSE);

	if (message_format) {
		va_start (args, message_format);
		msg = g_strdup_vprintf (message_format, args);
		va_end (args);			       		
	}
	
	
	dialog = (GtkMessageDialog *) gtk_message_dialog_new (parent,
							      GTK_DIALOG_MODAL,
							      message_type,
							      GTK_BUTTONS_OK_CANCEL,
							      msg);
	if (msg)
		g_free ((gchar *) msg);

	response = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (GTK_WIDGET (dialog));	

	return response;
}

