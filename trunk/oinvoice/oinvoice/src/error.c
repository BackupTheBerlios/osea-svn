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

#include "error.h"
#include "widgets.h"
#include <glade/glade.h>

GladeXML * error_menubar_xml = NULL;
GladeXML * error_toolbar_xml = NULL;
GladeXML * error_component_xml = NULL;

GtkWidget * error_menu_create ()
{
	GtkWidget * widget = NULL;
	gchar     * xml_file = NULL;


	if (! error_menubar_xml) {
		
		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "main", NULL );
		
		error_menubar_xml = widgets_load (xml_file, "main_window_error_menubar", NULL);
		
		
	}
	
	widget = glade_xml_get_widget (error_menubar_xml, "main_window_error_menubar");

	gtk_widget_show (widget);

	return widget;	
	
}

void error_menu_destroy (GtkWidget *widget)
{
	if (error_menubar_xml) {
		g_object_unref (error_menubar_xml);
		error_menubar_xml = NULL;
	}
	return;
}


GtkWidget * error_toolbar_create ()
{
	GtkWidget * widget = NULL;
	gchar     * xml_file = NULL;
	
	if (! error_toolbar_xml) {
		
		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "main", NULL );
		
		error_toolbar_xml = widgets_load (xml_file, "main_window_error_toolbar", NULL);
		
		
	}
	
	widget = glade_xml_get_widget (error_toolbar_xml, "main_window_error_toolbar");
	
	gtk_widget_show (widget);
	
	return widget;
}

void error_toolbar_destroy (GtkWidget *widget)
{
	if (error_toolbar_xml) {
		g_object_unref (error_toolbar_xml);
		error_toolbar_xml = NULL;
	}
	return;

}


GtkWidget * error_component_create ()
{
	GtkWidget *widget = NULL;
	gchar *xml_file = NULL;

	do {

		if (! error_component_xml) {		
			xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "main", NULL );
			
			error_component_xml = widgets_load (xml_file, "main_window_error_component", NULL);
			
			g_return_val_if_fail (error_component_xml, NULL);

			
		}
		
		widget = glade_xml_get_widget (error_component_xml, "main_window_error_component");

	} while (!widget);
		
	gtk_widget_show (widget);
		
	return widget;
}

void error_component_destroy (GtkWidget *widget)
{
	if (error_component_xml) {
		g_object_unref (error_component_xml);
		error_component_xml = NULL;
	}
	return;

}


void error_refresh_activate () 
{ 

}

void error_preferences_activate () 
{ 

}
