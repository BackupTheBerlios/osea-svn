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

#include <config.h>
#include <glib.h>
#include <liboseacomm/oseacomm.h>
#include <aoskernel/aos_kernel.h>
#include <string.h>
#include "usergroup.h"
#include "usergroup_group.h"
#include "main_window.h"
#include "login_window.h"
#include "event_source.h"
#include "event_process.h"
#include "dialog.h"
#include "transaction_progress_window.h"
#include "widgets.h"


gint           usergroup_group_transaction_number = 0;
gchar        * usergroup_group_last_transaction_event = NULL;

GtkTreeStore * usergroup_group_list_store      = NULL;
GtkTreeStore * usergroup_group_user_list_store = NULL;
OseaClientList        * usergroup_group_list            = NULL;
OseaClientList        * usergroup_group_user_list       = NULL;

GladeXML     * usergroup_group_window_xml      = NULL;

typedef struct __UsergroupGroupSignalMessage {
	gchar            * signal;
	StatusBarMessage * message;
} UsergroupGroupSignalMessage;

GtkTreeStore * usergroup_group_list_store_get (void)
{
	return usergroup_group_list_store;
}


void usergroup_group_ok_clicked (GtkButton *button, gpointer user_data);

/*
 * Callbacks called when switching between tabs in the component.
 * 
 */


void usergroup_group_enable_selection_actions ()
{
	GtkWidget * widget;

	if (usergroup_get_active_notebook_tab() == USERGROUP_GROUP_TAB) {

		widget = usergroup_menubar_get_widget ("edit_group1");
		widgets_set_sensitive ("user", widget, TRUE);
		
		widget = usergroup_menubar_get_widget ("remove_group1");
		widgets_set_sensitive ("user", widget, TRUE);

		widget = usergroup_toolbar_get_widget ("remove_button");
		widgets_set_sensitive ("user_group", widget, TRUE);
		
		widget = usergroup_toolbar_get_widget ("new_button");
		widgets_set_sensitive ("user_group", widget, TRUE);
	}

	return;
}

void usergroup_group_disable_selection_actions ()
{
	GtkWidget * widget;

//	if (usergroup_get_active_notebook_tab() == USERGROUP_GROUP_TAB) {

		widget = usergroup_menubar_get_widget ("edit_group1");
		widgets_set_sensitive ("user", widget, FALSE);
		
		widget = usergroup_menubar_get_widget ("remove_group1");
		widgets_set_sensitive ("user", widget, FALSE);

		widget = usergroup_toolbar_get_widget ("new_button");
		widgets_set_sensitive ("user_group", widget, FALSE);
		
		widget = usergroup_toolbar_get_widget ("remove_button");
		widgets_set_sensitive ("user_group", widget, FALSE);

//	}

	return;
}

void usergroup_group_update_selection_actions ()
{
	GtkTreeView      * treeview;
	GtkTreeIter        iter;
	GtkTreeSelection * selection;
	GtkWidget        * widget;

//	if (usergroup_get_active_notebook_tab() == USERGROUP_GROUP_TAB) {

		treeview = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));
		selection = gtk_tree_view_get_selection (treeview);
		
		if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
			usergroup_group_enable_selection_actions ();
		} else {
			usergroup_group_disable_selection_actions ();
		}
		
		widget = usergroup_toolbar_get_widget ("new_button");
		widgets_set_sensitive ("user_group", widget, TRUE);

		widget = usergroup_menubar_get_widget ("refresh1");
		widgets_set_sensitive ("user", widget, TRUE);

		widget = usergroup_toolbar_get_widget ("refresh_button");
		widgets_set_sensitive ("user_group", widget, TRUE);
			
//	}

	return;
}

void usergroup_group_free_selection (void)
{
	GtkLabel    * label;
	GtkTreeView * treeview;

	treeview = GTK_TREE_VIEW (usergroup_get_widget ("usergroup_group_treeview"));
	gtk_tree_selection_unselect_all (gtk_tree_view_get_selection (treeview));

	label =  GTK_LABEL (usergroup_get_widget ("usergroup_group_name_label"));
	gtk_label_set_text (label, "");

	label =  GTK_LABEL (usergroup_get_widget ("usergroup_group_have_label"));
	gtk_label_set_text (label, "");

	usergroup_group_disable_selection_actions ();
	usergroup_group_update_selection_actions ();

	return;
}


/*
 * Function that returns the selected user's id 
 */

gint usergroup_group_get_id_selected (void)
{
	GtkTreeSelection * selection;
	GtkTreeIter        iter;
	gint               id = -1;
	GtkTreeView      * treeview;

	treeview = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_group_list_store),
				    &iter, USERGROUP_GROUP_ID_COLUMN, &id, -1);
	}

	return id;
}


/* 
 * Functions for refreshing the treeview (after pressing refresh button...)
 */

gboolean __usergroup_group_add_to_model (gpointer key, gpointer node, gpointer usr_data)
{
	GtkTreeIter           iter;
	AosKernelGroup * group;
	GtkTreeStore        * store = GTK_TREE_STORE (usr_data);

	group = (AosKernelGroup *) node;

	g_print ("Adding group: %d %s %s\n", group->id, group->name, group->description);

	gtk_tree_store_append (store, &iter, NULL);
	gtk_tree_store_set (store, &iter,
			    USERGROUP_GROUP_ID_COLUMN, group->id,
			    USERGROUP_GROUP_NAME_COLUMN, g_strdup(group->name),
			    USERGROUP_GROUP_DESCRIPTION_COLUMN, g_strdup(group->description),
			    -1);


	return FALSE;
}

