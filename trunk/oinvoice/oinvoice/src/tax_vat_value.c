//  ASPL Fact: invoicing client program for ASPL Fact System
//  Copyright (C) 2003 Advanced Software Production Line, S.L.
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

#include <config.h>
#include <math.h>
#include <glib.h>
#include <liboseacomm/oseacomm.h>
#include <aoskernel/aos_kernel.h>
#include <aostax/aos_tax.h>
#include <errno.h>
#include "tax.h"
#include "tax_vat_value.h"
#include "main_window.h"
#include "login_window.h"
#include "event_source.h"
#include "event_process.h"
#include "dialog.h"
#include "transaction_progress_window.h"
#include <ctype.h>

gint           tax_vat_value_transaction_number = 0;

GtkTreeStore * tax_vat_value_tree_store = NULL;
OseaClientList        * tax_vat_value_tree = NULL;
OseaClientList        * tax_vat_value_customers_tree = NULL;
OseaClientList        * tax_vat_value_items_tree = NULL;
GladeXML     * tax_vat_value_window_xml = NULL;


enum { TAX_VAT_VALUE_ID_COLUMN,
       TAX_VAT_VALUE_REF_ITEM_COLUMN,
       TAX_VAT_VALUE_REF_CUST_COLUMN,
       TAX_VAT_VALUE_VALUE_COLUMN,
       TAX_VAT_VALUE_EDITABLE_COLUMN,
       TAX_VAT_VALUE_COLUMN_NUMBER 
};

enum {TAX_VAT_VALUE_ORDER_FIRST_ITEM,
      TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER
} tax_vat_value_selected_order = TAX_VAT_VALUE_ORDER_FIRST_ITEM;
      
GtkTreeStore *tax_vat_value_tree_store_get (void)
{
	return tax_vat_value_tree_store;
}


/* gchar *tax_vat_value_get_last_transaction_event () */
/* { */
/* 	return tax_vat_value_last_transaction_event; */
/* } */

void tax_vat_value_first_item_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkTreeView * view = NULL;
	GList       * columns = NULL;

	if (tax_vat_value_selected_order != TAX_VAT_VALUE_ORDER_FIRST_ITEM) {
		tax_vat_value_selected_order = TAX_VAT_VALUE_ORDER_FIRST_ITEM;

		view = GTK_TREE_VIEW (tax_vat_get_widget ("vat_value_treeview"));
		columns = gtk_tree_view_get_columns (view);
		gtk_tree_view_move_column_after (view, 
						 GTK_TREE_VIEW_COLUMN (g_list_nth_data (columns, 1)),
						 NULL);
		g_list_free (columns);

		tax_vat_value_regenerate_model ();
		
	}
	return;
}

void tax_vat_value_first_customer_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkTreeView * view = NULL;
	GList       * columns = NULL;

	if (tax_vat_value_selected_order != TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER) {
		tax_vat_value_selected_order = TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER;		
		
		view = GTK_TREE_VIEW (tax_vat_get_widget ("vat_value_treeview"));
		columns = gtk_tree_view_get_columns (view);
		gtk_tree_view_move_column_after (view, 
						 GTK_TREE_VIEW_COLUMN (g_list_nth_data (columns, 1)),
						 NULL);
		g_list_free (columns);

		tax_vat_value_regenerate_model ();
	}
	
	return;
}



AosTaxVatValueKey * tax_vat_value_get_key (int id_cust, int id_item)
{

	AosTaxVatValueKey * result = NULL;

	result = g_new0 (AosTaxVatValueKey, 1);
	result->id_customer = id_cust;
	result->id_item = id_item;

	return result;
}


