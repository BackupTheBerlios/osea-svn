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
#include <oseaclientcustomer/aos_customer.h>
#include "customer_actions.h"
#include "main_window.h"
#include "login_window.h"
#include "event_source.h"
#include "event_process.h"
#include "dialog.h"
#include "transaction_progress_window.h"
#include "widgets.h"
#include "osea_listmodel.h"
#include "entry-manager.h"



gint            customer_actions_transaction_number = 0;
gchar         * customer_actions_last_transaction_event = NULL;

AsplListmodel * customer_actions_listmodel = NULL;

EntryManager * customer_actions_window_entry_manager = NULL;


gpointer customer_actions_start_activity (gpointer       usr_data)
{
	StatusBarMessage   * status_message;

	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (usr_data, 0);

	return status_message;
}

void     customer_actions_end_activity (gpointer status_data, gpointer usr_data)
{
	StatusBarMessage   * status_message = (StatusBarMessage *) status_data;

	main_window_remove_statusbar_message (status_message);
	

	main_window_set_progressbar (FALSE);

}

void __customer_actions_combine_name_and_surname (GtkTreeViewColumn * col, 
						  GtkCellRenderer   * renderer,
						  GtkTreeModel      * model,
						  GtkTreeIter       * iter,
						  gpointer            user_data)
{
	gchar * name = NULL;
	gchar * surname = NULL;
	gchar * temp_string = NULL;
	gchar   final_string[100];
	AosCustomerCustomer * customer = NULL;

	customer = AOS_CUSTOMER_CUSTOMER(osea_listmodel_get_node (model, iter));
	
	if (customer->name && customer->name[0])
		temp_string = g_strdup_printf ("%s, %s", customer->surname, customer->name);
	else
		temp_string = g_strdup (customer->surname);

	strncpy (final_string, temp_string, 100);
	if (strlen (temp_string) > 100) {
		final_string[97]='.';
		final_string[98]='.';
		final_string[99]='.';
	}
	
	g_free (temp_string);

	g_object_set (renderer, "text", final_string, NULL);
}

void customer_actions_listmodel_create ()
{
	GtkTreeView         * view = NULL;
	GtkTreeViewColumn   * column = NULL;
	AsplListmodel       * model = NULL;
	GtkCellRenderer     * renderer = NULL;
	AsplListmodelConfig * cfg = osea_listmodel_config_new ("os-customer",
							       aos_customer_customer_list,
							       customer_actions_start_activity,
							       _("Asking os-customer server for customers..."),
							       customer_actions_end_activity,
							       NULL,
							       aos_customer_customer_remove,
							       _("Are you sure you want to delete this customer?"),
							       customer_actions_start_activity,
							       _("Removing customer..."),
							       customer_actions_end_activity,
							       NULL); 

 	customer_actions_listmodel = osea_listmodel_new (cfg, OSEACLIENT_TYPE_CUSTOMER_CUSTOMER); 
 	model = customer_actions_listmodel; 
	view = GTK_TREE_VIEW (customer_get_widget ("customer_treeview"));
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(model));
	gtk_tree_view_set_headers_clickable (view, TRUE);
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Legal ID"),
							   renderer,
							   "text", osea_listmodel_get_column (model, "legal-id"),
							   NULL);
	gtk_tree_view_append_column (view, column);
	
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Name"),
							   renderer,
							   NULL);	
	gtk_tree_view_append_column (view, column);

	gtk_tree_view_column_set_cell_data_func (column, renderer, __customer_actions_combine_name_and_surname, NULL, NULL);

	
	gtk_tree_view_set_search_column (view, osea_listmodel_get_column (model, "legal-id"));
	
	

}


void customer_actions_update_selection_actions ()
{

}

void customer_actions_free_data ()
{

}

gint customer_actions_get_id_selected ()
{
	return 0;
}

void customer_actions_cursor_changed ()
{

}

void customer_actions_new ()
{
}

void customer_actions_edit ()
{
}

void customer_actions_remove ()
{

}

void customer_actions_refresh ()
{
	osea_listmodel_refresh (customer_actions_listmodel);
}