static void usergroup_group_refresh_process (AsplDataList *datalist)
{
	OseaClientData         * data = osea_datalist_get (datalist, "DATA");
	GtkTreeStore      * new_store = NULL, *old_store = NULL;
	GtkCellRenderer   * renderer;
	GtkTreeView       * view = NULL;
	GtkTreeViewColumn * column;
	StatusBarMessage  * statusbarmsg = NULL;
	OseaClientList             * old_tree = NULL;

	// Clear status bar message
	statusbarmsg = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");
	
	main_window_remove_statusbar_message (statusbarmsg);

	main_window_set_progressbar (FALSE);

	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while refreshing data:\n%s"),
				  data->text_response);
		oseaclient_data_free (data, TRUE);
		return;
	}

	// Create a gtk-tree-store model
	new_store = gtk_tree_store_new (USERGROUP_GROUP_COLUMN_NUMBER,
					G_TYPE_INT,
					G_TYPE_STRING,
					G_TYPE_STRING);
	
	// Fill gtk-tree-store-model with our data
	oseaclient_list_foreach (data->data, __usergroup_group_add_to_model, new_store); 

	view = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));
	
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));
		
	if (! usergroup_group_list_store) {
		gtk_tree_view_set_headers_clickable (view, TRUE);

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes (_("Name"),
								   renderer,
								   "text", USERGROUP_GROUP_NAME_COLUMN,
								   NULL);
		gtk_tree_view_append_column (view, column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes (_("Description"),
								   renderer,
								   "text", USERGROUP_GROUP_DESCRIPTION_COLUMN,
								   NULL);
		gtk_tree_view_append_column (view, column);
		
		gtk_tree_view_set_search_column (view, USERGROUP_GROUP_NAME_COLUMN);
	}

	old_store = usergroup_group_list_store;
	usergroup_group_list_store = new_store;

	if (old_store) {
		g_object_unref (old_store);
	}

	old_tree = usergroup_group_list;
	usergroup_group_list = data->data;

	if (old_tree) {
		oseaclient_list_destroy (old_tree);
	}

	oseaclient_data_free (data, FALSE);
	osea_datalist_free (datalist);

	usergroup_group_free_selection ();

	usergroup_group_update_selection_actions ();

	return;
}



void usergroup_group_refresh (void)
{

	StatusBarMessage  * status_message;
	AsplDataList      * datalist;

	g_return_if_fail (oseaclient_session_server_exists ("os-kernel"));
	
	// Ask for data to our server
	// Create an event to be executed on os-kernel response
	usergroup_group_last_transaction_event = event_source_arm_signal ("usergroup_group_transaction", 
									  (GSourceFunc) usergroup_group_refresh_process);

	// Set up progress bar
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Asking os-kernel server for groups..."),0);


	// Initialize and fill up all needed data at event_process_oseaclient_data
	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", usergroup_group_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);


	// Make a request to the kernel
	if (! aos_kernel_group_list (0, 0, event_process_oseaclient_data, datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while refreshing \nfrom server 'os-kernel'"));

		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);	
	}
	return;
}


/*
 * Functions for removing groups 
 *
 */


static void usergroup_group_remove_process (AsplDataList *datalist)
{
	OseaClientNulData     * data = osea_datalist_get (datalist, "DATA");
	GtkTreeStore     * new_store = NULL;
	StatusBarMessage * statusbarmsg = NULL;
	GtkTreeView      * view = NULL;
	GtkTreeSelection * selection = NULL;
	GtkTreeIter        iter;
	gint               id;

	g_print ("Entering usergroup_group_remove_process\n");

	// Clear status bar message
	statusbarmsg = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");
	main_window_remove_statusbar_message (statusbarmsg);
	main_window_set_progressbar (FALSE);		

	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while removing group:\n%s"),
				  data->text_response);
		oseaclient_nul_free (data);
		return;
	}

	// Get which group is going to be removed
	view = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));
	selection = gtk_tree_view_get_selection (view);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {

		// Get the group id to be removed
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_group_list_store),
				    &iter, USERGROUP_GROUP_ID_COLUMN, &id, -1);
		
		g_print ("Removing: %d\n", id);
		// Remove it from our gtree
		oseaclient_list_remove (usergroup_group_list, GINT_TO_POINTER (id));


		// Create the new model
		new_store = gtk_tree_store_new (USERGROUP_GROUP_COLUMN_NUMBER,
						G_TYPE_INT,
						G_TYPE_STRING,
						G_TYPE_STRING);
		
		oseaclient_list_foreach (usergroup_group_list, __usergroup_group_add_to_model, new_store); 

	
		gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));

		if (usergroup_group_list_store) {
			g_object_unref (usergroup_group_list_store);
		}
		
		usergroup_group_list_store = new_store;
	}

	usergroup_group_free_selection();

	oseaclient_nul_free (data);
	osea_datalist_free (datalist);
	return;
}