static void tax_vat_value_cell_edited  (GtkCellRendererText *cell,
					const gchar         *path_string,
					const gchar         *new_text,
					gpointer             data)
{
	GtkTreeModel * model = (GtkTreeModel *) tax_vat_value_tree_store;
	GtkTreePath  * path = gtk_tree_path_new_from_string (path_string);
	gchar        * endptr, * old_text;
	GtkTreeIter  * iter;
	GtkTreeIter    parent_iter;
	gint           id_cust, id_item;
	double         new_fval;
	glong          new_val;
	gfloat         new_float;

	iter = g_new0 (GtkTreeIter, 1);

 	gtk_tree_model_get_iter (model, iter, path);
 	gtk_tree_model_iter_parent (model, &parent_iter, iter);

	switch (tax_vat_value_selected_order) {
	case TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER:
		gtk_tree_model_get (model, iter, TAX_VAT_VALUE_ID_COLUMN, &id_item, TAX_VAT_VALUE_VALUE_COLUMN, &old_text, -1);	 
		gtk_tree_model_get (model, &parent_iter, TAX_VAT_VALUE_ID_COLUMN, &id_cust, -1);		
		break;
	case TAX_VAT_VALUE_ORDER_FIRST_ITEM:
		gtk_tree_model_get (model, iter, TAX_VAT_VALUE_ID_COLUMN, &id_cust, TAX_VAT_VALUE_VALUE_COLUMN, &old_text, -1);	 
		gtk_tree_model_get (model, &parent_iter, TAX_VAT_VALUE_ID_COLUMN, &id_item, -1);		
		break;
	}
	
	new_fval = 100 * strtod (new_text, &endptr);
	
	if (endptr[0] !='\0' )
		return;
	
	if (new_fval == 0 && new_text == endptr)
		return;
	if (errno == ERANGE)
		return;

	new_val = lround (new_fval);
	new_float = new_val / 100.00;
	new_text = g_strdup_printf ("%.2f", new_float);
	
	if (! strcmp (new_text, old_text))
		return;


	gtk_tree_store_set (GTK_TREE_STORE(model), iter, 
			    TAX_VAT_VALUE_EDITABLE_COLUMN, FALSE,
			    -1);

	tax_vat_value_set (id_cust, id_item, new_val, iter);

}


gboolean __print_value_tree (gpointer k, gpointer n, gpointer usr_data)
{
	AosTaxVatValueKey  * key = (AosTaxVatValueKey *) k;
	AosTaxVatValueData * value  = (AosTaxVatValueData *) n;

	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "I:%d, C:%d, V:%ld", key->id_item, key->id_customer, value->value);
	
	return FALSE;

}

gfloat __tax_vat_value_get_value_at (OseaClientList *values, gint id, gint sub_id)
{
	AosTaxVatValueKey  * key = NULL;
	AosTaxVatValueData * datanode = NULL;

	key = g_new0 (AosTaxVatValueKey, 1);
	
	switch (tax_vat_value_selected_order) {
	case TAX_VAT_VALUE_ORDER_FIRST_ITEM:
		key->id_item = id;
		key->id_customer = sub_id;
		break;
	case TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER:
		key->id_item = sub_id;
		key->id_customer = id;
		break;
	}

	datanode = oseaclient_list_lookup (values, key);
	if (!datanode) {
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't find datanode with id_item=%d and id_cust=%d", 
		       key->id_item, key->id_customer);
		oseaclient_list_foreach (values, __print_value_tree, NULL);
		return 0.0;
	}

	return (datanode->value)/100;
}

gboolean __tax_vat_value_add_to_model_aux (gpointer key, gpointer node, gpointer usr_data)
{

	AosTaxVatValueHeader * value;
	gchar                  * string = NULL;
	gpointer               * data = (gpointer *) usr_data;
	GtkTreeStore           * store = GTK_TREE_STORE (data[0]);
	GtkTreeIter            * iter_parent = (GtkTreeIter *) (data[1]);
	OseaClientList                  * values = (OseaClientList *) (data[2]);
	gint                     id_parent = GPOINTER_TO_INT (data[3]);
	GtkTreeIter              iter;
	
	value = (AosTaxVatValueHeader *) node;

	g_return_val_if_fail (value, TRUE);

	string = g_strdup_printf ("%.2f", __tax_vat_value_get_value_at (values, id_parent, GPOINTER_TO_INT (key)));

	gtk_tree_store_append (store, &iter, iter_parent);
	switch (tax_vat_value_selected_order) {
	case TAX_VAT_VALUE_ORDER_FIRST_ITEM:
		gtk_tree_store_set (store, &iter,
				    TAX_VAT_VALUE_ID_COLUMN, value->id,
				    TAX_VAT_VALUE_REF_CUST_COLUMN, value->reference,
				    TAX_VAT_VALUE_VALUE_COLUMN, string,
				    TAX_VAT_VALUE_EDITABLE_COLUMN, TRUE,
				    -1);
		break;
	case TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER:
		gtk_tree_store_set (store, &iter,
				    TAX_VAT_VALUE_ID_COLUMN, value->id,
				    TAX_VAT_VALUE_REF_ITEM_COLUMN, value->reference,
				    TAX_VAT_VALUE_VALUE_COLUMN, string,
				    TAX_VAT_VALUE_EDITABLE_COLUMN, TRUE,
				    -1);
		break;
	}

	g_free (string);

	return FALSE;
}


