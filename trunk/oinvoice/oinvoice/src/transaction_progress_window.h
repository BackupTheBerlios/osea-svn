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


#ifndef _TRANSACTION_PROGRESS_WINDOW_H_
#define _TRANSACTION_PROGRESS_WINDOW_H_

#include <gnome.h>
#include <glade/glade.h>

// Funci�n que crea la ventana de login

void     transaction_progress_window_show         (GtkWindow *parent, const gchar *text, const gchar *event);

void     transaction_progress_window_destroy      ();

gboolean transaction_progress_window_delete_event (GtkWidget *widget, GdkEvent *event, gpointer data);

void     transaction_cancel_clicked               (GtkButton *button, gpointer user_data);

#endif