void usergroup_group_remove (void)
{
	StatusBarMessage            * status_message;
	AsplDataList                       * datalist;
	GtkTreeSelection            * selection;
	GtkTreeView                 * treeview;
	AosKernelGroup             * group;
	GtkTreeIter                   iter;
	gint                          id, response;

	g_return_if_fail (oseaclient_session_server_exists ("os-kernel"));

	// Try to get an iter to the group and finally, group data.
	treeview = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));

	selection = gtk_tree_view_get_selection (treeview);

	if (!gtk_tree_selection_get_selected (selection, NULL, &iter)) 
		return;
	
	gtk_tree_model_get (GTK_TREE_MODEL (usergroup_group_list_store),
			    &iter, USERGROUP_GROUP_ID_COLUMN, &id, -1);
	
	group = (AosKernelGroup *) oseaclient_list_lookup (usergroup_group_list, GINT_TO_POINTER (id));


	
	// Ask user if he wants to delete the group
	response = dialog_cancel_ok_run (main_window_get (), GTK_MESSAGE_WARNING,
					 _("Are you sure you want to delete '%s' group?\n"),
					 group->name);

	if (response == GTK_RESPONSE_CANCEL)
		return;

	// Set up progress bar 
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Removing Group..."),0);
	
	
	// Ask for data to our server
	// Create and arm a signal
	usergroup_group_last_transaction_event = event_source_arm_signal ("group_transaction",
									  (GSourceFunc) usergroup_group_remove_process);

	// Create all data needed by usergroup_remove_process function
	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", usergroup_group_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);

	// Ask kernel to remove a group
	if (! aos_kernel_group_remove (id, event_process_oseaclient_nuldata, datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while removing Group"));
		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);	
		
	}
	return;
}



/*
 * Functions for managing the group's properties window (for creating
 * and editing groups).
 *
 */

gboolean  __usergroup_group_user_add_to_model (gpointer key, gpointer node, gpointer usr_data)
{
	GtkTreeIter        iter;
	AosKernelUser * user;
	GtkTreeStore     * store = GTK_TREE_STORE (usr_data);

	user = (AosKernelUser *) node;

	g_print ("Adding user: %d %s %s\n", user->id, user->nick, user->description);

	gtk_tree_store_append (store, &iter, NULL);
	gtk_tree_store_set (store, &iter,
			    USERGROUP_GROUP_USER_NAME, user->nick,
			    USERGROUP_GROUP_USER_SELECTED, FALSE,
			    USERGROUP_GROUP_USER_ID, user->id,
			    USERGROUP_GROUP_USER_ACTIVATABLE, TRUE,
			    -1);

	return FALSE;
}

gboolean usergroup_group_user_remove_process (AsplDataList * datalist) {
	
	GtkWidget    * group_window;
	gint           found_id = 0;
	OseaClientNulData * data = osea_datalist_get (datalist, "DATA");
	gint           id = GPOINTER_TO_INT (osea_datalist_get(datalist, "ID"));
	gboolean       new_value = GPOINTER_TO_INT (osea_datalist_get(datalist, "VALUE"));
	gboolean       found = FALSE, valid = TRUE;
	GtkTreeIter    iter;

	if (data->state == OSEACLIENT_ERROR) {
		group_window = glade_xml_get_widget (usergroup_group_window_xml, "group_window");
		dialog_close_run (GTK_WINDOW (group_window), GTK_MESSAGE_ERROR,
				  _("There was a problem: %s"), data->text_response);
		return TRUE;
	}

	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL(usergroup_group_user_list_store), &iter);
	
	while (valid && !found) {
		/* Walk through the list, reading each row */
		gtk_tree_model_get (GTK_TREE_MODEL(usergroup_group_user_list_store), &iter, 
				    USERGROUP_GROUP_USER_ID, &found_id,
				    -1);

		if (found_id == id)
			found = TRUE;
		else 
			valid = gtk_tree_model_iter_next (GTK_TREE_MODEL(usergroup_group_user_list_store), &iter);

	}

	if (found)
		gtk_tree_store_set (usergroup_group_user_list_store, &iter,
				    USERGROUP_GROUP_USER_SELECTED, new_value,
				    USERGROUP_GROUP_USER_ACTIVATABLE, TRUE,
				    -1);
	

	osea_datalist_free (datalist);
	return TRUE;
}

void usergroup_group_window_cell_toggled (GtkCellRendererToggle *cell,
					  gchar                 *path_str,
					  gpointer               data)
{
	GtkTreeModel * model         = (GtkTreeModel *) osea_datalist_get (data, "STORE");
	GtkTreePath  * path          = gtk_tree_path_new_from_string (path_str);
	GtkWidget    * group_window;
	AsplDataList * datalist      = NULL;
	GtkTreeIter    iter;
	gboolean       selected;
	gboolean       is_new        = GPOINTER_TO_INT (osea_datalist_get (data, "IS_NEW"));
	gint           group_id      = GPOINTER_TO_INT (osea_datalist_get (data, "GROUP_ID"));
	gint           user_id;
	
	// 'is_new' var is telling us if we are editing to creating. The policy will be: if we are creating
	// and a cell state is toggle we do nothing. But if we are editing we try to update at real-time user
	// desire.
	
 	gtk_tree_model_get_iter (model, &iter, path);

	gtk_tree_model_get (model, &iter, 
			    USERGROUP_GROUP_USER_SELECTED, &selected, 
			    USERGROUP_GROUP_USER_ID, &user_id,
			    -1);
	
	if (!is_new ) {
		// We are editing so prepare all stuff to change at real-time
		usergroup_group_last_transaction_event = event_source_arm_signal ("usergroup_group_user_remove", 
										  (GSourceFunc) usergroup_group_user_remove_process);
		// Initialize and fill up all needed data at event_process_oseaclient_data
		datalist = osea_datalist_new ();
		osea_datalist_set (datalist, "SIGNAL", usergroup_group_last_transaction_event);
		osea_datalist_set (datalist, "VALUE", GINT_TO_POINTER(!selected));
		osea_datalist_set (datalist, "ID", GINT_TO_POINTER(user_id));
	}

	if (selected) {
		selected = FALSE;
		if ((!is_new) && (!aos_kernel_group_remove_user (group_id, user_id, event_process_oseaclient_nuldata, datalist))) {
			group_window = glade_xml_get_widget (usergroup_group_window_xml, "group_window");
			dialog_close_run (GTK_WINDOW (group_window), GTK_MESSAGE_ERROR,
					  _("There was a problem while removing user to this group."));
		}

	}else {
		selected = TRUE;
		if ((!is_new) && (!aos_kernel_group_add_user (group_id, user_id, event_process_oseaclient_nuldata, datalist))) {
			group_window = glade_xml_get_widget (usergroup_group_window_xml, "group_window");
			dialog_close_run (GTK_WINDOW (group_window), GTK_MESSAGE_ERROR,
					  _("There was a problem while adding user to this group."));
		}
	}

	if (is_new)
		gtk_tree_store_set (GTK_TREE_STORE (model), &iter,  USERGROUP_GROUP_USER_SELECTED, selected, -1);
	else
		gtk_tree_store_set (GTK_TREE_STORE (model), &iter,  USERGROUP_GROUP_USER_ACTIVATABLE, FALSE, -1);
	
	gtk_tree_path_free (path);

	g_print ("Cell toggled\n");
	return;
}