gboolean __tax_vat_value_add_to_model (gpointer key, gpointer node, gpointer usr_data)
{
	AosTaxVatValueHeader * value;
	gpointer               * data = (gpointer *) usr_data;
	GtkTreeStore           * store = GTK_TREE_STORE (data[0]);
	OseaClientList                  * subrows = (OseaClientList *) (data[1]);
	OseaClientList                  * values = (OseaClientList *) (data[2]);
	gpointer               * userdata = NULL;
	GtkTreeIter              iter;

	value = (AosTaxVatValueHeader *) node;

	gtk_tree_store_append (store, &iter, NULL);
	switch (tax_vat_value_selected_order) {
	case TAX_VAT_VALUE_ORDER_FIRST_ITEM:
		gtk_tree_store_set (store, &iter,
				    TAX_VAT_VALUE_ID_COLUMN, value->id,
				    TAX_VAT_VALUE_REF_ITEM_COLUMN, value->reference,
				    TAX_VAT_VALUE_EDITABLE_COLUMN, FALSE,
				    -1);
		break;
	case TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER:
		gtk_tree_store_set (store, &iter,
				    TAX_VAT_VALUE_ID_COLUMN, value->id,
				    TAX_VAT_VALUE_REF_CUST_COLUMN, value->reference,
				    TAX_VAT_VALUE_EDITABLE_COLUMN, FALSE,
				    -1);
		break;
	}

	userdata = g_new (gpointer, 4);
	userdata[0] = store;
	userdata[1] = &iter;
	userdata[2] = values;
	userdata[3] = key;
	oseaclient_list_foreach (subrows, 
			__tax_vat_value_add_to_model_aux, 
			userdata); 

	g_free (userdata);

	return FALSE;
}

GtkTreeStore * tax_vat_value_regenerate_model (void)
{
	GtkTreeStore * new_store = NULL;
	GtkTreeStore * old_store = NULL;
	gpointer     * userdata;
	GtkTreeView  * view = NULL;

	new_store = gtk_tree_store_new (TAX_VAT_VALUE_COLUMN_NUMBER,
					G_TYPE_INT,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_BOOLEAN);
	
	userdata = g_new (gpointer, 3);
	userdata[0] = new_store;
	switch (tax_vat_value_selected_order) {
	case TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER:
		userdata[1] = tax_vat_value_items_tree;
		userdata[2] = tax_vat_value_tree;
		oseaclient_list_foreach (tax_vat_value_customers_tree,
				__tax_vat_value_add_to_model, userdata);
		break;
	case TAX_VAT_VALUE_ORDER_FIRST_ITEM:
		userdata[1] = tax_vat_value_customers_tree;
		userdata[2] = tax_vat_value_tree;
		oseaclient_list_foreach (tax_vat_value_items_tree,
				__tax_vat_value_add_to_model, userdata); 
		break;
	}
	g_free (userdata);

	view = GTK_TREE_VIEW (tax_vat_get_widget ("vat_value_treeview"));

	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));
	gtk_tree_view_expand_all (view);
	
	old_store = tax_vat_value_tree_store;
	tax_vat_value_tree_store = new_store;

	if (old_store) {
		g_object_unref (old_store);
	}

	return new_store;
}

