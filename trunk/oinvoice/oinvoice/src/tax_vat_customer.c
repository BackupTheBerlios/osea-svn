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

#include <config.h>
#include <glib.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>
#include <oseaclienttax/aos_tax.h>
#include "tax.h"
#include "tax_vat_customer.h"
#include "tax_vat_value.h"
#include "main_window.h"
#include "login_window.h"
#include "event_source.h"
#include "event_process.h"
#include "dialog.h"
#include "transaction_progress_window.h"
#include "widgets.h"
#include "osea_listmodel.h"
#include "entry-manager.h"
#include "detailedview-manager.h"
#include <oseaclienttax/aos_tax.h>


gint            tax_vat_customer_transaction_number = 0;
gchar         * tax_vat_customer_last_transaction_event = NULL;

AsplListmodel * tax_vat_customer_listmodel = NULL;

// GladeXML     * tax_vat_customer_window_xml           = NULL;
EntryManager * tax_vat_customer_window_entry_manager = NULL;

DetailedViewManager * tax_vat_customer_detailed_view_manager = NULL;

typedef struct __TaxVatCustomerSignalMessage {
	gchar            * signal;
	StatusBarMessage * message;
} TaxVatCustomerSignalMessage;


AsplListmodel * tax_vat_customer_listmodel_get (void)
{
	return tax_vat_customer_listmodel;
}

gpointer tax_vat_customer_start_activity (gpointer       usr_data)
{
	StatusBarMessage   * status_message;

	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (usr_data, 0);

	return status_message;
}

void     tax_vat_customer_end_activity (gpointer status_data, gpointer usr_data)
{
	StatusBarMessage   * status_message = (StatusBarMessage *) status_data;

	main_window_remove_statusbar_message (status_message);
	

	main_window_set_progressbar (FALSE);

}

void tax_vat_customer_listmodel_create ()
{
	GtkTreeView         * view = NULL;
	GtkTreeViewColumn   * column = NULL;
	AsplListmodel       * model = NULL;
	GtkCellRenderer     * renderer = NULL;
	AsplListmodelConfig * cfg = osea_listmodel_config_new ("os-tax",
							       aos_tax_vat_customer_list,
							       tax_vat_customer_start_activity,
							       _("Asking os-tax server for VAT customer types..."),
							       tax_vat_customer_end_activity,
							       NULL,
							       aos_tax_vat_customer_remove,
							       _("Are you sure you want to delete this VAT Customer Type?\n"),
							       tax_vat_customer_start_activity,
							       _("Removing VAT Customer type..."),
							       tax_vat_customer_end_activity,
							       NULL); 

 	tax_vat_customer_listmodel = osea_listmodel_new (cfg, OSEACLIENT_TYPE_TAX_VATCUSTOMER); 
 	model = tax_vat_customer_listmodel; 

	view = GTK_TREE_VIEW (tax_vat_get_widget ("vat_customer_treeview"));
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(model));
	gtk_tree_view_set_headers_clickable (view, TRUE);
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Reference"),
							   renderer,
							   "text", osea_listmodel_get_column (model, "reference"),
							   NULL);
	gtk_tree_view_append_column (view, column);
	
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("VAT Customer Type Name"),
							   renderer,
							   "text", osea_listmodel_get_column (model, "name"),
							   NULL);
	gtk_tree_view_append_column (view, column);
	
	gtk_tree_view_set_search_column (view, osea_listmodel_get_column (model, "reference"));
	
	tax_vat_customer_detailed_view_manager = detailedview_manager_new (
		GTK_TREE_VIEW(tax_vat_get_widget ("vat_customer_treeview")));
	detailedview_manager_attach_label (tax_vat_customer_detailed_view_manager, 
				    GTK_LABEL (tax_vat_get_widget ("vat_customer_type_ref_label")),
				    "reference", FALSE);
	detailedview_manager_attach_label (tax_vat_customer_detailed_view_manager, 
				    GTK_LABEL (tax_vat_get_widget ("vat_customer_type_name_label")),
				    "name", FALSE);
	detailedview_manager_attach_label (tax_vat_customer_detailed_view_manager, 
				    GTK_LABEL (tax_vat_get_widget ("vat_customer_type_description_label")),
				    "description", FALSE);

}


void tax_vat_customer_enable_selection_actions ()
{
	GtkWidget * widget;

	widget = tax_vat_menubar_get_widget ("edit_vat_customer_type1");
	widgets_set_sensitive ("vat", widget, TRUE);

	widget = tax_vat_menubar_get_widget ("remove_vat_customer_type1");
	widgets_set_sensitive ("vat", widget, TRUE);

	widget = tax_vat_menubar_get_widget ("refresh1");
	widgets_set_sensitive ("vat", widget, TRUE);

	widget = tax_vat_toolbar_get_widget ("remove_vat_type_button");
	widgets_set_sensitive ("vat", widget, TRUE);

	widget = tax_vat_toolbar_get_widget ("new_vat_type_button");
	widgets_set_sensitive ("vat", widget, TRUE);

	widget = tax_vat_toolbar_get_widget ("refresh_button");
	widgets_set_sensitive ("vat", widget, TRUE);

	return;
}