gboolean  __usergroup_group_select_user_on_model (GtkTreeModel *model,
						  GtkTreePath *path,
						  GtkTreeIter *iter,
						  gpointer data)
{
	AosKernelUser * user = (AosKernelUser *) data;
	gint              model_user_id;
	gboolean          model_user_selected;
	
	gtk_tree_model_get (model, iter, 
			    USERGROUP_GROUP_USER_ID, &model_user_id,
			    USERGROUP_GROUP_USER_SELECTED, &model_user_selected,
			    -1);

	if (user->id == model_user_id) {
		g_print ("We have found an user on this group..\n");
		gtk_tree_store_set (GTK_TREE_STORE (model), iter, 
				    USERGROUP_GROUP_USER_SELECTED, TRUE,
				    -1);
		return TRUE;
	}
	return FALSE;
}

gboolean  __usergroup_group_select_user (gpointer key, gpointer node, gpointer usr_data)
{

	gtk_tree_model_foreach (GTK_TREE_MODEL (usr_data), __usergroup_group_select_user_on_model, node);
	return FALSE;
}

gboolean usergroup_group_properties_step2_process (AsplDataList * datalist) 
{
	OseaClientData    * data  = osea_datalist_get (datalist, "DATA");
	
	// Select all user from data->data
	oseaclient_list_foreach (data->data, __usergroup_group_select_user, osea_datalist_get (datalist, "STORE"));

	// Hide progress window launched on usergroup_group_properties_step1_process
	transaction_progress_window_destroy ();

	return TRUE;
}