void tax_vat_value_refresh_process (AsplDataList * datalist)
{
	OseaClientMultiData    * data = NULL;
	GtkTreeStore      * new_store = NULL;
	GtkCellRenderer   * renderer;
	GtkTreeView       * view = NULL;
	GtkTreeViewColumn * column;
	StatusBarMessage  * statusbarmsg = NULL;
	OseaClientList             * old_tax_vat_value_tree;
	OseaClientList             * old_tax_vat_value_customers_tree;
	OseaClientList             * old_tax_vat_value_items_tree;

	data = osea_datalist_get (datalist, "DATA");	
	
	statusbarmsg = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");

	main_window_remove_statusbar_message (statusbarmsg);

	main_window_set_progressbar (FALSE);

	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while refreshing data:\n%s"),
				  data->text_response);
		oseaclient_multi_free (data, TRUE);
		return;
	}

	old_tax_vat_value_tree = tax_vat_value_tree;
	old_tax_vat_value_customers_tree = tax_vat_value_customers_tree;
	old_tax_vat_value_items_tree = tax_vat_value_items_tree;

	tax_vat_value_customers_tree = data->multi_data->data;
	tax_vat_value_items_tree = data->multi_data->next->data;
	tax_vat_value_tree = data->multi_data->next->next->data;       

	if (old_tax_vat_value_tree) {
 		oseaclient_list_destroy (old_tax_vat_value_tree);
	}

	if (old_tax_vat_value_customers_tree) {
		oseaclient_list_destroy (old_tax_vat_value_customers_tree);
	}

	if (old_tax_vat_value_items_tree) {
		oseaclient_list_destroy (old_tax_vat_value_items_tree);
	}


	new_store = tax_vat_value_regenerate_model ();

	view = GTK_TREE_VIEW (tax_vat_get_widget ("vat_value_treeview"));
		
	if (! gtk_tree_view_get_columns (view)) {
		gtk_tree_view_set_headers_clickable (view, TRUE);
		if (tax_vat_value_selected_order == TAX_VAT_VALUE_ORDER_FIRST_CUSTOMER) {
			renderer = gtk_cell_renderer_text_new ();

			column = gtk_tree_view_column_new_with_attributes (_("Customer Type Ref."),
									   renderer,
									   "text", TAX_VAT_VALUE_REF_CUST_COLUMN,
									   NULL);
			gtk_tree_view_append_column (view, column);
			
			renderer = gtk_cell_renderer_text_new ();
			column = gtk_tree_view_column_new_with_attributes (_("Item Type Ref."),
									   renderer,
									   "text", TAX_VAT_VALUE_REF_ITEM_COLUMN,
									   NULL);
			gtk_tree_view_append_column (view, column);
		} else {
			renderer = gtk_cell_renderer_text_new ();
			column = gtk_tree_view_column_new_with_attributes (_("Item Type Ref."),
									   renderer,
									   "text", TAX_VAT_VALUE_REF_ITEM_COLUMN,
									   NULL);
			gtk_tree_view_append_column (view, column);
			
			renderer = gtk_cell_renderer_text_new ();
			column = gtk_tree_view_column_new_with_attributes (_("Customer Type Ref."),
									   renderer,
									   "text", TAX_VAT_VALUE_REF_CUST_COLUMN,
									   NULL);
			gtk_tree_view_append_column (view, column);
		}

		renderer = gtk_cell_renderer_text_new ();

		g_signal_connect (renderer, "edited", G_CALLBACK (tax_vat_value_cell_edited), NULL);

		column = gtk_tree_view_column_new_with_attributes (_("Value"),
								   renderer,
								   "text", TAX_VAT_VALUE_VALUE_COLUMN,
								   "editable", TAX_VAT_VALUE_EDITABLE_COLUMN,
								   NULL);

		gtk_tree_view_append_column (view, column);
		
	}

	oseaclient_multi_free (data, FALSE);
	osea_datalist_free (datalist);

}



void tax_vat_value_refresh (void)
{
	StatusBarMessage         * status_message;
	AsplDataList                    * datalist;
	gchar                    * tax_vat_value_last_transaction_event = NULL;
	

	g_return_if_fail (oseaclient_session_server_exists ("os-tax"));
	
	// Ask for data to our server
	
	tax_vat_value_last_transaction_event = event_source_arm_signal ("tax_vat_value_transaction", 
									(GSourceFunc) tax_vat_value_refresh_process);
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Asking os-tax server for VAT values..."),0);
	
	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", tax_vat_value_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);
	
	if (! aos_tax_vat_value_list (event_process_oseaclient_multidata,
					datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while refreshing \nfrom server 'os-tax'"));
		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);
	}
	return;
}


