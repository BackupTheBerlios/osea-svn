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


#ifndef __CONNECTION_SETTINGS_WIZARD_H__
#define __CONNECTION_SETTINGS_WIZARD_H__

#include <gnome.h>
#include <glade/glade.h>

typedef enum {CONNECTION_SETTINGS_WIZARD_START_TAB, 
	      CONNECTION_SETTINGS_WIZARD_DATA_TAB, 
	      CONNECTION_SETTINGS_WIZARD_FINISH_TAB} ConnectionSettingsWizardTab;


void     connection_settings_wizard_show                    ();

void     connection_settings_wizard_prepare                 (GnomeDruid *druid, gchar *tab);

void     connection_settings_wizard_activate_tab            (ConnectionSettingsWizardTab);


// Connection configuration wizard's callbacks 

gboolean connection_settings_wizard_delete_event            (GtkWidget *widget, GdkEvent *event, gpointer user_data);

void     connection_settings_wizard_apply_clicked           (GtkWidget *widget, gpointer user_data);

void     connection_settings_wizard_backward_clicked        (GtkWidget *widget, gpointer user_data);

void     connection_settings_wizard_cancel_clicked          (GtkWidget *widget, gpointer user_data);

void     connection_settings_wizard_forward_clicked         (GtkWidget *widget, gpointer user_data);

void     connection_settings_wizard_hostname_entry_activate (GtkEntry *widget, gpointer user_data);


#endif