gboolean usergroup_group_properties_step1_process (AsplDataList * datalist)
{
	GtkTreeStore      * new_store    = NULL, *old_store = NULL;
	GtkCellRenderer   * renderer;
	GtkTreeView       * view         = NULL;
	GtkTreeViewColumn * column;
	GtkTreeSelection  * selection    = NULL;
	OseaClientList             * old_tree     = NULL;
	GtkTreeIter         iter;
	gboolean            is_new       = GPOINTER_TO_INT (osea_datalist_get (datalist, "IS_NEW"));
	OseaClientData         * data         = osea_datalist_get (datalist, "DATA");
	gint                group_id     = 0;
	GtkWindow         * group_window = NULL;
	GtkWidget         * widget       = NULL;
	

	g_print ("Recieved datalist\n");
	osea_datalist_print (datalist);

	// Create a gtk-tree-store model
	new_store = gtk_tree_store_new (USERGROUP_GROUP_USER_COLUMN_NUMBER,
					G_TYPE_STRING,
					G_TYPE_BOOLEAN,
					G_TYPE_INT,
					G_TYPE_BOOLEAN);
	
	// Fill gtk-tree-store-model with our data
	oseaclient_list_foreach (data->data, __usergroup_group_user_add_to_model, new_store); 
	
	view = GTK_TREE_VIEW (glade_xml_get_widget (usergroup_group_window_xml,"group_window_treeview"));
	
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));
	
	// Create all visible columns
	
	gtk_tree_view_set_headers_clickable (view, TRUE);
	
	// Create boolean-selection column. We also send a datalist which contains the model and is_new var.
	// With this we are telling toggled callback what to do when user click. 
	// See usergroup_group_window_cell_toggled.

	renderer = gtk_cell_renderer_toggle_new ();

	// get the group identifier throught selected group in the group-treeview. 
	if (!is_new) {
		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (usergroup_get_widget ("group_treeview")));	
		
		if (!gtk_tree_selection_get_selected (selection, NULL, &iter)) 
			return FALSE;
		
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_group_list_store),
				    &iter, USERGROUP_GROUP_ID_COLUMN, &group_id, -1);
	}

	datalist = osea_datalist_new ();
	osea_datalist_set (datalist, "STORE", new_store);
	osea_datalist_set (datalist, "IS_NEW", GINT_TO_POINTER (is_new));
	osea_datalist_set (datalist, "GROUP_ID", GINT_TO_POINTER (group_id));

	g_signal_connect (G_OBJECT (renderer), "toggled",
			  G_CALLBACK (usergroup_group_window_cell_toggled), datalist);

	column = gtk_tree_view_column_new_with_attributes (_("Selected"),
							   renderer,
							   "active", USERGROUP_GROUP_USER_SELECTED,
							   "activatable", USERGROUP_GROUP_USER_ACTIVATABLE,
							   NULL);
	gtk_tree_view_append_column (view, column);
	
	// Create user name column
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Name"),
							   renderer,
							   "text", USERGROUP_GROUP_USER_NAME,
							   NULL);
	gtk_tree_view_append_column (view, column);
	
	
	// Delete all unused items
	old_store = usergroup_group_user_list_store;
	usergroup_group_user_list_store = new_store;
	
	if (old_store) {
		g_object_unref (old_store);
	}
	
	old_tree = usergroup_group_user_list;
	usergroup_group_user_list = data->data;
	
	if (old_tree) {
		oseaclient_list_destroy (old_tree);
	}
	
	oseaclient_data_free (data, FALSE);

	// Check if we are editing the group to ask kernel which user belongs to this group
	if (!is_new) {
		g_print ("This group is not new..editing\n");
		// Due to this operation could be longer we show this progress window. But we need an event,
		// so we are going to create it at this point. This is not usual because this operation usually is
		// taken just before make an oseaclient_* service petition.
		usergroup_group_last_transaction_event = event_source_arm_signal ("usergroup_group_user_transaction", 
										  (GSourceFunc) usergroup_group_properties_step2_process);
		
		// Initialize and fill up all needed data at event_process_oseaclient_data
		datalist = osea_datalist_new ();
		osea_datalist_set (datalist, "SIGNAL", usergroup_group_last_transaction_event);
		osea_datalist_set (datalist, "STORE", new_store);
		
		if (!aos_kernel_group_list_user (0, 0, group_id, event_process_oseaclient_data, datalist)) {
			group_window = GTK_WINDOW (glade_xml_get_widget (usergroup_group_window_xml, "group_window"));
			dialog_close_run (group_window, GTK_MESSAGE_ERROR,
					  _("There was a problem while asking for already created users."));
		}
		// Re-link again button ok clicked signal. This is done to report usergroup_group_ok_clicked
		// cb that the group which is been edited and not created.
		datalist = osea_datalist_new ();
		osea_datalist_set (datalist, "IS_NEW", GINT_TO_POINTER (FALSE));
		osea_datalist_set (datalist, "GROUP_ID", GINT_TO_POINTER (group_id));
		
		// First we must disconnect old signal
		widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_button_ok");
		g_signal_handlers_disconnect_matched (widget, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, 
						      usergroup_group_ok_clicked, NULL);

		g_signal_connect (widget, "clicked", (GCallback) usergroup_group_ok_clicked,  datalist);
		return TRUE;
	}
	// Re-link again button ok clicked signal. This is done to report usergroup_group_ok_clicked
	// cb that the group which is been created and not edited.
	datalist = osea_datalist_new ();
	osea_datalist_set (datalist, "IS_NEW", GINT_TO_POINTER (TRUE));
	osea_datalist_set (datalist, "GROUP_ID", GINT_TO_POINTER (0));

	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_button_ok");
	g_signal_handlers_disconnect_matched (widget, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, 
					      usergroup_group_ok_clicked, NULL);
	
	g_signal_connect (widget, "clicked", (GCallback) usergroup_group_ok_clicked,  datalist);
	return TRUE;
}

void usergroup_group_window_show (gchar *name, gchar *description, gboolean is_new)
{
	gchar         * xml_file = NULL;
	gchar         * title      = NULL;
	GtkWidget     * widget     = NULL;
	GtkWidget     * name_entry = NULL;
	GtkTextBuffer * txtbuf     = NULL;
	AsplDataList  * datalist   = NULL;
	GtkWidget     * group_window = NULL;

	g_return_if_fail (! usergroup_group_window_xml);

	// Load group window
	xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "user", NULL );
		
	usergroup_group_window_xml = widgets_load (xml_file, "group_window", NULL);
		
	if (name) {
		widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_name_entry");
		gtk_entry_set_text (GTK_ENTRY(widget), name);
	}

	if (description) {
		widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_description_textview");
		txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
		gtk_text_buffer_set_text (txtbuf, description, -1);
	}

	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window");

	gtk_window_set_transient_for (GTK_WINDOW(widget), main_window_get ());
	main_window_set_active_window (GTK_WINDOW(widget));

	if (name) {
		title = g_strdup_printf (_("%s properties - ASPL Fact"), name);
		gtk_window_set_title (GTK_WINDOW(widget), (const gchar *) title);
	}

	// set the focus
  	name_entry =  glade_xml_get_widget (usergroup_group_window_xml, "group_window_name_entry"); 

	gtk_widget_grab_focus (name_entry);

	gtk_widget_show (widget);

	// Check if user want to create a group or to edit preferences on already existing one
	// We are going to differenciate both cases with is_new param. 
	if (!is_new) {
		// We also launch this progress window because we must retrieve information about the group
		// which is going to be edited
		widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window");
		transaction_progress_window_show (GTK_WINDOW (widget), 
						  _("Retrieving group information..."), 
						  "no_event_to_cancel_signal");
				
	}
	usergroup_group_last_transaction_event = event_source_arm_signal ("usergroup_group_properties_step1_transaction", 
									  (GSourceFunc) usergroup_group_properties_step1_process);
	// Initialize and fill up all needed data at event_process_oseaclient_data
	datalist = osea_datalist_new ();
	osea_datalist_set (datalist, "SIGNAL", usergroup_group_last_transaction_event);
	osea_datalist_set (datalist, "IS_NEW", GINT_TO_POINTER (is_new));
	
	if (!aos_kernel_user_list (0, 0, event_process_oseaclient_data, datalist)) {
		group_window = glade_xml_get_widget (usergroup_group_window_xml, "group_window");
		dialog_close_run (GTK_WINDOW (group_window), GTK_MESSAGE_ERROR,
				  _("There was a problem while asking for already created users."));
	}
	return;

}