void tax_vat_value_set_process (AsplDataList *datalist)
{
	GtkTreeView          * view = NULL;
	StatusBarMessage     * statusbarmsg = NULL;
	AosTaxVatValueKey  * key;
	AosTaxVatValueData * node;
 	GtkTreeSelection     * selection; 
	gchar                * string;
 	GtkTreeIter          * iter; 
	OseaClientNulData         * data;
	glong                * pvalue;
	gint                   id_customer, id_item;

	data = osea_datalist_get (datalist, "DATA");
	pvalue = (glong *) osea_datalist_get (datalist, "VALUE");
	id_customer = GPOINTER_TO_INT (osea_datalist_get (datalist, "ID_CUSTOMER"));
	id_item = GPOINTER_TO_INT (osea_datalist_get (datalist, "ID_ITEM"));
	iter = (GtkTreeIter *) osea_datalist_get (datalist, "ITER");

	statusbarmsg = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");	
	main_window_remove_statusbar_message (statusbarmsg);
	
	main_window_set_progressbar (FALSE);

	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while setting data:\n%s"),
				  data->text_response);
		oseaclient_nul_free (data);
		gtk_tree_store_set (tax_vat_value_tree_store, iter,  
				    TAX_VAT_VALUE_VALUE_COLUMN, (gchar *) osea_datalist_get (datalist, "OLD_VALUE"),
				    TAX_VAT_VALUE_EDITABLE_COLUMN, TRUE,
				    -1);
		osea_datalist_free (datalist);
		return;
	}



	key = tax_vat_value_get_key (id_customer, id_item);
	node = g_new (AosTaxVatValueData, 1);
	node->key = key;
	node->value = *pvalue;	
	oseaclient_list_replace (tax_vat_value_tree, key, node);

 	view = GTK_TREE_VIEW (tax_vat_get_widget ("vat_value_treeview"));
 	selection = gtk_tree_view_get_selection (view); 

	string = g_strdup_printf ("%.2f", ((*pvalue)/100.00)); 
	gtk_tree_store_set (tax_vat_value_tree_store, iter,  
			    TAX_VAT_VALUE_VALUE_COLUMN, string,  
			    TAX_VAT_VALUE_EDITABLE_COLUMN, TRUE,
			    -1);
	
	oseaclient_nul_free (data);
	osea_datalist_free (datalist);

	return;
}


void tax_vat_value_set (gint id_customer, gint id_item, glong value, GtkTreeIter *iter)
{
	StatusBarMessage              * status_message;
	AsplDataList                  * datalist;
	glong                         * pvalue;
	gchar                         * tax_vat_value_last_transaction_event = NULL;
	gchar                         * old_text;
	gchar                         * new_text;

	g_return_if_fail (oseaclient_session_server_exists ("os-tax"));
		
	// Ask for data to our server
	tax_vat_value_last_transaction_event = event_source_arm_signal ("tax_vat_value_transaction",
									(GSourceFunc) tax_vat_value_set_process);
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Setting new VAT value..."),0);

	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", tax_vat_value_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);

	pvalue = g_new0 (glong, 1);
	*pvalue = value;
	osea_datalist_set_full (datalist, "VALUE", pvalue, g_free);
	
	osea_datalist_set (datalist, "ID_ITEM", GINT_TO_POINTER(id_item));
	osea_datalist_set (datalist, "ID_CUSTOMER", GINT_TO_POINTER(id_customer));
	osea_datalist_set_full (datalist, "ITER", iter, g_free);
	gtk_tree_model_get (GTK_TREE_MODEL(tax_vat_value_tree_store), iter, TAX_VAT_VALUE_VALUE_COLUMN, &old_text, -1);	
	osea_datalist_set_full (datalist, "OLD_VALUE", old_text, g_free);

	if (! aos_tax_vat_value_set (id_customer, id_item, value,
				       event_process_oseaclient_nuldata, datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while setting values \nin server 'os-tax'"));
		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);	

		return;
	}

	new_text = g_strdup_printf ("%.2f", ((value)/100.00)); 
	gtk_tree_store_set (tax_vat_value_tree_store, iter, TAX_VAT_VALUE_VALUE_COLUMN, new_text, -1);
	g_free (new_text);

	return;
}

/**
 * tax_vat_value_free_data:
 * 
 * Frees all internal module data.
 **/
void           tax_vat_value_free_data                 (void)
{
	// Free gtk-tree-store object
	if (tax_vat_value_tree_store) {

		g_object_unref (tax_vat_value_tree_store);

		tax_vat_value_tree_store = NULL;
	}
	
	// Free Gtrees recieved from liboseaclient level.
	if (tax_vat_value_tree) {
		oseaclient_list_destroy (tax_vat_value_tree);
		
		tax_vat_value_tree = NULL;
	}

	if (tax_vat_value_customers_tree) {
		oseaclient_list_destroy (tax_vat_value_customers_tree);
		
		tax_vat_value_customers_tree = NULL;
	}

	if (tax_vat_value_items_tree) {
		oseaclient_list_destroy (tax_vat_value_items_tree);
		
		tax_vat_value_items_tree = NULL;
	}

	return;
}
