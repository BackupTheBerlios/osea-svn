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

#include "customer_window.h"
#include "main_window.h"
#include "widgets.h"

GladeXML     * customer_window_xml = NULL;

void customer_window_show (gpointer customer)
{
	gchar         * xml_file = NULL;
//	gchar         * title = NULL;
	GtkWidget     * widget = NULL;
//	GtkWidget     * name_entry = NULL;
//	GtkTextBuffer * txtbuf = NULL;

	g_return_if_fail (! customer_window_xml);

	xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "customer", NULL );
		
	customer_window_xml = widgets_load (xml_file, "customer_window", NULL);
		
/* 	if (name) { */
/* 		widget = glade_xml_get_widget (tax_vat_customer_window_xml, "customer_window_name_entry"); */
/* 		gtk_entry_set_text (GTK_ENTRY(widget), name); */
/* 	} */

/* 	if (ref) { */
/* 		widget = glade_xml_get_widget (tax_vat_customer_window_xml, "customer_window_reference_entry"); */
/* 		gtk_entry_set_text (GTK_ENTRY(widget), ref); */
/* 	} */

/* 	if (description) { */
/* 		widget = glade_xml_get_widget (tax_vat_customer_window_xml, "customer_window_description_textview"); */
/* 		txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget)); */
/* 		gtk_text_buffer_set_text (txtbuf, description, -1); */
/* 	} */

	widget = glade_xml_get_widget (customer_window_xml, "customer_window");

	gtk_window_set_transient_for (GTK_WINDOW(widget), main_window_get ());
	main_window_set_active_window (GTK_WINDOW(widget));

/* 	if (name) { */
/* //		old_title = (gchar *) gtk_window_get_title (GTK_WINDOW(widget)); */
/* 		title = g_strdup_printf (_("%s - ASPL Fact"), name); */
/* 		gtk_window_set_title (GTK_WINDOW(widget), (const gchar *) title); */
/* //		g_free (old_title); */
/* 	} */

	// set the focus
/*   	name_entry =  glade_xml_get_widget (tax_vat_customer_window_xml, "customer_window_name_entry");  */
/* 	gtk_widget_grab_focus (name_entry); */

	gtk_widget_show (widget);

	return;

}