void usergroup_group_window_destroy ()
{
	GtkWidget  * group_window = NULL;

	group_window = glade_xml_get_widget (usergroup_group_window_xml, "group_window");

	g_object_unref (G_OBJECT (usergroup_group_window_xml));
	usergroup_group_window_xml = NULL;
	
	gtk_object_destroy (GTK_OBJECT (group_window));
	group_window = NULL;	
	
	main_window_unset_active_window ();

	return;
}




void usergroup_group_new (void)
{
	usergroup_group_free_selection ();
	usergroup_group_window_show (NULL, NULL, TRUE);

	return;
}

void usergroup_group_edit (void)
{
	GtkTreeSelection    * selection;
	GtkTreeView         * treeview;
	AosKernelGroup    * group;
	GtkTreeIter           iter;
	gint                  id;

	g_print ("Editing group preferences..");


	treeview = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));	
	selection = gtk_tree_view_get_selection (treeview);
	
	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_group_list_store),
				    &iter, USERGROUP_GROUP_ID_COLUMN, &id, -1);
		group = (AosKernelGroup *) oseaclient_list_lookup (usergroup_group_list, GINT_TO_POINTER (id));

		usergroup_group_window_show (group->name, group->description, FALSE);
	} 
	
	return;
}






//*****************************************************************************************
// Group editing window






void usergroup_group_window_name_entry_changed (GtkEntry *name_entry, gpointer user_data)
{
	const gchar * text;
	gchar       * title;//, * old_title;
	GtkWidget   * widget = NULL;

	text = gtk_entry_get_text (name_entry);
	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window");

//	old_title = (gchar *) gtk_window_get_title (GTK_WINDOW(widget));
	
	if (!text || !text[0])
		gtk_window_set_title (GTK_WINDOW(widget), (const gchar *) g_strdup ("Group properties - ASPL Fact"));
	else {
		title = g_strdup_printf (_("%s properties - ASPL Fact"), text);
		gtk_window_set_title (GTK_WINDOW(widget), (const gchar *) title);
	}

//	g_free (old_title);
		
	return;
}

void usergroup_group_add_user_process (AsplDataList * datalist)
{
	OseaClientNulData * data = osea_datalist_get (datalist, "DATA");
	GtkWindow    * group_window;
	
	if (data->state == OSEACLIENT_ERROR) {
		group_window = GTK_WINDOW(glade_xml_get_widget (usergroup_group_window_xml, "group_window"));
		dialog_close_run (group_window, GTK_MESSAGE_ERROR,
				  _("There was a problem adding user to the group:%s"),
				  data->text_response);
	}
	oseaclient_nul_free (data);
	osea_datalist_free (datalist);
	return;
}


static void usergroup_group_new_process (AsplDataList *datalist)
{
	OseaClientSimpleData     * data;
	GtkWindow           * group_window = NULL;
	GtkWidget           * widget = NULL;
	GtkTreeStore        * new_store = NULL;
	GtkTextBuffer       * txtbuf = NULL;
	GtkTreeView         * view = NULL;
	AosKernelGroup    * group = NULL;
	GtkTextIter           iter1, iter2;
	GtkTreeIter           iter;
	gboolean              valid = FALSE;
	gint                  user_id;
	gboolean              user_selected;
	gchar               * user_name;
	

	// check out all oseaclient data
	data = osea_datalist_get (datalist, "DATA");

	// Shut down progress window 
	transaction_progress_window_destroy ();
	group_window = GTK_WINDOW(glade_xml_get_widget (usergroup_group_window_xml, "group_window"));
	
	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (group_window, GTK_MESSAGE_ERROR,
				  _("There was a problem while creating group:\n%s"),
				  data->text_response);
		oseaclient_simple_free (data);
		return;
	}

	// Create the new sent group insted of ask for all groups and insert it
	// on usergroup_group_list
	group = g_new0 (AosKernelGroup, 1);
	group->id = data->id;

	// Get group's name from group_window
	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_name_entry");
	group->name = g_strdup (gtk_entry_get_text (GTK_ENTRY(widget)));

	// Get group's description from group_window
	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_description_textview");
	txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
	gtk_text_buffer_get_start_iter (txtbuf, &iter1);
	gtk_text_buffer_get_end_iter (txtbuf, &iter2);
	group->description = gtk_text_buffer_get_text (txtbuf, &iter1, &iter2, FALSE);	

	g_print ("ADDING: %d %s %s\n", group->id, group->name, group->description);

	oseaclient_list_insert (usergroup_group_list, GINT_TO_POINTER (group->id), group);

	// Since we have checked out all needed data from group window, we can shut down it.
	usergroup_group_window_destroy ();

	// Create the new model
	new_store = gtk_tree_store_new (USERGROUP_GROUP_COLUMN_NUMBER,
					G_TYPE_INT,
					G_TYPE_STRING,
					G_TYPE_STRING);
	
	oseaclient_list_foreach (usergroup_group_list, __usergroup_group_add_to_model, new_store); 

	view = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));
	
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));

	if (usergroup_group_list_store) {
		g_object_unref (usergroup_group_list_store);
	}

	usergroup_group_list_store = new_store;

	oseaclient_simple_free (data);
	osea_datalist_free (datalist);


	// Insert into the group all selected users 
	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL (usergroup_group_user_list_store), &iter);

	while (valid) {
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_group_user_list_store), &iter,
				    USERGROUP_GROUP_USER_ID, &user_id,
				    USERGROUP_GROUP_USER_SELECTED, &user_selected,
				    USERGROUP_GROUP_USER_NAME, &user_name,
				    -1);

		if (user_selected) {
			usergroup_group_last_transaction_event = event_source_arm_signal ("usergroup_group_add_user_transaction", 
											  (GSourceFunc) usergroup_group_add_user_process);
			// Initialize and fill up all needed data at event_process_oseaclient_data
			datalist = osea_datalist_new ();
			osea_datalist_set (datalist, "SIGNAL", usergroup_group_last_transaction_event);
			
			
			if (!aos_kernel_group_add_user (group->id, user_id, event_process_oseaclient_nuldata, datalist)) {
				dialog_close_run (GTK_WINDOW (group_window), GTK_MESSAGE_ERROR,
						  _("There was a problem while add user %s to the group."), user_name);
			}
		}
		valid = gtk_tree_model_iter_next (GTK_TREE_MODEL (usergroup_group_user_list_store), &iter);
		
	}

	return;
	
}