void tax_vat_customer_disable_selection_actions ()
{
	GtkWidget * widget;

	widget = tax_vat_menubar_get_widget ("edit_vat_customer_type1");
	widgets_set_sensitive ("vat", widget, FALSE);

	widget = tax_vat_menubar_get_widget ("remove_vat_customer_type1");
	widgets_set_sensitive ("vat", widget, FALSE);

	widget = tax_vat_toolbar_get_widget ("new_vat_type_button");
	widgets_set_sensitive ("vat", widget, FALSE);

	widget = tax_vat_toolbar_get_widget ("remove_vat_type_button");
	widgets_set_sensitive ("vat", widget, FALSE);
	
	return;
}

void tax_vat_customer_update_selection_actions ()
{
	GtkTreeView      * treeview;
	GtkTreeIter        iter;
	GtkTreeSelection * selection;
	GtkWidget        * widget;

	treeview = GTK_TREE_VIEW (tax_vat_get_widget ("vat_customer_treeview"));
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		tax_vat_customer_enable_selection_actions ();
	} else {
		tax_vat_customer_disable_selection_actions ();
	}

	widget = tax_vat_toolbar_get_widget ("new_vat_type_button");
	widgets_set_sensitive ("vat", widget, TRUE);

	widget = tax_vat_toolbar_get_widget ("refresh_button");
	widgets_set_sensitive ("vat", widget, TRUE);

	widget = tax_vat_menubar_get_widget ("refresh1");
	widgets_set_sensitive ("vat", widget, TRUE);

	return;
}


void tax_vat_customer_cursor_changed (GtkTreeView *treeview, gpointer user_data)
{
	GtkTreeSelection    * selection;
	GtkTreeIter           iter;
	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter))
		tax_vat_customer_enable_selection_actions ();
		
	
	return;
}


gchar * tax_vat_customer_get_last_transaction_event ()
{
	return tax_vat_customer_last_transaction_event;
}


void tax_vat_customer_refresh (void)
{
	osea_listmodel_refresh (tax_vat_customer_listmodel);
}



void tax_vat_customer_free_selection (void)
{
	GtkTreeView * treeview;

	treeview = GTK_TREE_VIEW (tax_vat_get_widget ("vat_customer_treeview"));
	gtk_tree_selection_unselect_all (gtk_tree_view_get_selection (treeview));

	tax_vat_customer_disable_selection_actions ();
	tax_vat_customer_update_selection_actions ();

	return;
}


void tax_vat_customer_window_show (AsplListmodel * model, GtkTreeIter * iter)
{
	gchar         * xml_file = NULL;

	xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "vat", NULL );
	
	// Create entry manager
	if (tax_vat_customer_window_entry_manager)
		g_object_unref (G_OBJECT (tax_vat_customer_window_entry_manager));
	
	tax_vat_customer_window_entry_manager = entry_manager_new (xml_file, "customer_window", 
								   main_window_get ());

	// Attach all GtkEntries
	entry_manager_attach_entry  (tax_vat_customer_window_entry_manager,
				     "customer_window_name_entry", TRUE);

	entry_manager_attach_entry  (tax_vat_customer_window_entry_manager,
				     "customer_window_reference_entry", FALSE);
	entry_manager_attach_txtbuf (tax_vat_customer_window_entry_manager,
				     "customer_window_description_textview", FALSE);


	if (iter)
		// Load window with data
		entry_manager_load (tax_vat_customer_window_entry_manager, model, * iter);
	else
		// Start window with out data		
		entry_manager_start (tax_vat_customer_window_entry_manager, model);

	return;

}


gint tax_vat_customer_get_id_selected (void)
{
	GtkTreeSelection    * selection;
	GtkTreeIter           iter;
	gint                  id = -1;
	GtkTreeView         * treeview;
	AosTaxVatCustomer * node;

	treeview = GTK_TREE_VIEW (tax_vat_get_widget ("vat_customer_treeview"));	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		node = AOS_TAX_VATCUSTOMER(osea_listmodel_get_node (tax_vat_customer_listmodel, &iter));
		id = node->id;
	}

	return id;
}


void tax_vat_customer_new (void)
{
	tax_vat_customer_free_selection ();
	tax_vat_customer_window_show (tax_vat_customer_listmodel, NULL);

	return;
}

void tax_vat_customer_edit (void)
{
	GtkTreeSelection    * selection;
	GtkTreeView         * treeview;
	GtkTreeIter           iter;


	treeview = GTK_TREE_VIEW (tax_vat_get_widget ("vat_customer_treeview"));	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		
		tax_vat_customer_window_show (tax_vat_customer_listmodel, &iter);
	} 

	return;
}

void tax_vat_customer_remove (void)
{
	GtkTreeSelection            * selection;
	GtkTreeView                 * treeview;
	GtkTreeIter                   iter;

	treeview = GTK_TREE_VIEW (tax_vat_get_widget ("vat_customer_treeview"));
	
	selection = gtk_tree_view_get_selection (treeview);
	
	// Try to get an iter to the customer
	if (!gtk_tree_selection_get_selected (selection, NULL, &iter)) 
		return;

	osea_listmodel_remove (tax_vat_customer_listmodel, &iter);
	       
}




/**
 * tax_vat_customer_free_data:
 * 
 * Frees all internal module data.
 **/
void           tax_vat_customer_free_data                 (void)
{
	// Free gtk-list-store object
	if (tax_vat_customer_listmodel) {
		g_object_unref (tax_vat_customer_listmodel);
		tax_vat_customer_listmodel = NULL;
	}
	
	return;
}