static void usergroup_group_edit_process (AsplDataList *datalist)
{
	OseaClientNulData        * data;
	GtkWindow           * group_window = NULL;
	GtkWidget           * widget = NULL;
	GtkTreeStore        * new_store = NULL;
	GtkTextBuffer       * txtbuf = NULL;
	GtkTreeView         * view = NULL;
	GtkTreeSelection    * selection;
	AosKernelGroup     * group = NULL, *old_group = NULL;
	GtkTextIter           iter1, iter2;
	GtkTreeIter           iter;
	gint                  id = -65535;

	data = osea_datalist_get (datalist, "DATA");

	view = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));	
	selection = gtk_tree_view_get_selection (view);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter))
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_group_list_store),
				    &iter, USERGROUP_GROUP_ID_COLUMN, &id, -1);

	if (id == -65535) {
		oseaclient_nul_free (data);
		return;
	}

	transaction_progress_window_destroy ();
	group_window = GTK_WINDOW(glade_xml_get_widget (usergroup_group_window_xml, "group_window"));
	
	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (group_window, GTK_MESSAGE_ERROR,
				  _("There was a problem while editing group:\n%s"),
				  data->text_response);
		oseaclient_nul_free (data);
		return;
	}

	group = g_new0 (AosKernelGroup, 1);
	group->id = id;

	old_group = oseaclient_list_lookup (usergroup_group_list, GINT_TO_POINTER (id));

	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_name_entry");
	group->name = g_strdup (gtk_entry_get_text (GTK_ENTRY(widget)));

	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_description_textview");
	txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
	gtk_text_buffer_get_start_iter (txtbuf, &iter1);
	gtk_text_buffer_get_end_iter (txtbuf, &iter2);
	group->description = gtk_text_buffer_get_text (txtbuf, &iter1, &iter2, FALSE);	

	usergroup_group_window_destroy ();

	oseaclient_list_replace (usergroup_group_list, GINT_TO_POINTER (group->id), group);

	new_store = gtk_tree_store_new (USERGROUP_GROUP_COLUMN_NUMBER,
					G_TYPE_INT,
					G_TYPE_STRING,
					G_TYPE_STRING);
	
	oseaclient_list_foreach (usergroup_group_list, __usergroup_group_add_to_model, new_store); 

	view = GTK_TREE_VIEW (usergroup_get_widget ("group_treeview"));
	
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));

	if (usergroup_group_list_store) {
		g_object_unref (usergroup_group_list_store);
	}

	usergroup_group_list_store = new_store;

	oseaclient_nul_free (data);
	osea_datalist_free (datalist);
	return;

}


void usergroup_group_ok_clicked (GtkButton *button, gpointer user_data)
{

	GtkWindow          * group_window  = NULL;
	GtkWidget          * widget        = NULL;
	GtkTextBuffer      * txtbuf        = NULL;
	gchar              * name          = NULL;
	gchar              * description   = NULL;
	GtkTextIter          iter1, iter2;
	gboolean             is_new        = GPOINTER_TO_INT (osea_datalist_get (user_data, "IS_NEW"));
	AsplDataList       * datalist      = NULL;
	gint                 group_id      = GPOINTER_TO_INT (osea_datalist_get (user_data, "GROUP_ID"));

	osea_datalist_print ((AsplDataList *) user_data);
	// check if we have connection to os-kernel
	g_return_if_fail (oseaclient_session_server_exists ("os-kernel"));

	// Get the window
	group_window = GTK_WINDOW(glade_xml_get_widget (usergroup_group_window_xml, "group_window"));

	// Gets name entry's content
	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_name_entry");
	name = g_strdup (gtk_entry_get_text (GTK_ENTRY(widget)));
	if (!(name && *name)) {
		dialog_close_run (group_window, GTK_MESSAGE_ERROR,
				  _("You must provide a valid group name"));
		return;
	}

	// Gets description entry's content
	widget = glade_xml_get_widget (usergroup_group_window_xml, "group_window_description_textview");
	txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));

	gtk_text_buffer_get_start_iter (txtbuf, &iter1);
	gtk_text_buffer_get_end_iter (txtbuf, &iter2);
	description = gtk_text_buffer_get_text (txtbuf, &iter1, &iter2, FALSE);	

	// Start transaction
	usergroup_group_last_transaction_event = g_strdup_printf ("group_transaction %d", ++usergroup_group_transaction_number);

	datalist = osea_datalist_new ();
	osea_datalist_set_full (datalist, "SIGNAL", usergroup_group_last_transaction_event, g_free);
	
	event_source_add_signal (usergroup_group_last_transaction_event);
	
	if (is_new) {

		// New transaction 
		transaction_progress_window_show (GTK_WINDOW (group_window), 
						  _("Creating group..."), 
						  usergroup_group_last_transaction_event);
		event_source_set_callback (usergroup_group_last_transaction_event, 
					   (GSourceFunc) usergroup_group_new_process);
		
		if (! aos_kernel_group_new (name, description, event_process_oseaclient_simpledata, datalist)) {
			dialog_close_run 
				(main_window_get (), GTK_MESSAGE_ERROR,
				 _("There was a problem while refreshing \nfrom server 'os-kernel'"));
			transaction_progress_window_destroy ();
		}
		
	} else {
			
		// Edit transaction
		transaction_progress_window_show (GTK_WINDOW (group_window), 
						  _("Updating group..."), 
						  usergroup_group_last_transaction_event);
		event_source_set_callback (usergroup_group_last_transaction_event, 
					   (GSourceFunc) usergroup_group_edit_process);
		if (! aos_kernel_group_edit (group_id, name, description,  event_process_oseaclient_nuldata, datalist)) {
			dialog_close_run 
				(main_window_get (), GTK_MESSAGE_ERROR,
				 _("There was a problem while refreshing \nfrom server 'os-kernel'"));
			transaction_progress_window_destroy ();
		}
		
	}
	
	g_free (name);
	g_free (description);
	
	return;
}

gboolean usergroup_group_window_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	usergroup_group_window_destroy ();
	return FALSE;
}

/**
 * usergroup_group_free_data:
 * 
 * Frees all internal module data.
 **/
void           usergroup_group_free_data                 (void)
{
	// Free gtk-list-store object
	if (usergroup_group_list_store) {

		g_object_unref (usergroup_group_list_store);
		
		usergroup_group_list_store = NULL;
	}
	
	// Free Gtree recieved from liboseaclient level.
	if (usergroup_group_list) {
		oseaclient_list_destroy (usergroup_group_list);
		
		usergroup_group_list = NULL;
	}

	return;
}

gboolean __usergroup_group_add_to_string (gpointer key, gpointer node, gpointer usr_data)
{
	AosKernelUser * user;
	GString         * string = (GString *) usr_data;

	user = (AosKernelUser *) node;

	if (string->len != 0)
		g_string_append_printf (string, ", ");
	
	g_string_append_printf (string, "%s", user->nick);

	return FALSE;
}


void usergroup_group_cursor_changed_process (AsplDataList *datalist)
{
	OseaClientData         * data = osea_datalist_get (datalist, "DATA");
	GString           * string;
	GtkLabel          * label;
	StatusBarMessage  * statusbarmsg;

	statusbarmsg = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");
	main_window_remove_statusbar_message (statusbarmsg);
	main_window_set_progressbar (FALSE);

	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while refreshing data:\n%s"),
				  data->text_response);
		oseaclient_data_free (data, TRUE);
		return;
	}

	string = g_string_new ("");

	oseaclient_list_foreach (data->data, __usergroup_group_add_to_string, string);

	label =  GTK_LABEL (usergroup_get_widget ("usergroup_group_have_label"));
	gtk_label_set_text (label, (const gchar *) string->str);
	g_string_free (string, TRUE);
	
	oseaclient_data_free (data, TRUE);
	osea_datalist_free (datalist);

	return;	
}

void usergroup_group_cursor_changed (GtkTreeView *treeview, gpointer user_data)
{
	GtkTreeSelection    * selection;
	GtkTreeIter           iter;
	GtkLabel            * label;
	AosKernelGroup    * group;
	gint                  id;
	StatusBarMessage    * status_message;
	AsplDataList        * datalist;
	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {

		usergroup_group_enable_selection_actions ();
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_group_list_store),
				    &iter, USERGROUP_GROUP_ID_COLUMN, &id, -1);
		
		group = (AosKernelGroup *) oseaclient_list_lookup (usergroup_group_list, GINT_TO_POINTER (id));

		label =  GTK_LABEL (usergroup_get_widget ("usergroup_group_name_label"));
		gtk_label_set_text (label, (const gchar *) group->name);

		label =  GTK_LABEL (usergroup_get_widget ("usergroup_group_have_label"));
		gtk_label_set_text (label, "...");

		if (! oseaclient_session_get_permission ("os-kernel", "list_user"))
			return;

		usergroup_group_last_transaction_event = event_source_arm_signal ("group_transaction",  
										 (GSourceFunc) usergroup_group_cursor_changed_process);

		main_window_set_progressbar (TRUE);
		status_message = main_window_set_statusbar_message (_("Getting group's user..."),0);
		
		datalist = osea_datalist_new ();
		
		osea_datalist_set_full (datalist, "SIGNAL", usergroup_group_last_transaction_event, g_free);
		osea_datalist_set (datalist, "MESSAGE", status_message);

		if (! aos_kernel_group_list_user (0, 0, id, event_process_oseaclient_data, datalist)) {
			dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
					  _("There was a problem while getting group's users"));
			main_window_remove_statusbar_message (status_message);
			main_window_set_progressbar (FALSE);
			
		}
	}
	
	return;
}
