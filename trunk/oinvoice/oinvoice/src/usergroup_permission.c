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
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>
#include <string.h>
#include "usergroup.h"
#include "usergroup_user.h"
#include "usergroup_group.h"
#include "usergroup_permission.h"
#include "main_window.h"
#include "login_window.h"
#include "event_source.h"
#include "event_process.h"
#include "dialog.h"
#include "widgets.h"
#include "transaction_progress_window.h"


gint           usergroup_permission_transaction_number = 0;
gchar        * usergroup_permission_last_transaction_event = NULL;

GtkTreeStore * usergroup_permission_list_store = NULL;
OseaClientList        * usergroup_permission_list = NULL;


GtkTreeStore * usergroup_permission_group_store  = NULL;
GtkTreeStore * usergroup_permission_user_store   = NULL;

GladeXML     * usergroup_permission_window_xml = NULL;

typedef struct __UsergroupPermissionSignalMessage {
	gchar            * signal;
	StatusBarMessage * message;
} UsergroupPermissionSignalMessage;

enum { USERGROUP_PERMISSION_ID_COLUMN, 
       USERGROUP_PERMISSION_SERVER_COLUMN,
       USERGROUP_PERMISSION_NAME_COLUMN,
       USERGROUP_PERMISSION_DESCRIPTION_COLUMN,
       USERGROUP_PERMISSION_USER_COLUMN,
       USERGROUP_PERMISSION_GROUP_COLUMN,
       USERGROUP_PERMISSION_ONLY_GROUP_COLUMN,
       USERGROUP_PERMISSION_EFFECTIVE_COLUMN,
       USERGROUP_PERMISSION_DEPEND_COLUMN,
       USERGROUP_PERMISSION_COLUMN_NUMBER };

typedef enum { USERGROUP_PERMISSION_USER_SELECTION,
	       USERGROUP_PERMISSION_GROUP_SELECTION
} UsergroupPermissionSelection;

UsergroupPermissionSelection usergroup_permission_selection = USERGROUP_PERMISSION_USER_SELECTION;

static GStaticMutex   usergroup_permission_mutex = G_STATIC_MUTEX_INIT;


GtkTreeStore * usergroup_permission_user_store_get (void)
{
	return usergroup_permission_user_store;
}

GtkTreeStore * usergroup_permission_group_store_get (void)
{
	return usergroup_permission_group_store;
}

GtkTreeStore * usergroup_permission_list_store_get (void)
{
	return usergroup_permission_list_store;
}



void usergroup_permission_group_activate ()
{
	usergroup_permission_selection = USERGROUP_PERMISSION_GROUP_SELECTION;

	usergroup_permission_refresh ();

	return;
}

void usergroup_permission_user_activate (GtkMenuItem * menu_item)
{
	usergroup_permission_selection = USERGROUP_PERMISSION_USER_SELECTION;

	usergroup_permission_refresh ();

	return;
}

/*
 * Callbacks called when switching between tabs in the component.
 * 
 */


void usergroup_permission_enable_selection_actions ()
{
/* 	GtkWidget * widget; */

/* 	if (usergroup_get_active_notebook_tab() == USERGROUP_PERMISSION_TAB) { */

/* 		widget = usergroup_menubar_get_widget ("refresh1"); */
/* 		widgets_set_sensitive ("user_permission", widget, TRUE); */

/* 		widget = usergroup_toolbar_get_widget ("refresh_button"); */
/* 		widgets_set_sensitive ("user_permission", widget, TRUE); */
/* 	} */

	return;
}

void usergroup_permission_disable_selection_actions ()
{
	GtkWidget     * widget;
	GtkTextView   * textview;
	GtkTextBuffer * txtbuf = NULL;

	widget = usergroup_menubar_get_widget ("edit_user1");
	widgets_set_sensitive ("user", widget, FALSE);

	widget = usergroup_menubar_get_widget ("remove_user1");
	widgets_set_sensitive ("user", widget, FALSE);

	textview =  GTK_TEXT_VIEW (usergroup_get_widget ("permission_description_textview"));

	txtbuf = gtk_text_view_get_buffer (textview);
	gtk_text_buffer_set_text (txtbuf, "", -1);
		

	return;
}

void usergroup_permission_update_selection_actions ()
{
	GtkWidget * widget = usergroup_toolbar_get_widget ("refresh_button");

	widgets_set_sensitive ("user_permission", widget, TRUE);

	widget = usergroup_menubar_get_widget ("refresh1"); 
	widgets_set_sensitive ("user_permission", widget, TRUE); 

	usergroup_permission_disable_selection_actions ();
	return;
}



/*
 * Function that returns the selected user's id 
 */

gint usergroup_permission_get_id_selected (void)
{
	GtkTreeSelection * selection;
	GtkTreeIter        iter;
	gint               id = -1;
	GtkTreeView      * treeview;

	treeview = GTK_TREE_VIEW (usergroup_get_widget ("permission_treeview"));	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_permission_list_store),
				    &iter, USERGROUP_PERMISSION_ID_COLUMN, &id, -1);
	}

	return id;
}


void usergroup_permission_cursor_changed (GtkTreeView *treeview, gpointer user_data)
{
	GtkTreeSelection      * selection;
	GtkTreeIter             iter;
	GtkTextView           * textview;
	AosKernelPermission * permission;
	GtkTextBuffer         * txtbuf = NULL;
	
	selection = gtk_tree_view_get_selection (treeview);

	textview =  GTK_TEXT_VIEW (usergroup_get_widget ("permission_description_textview"));

	txtbuf = gtk_text_view_get_buffer (textview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {

		usergroup_permission_enable_selection_actions ();
		permission = (AosKernelPermission *) 
			oseaclient_list_lookup (usergroup_permission_list, 
				       GINT_TO_POINTER(usergroup_permission_get_id_selected ()));

		gtk_text_buffer_set_text (txtbuf, permission->description, -1);
	} else
		gtk_text_buffer_set_text (txtbuf, "", -1);
	
	return;
}


// Functions for setting/unsetting permissions

void usergroup_permission_toggled_process (AsplDataList *datalist) 
{
	OseaClientNulData      * data         = osea_datalist_get (datalist, "DATA");
	StatusBarMessage  * statusbarmsg = NULL;
	gint                id           = GPOINTER_TO_INT (osea_datalist_get(datalist, "ID"));
	gint                found_id     = 0;
	gboolean            new_value    = GPOINTER_TO_INT (osea_datalist_get(datalist, "NEW_VALUE"));
	gboolean            valid        = TRUE;
	gboolean            found        = FALSE;
	gboolean            user, group, effective, only_group;
	GtkTreeIter         iter;

	statusbarmsg = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");	
	main_window_remove_statusbar_message (statusbarmsg);	
	main_window_set_progressbar (FALSE);

	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while setting permission:\n%s"),
				  data->text_response);
		oseaclient_nul_free (data);
		return;
	}


	// Now we must refresh the value of model with the new value

	/* Get the first iter in the list */
	valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL(usergroup_permission_list_store), &iter);
	
	while (valid && !found) {
		/* Walk through the list, reading each row */
		gtk_tree_model_get (GTK_TREE_MODEL(usergroup_permission_list_store), &iter, 
				    USERGROUP_PERMISSION_ID_COLUMN, &found_id,
				    -1);
		
		if (found_id == id) {
			found = TRUE;
		} else 
			valid = gtk_tree_model_iter_next (GTK_TREE_MODEL(usergroup_permission_list_store), &iter);

	}

	if (found) {
		gtk_tree_store_set (usergroup_permission_list_store, &iter,
				    USERGROUP_PERMISSION_USER_COLUMN, new_value,
				    -1);

		user = new_value;

		gtk_tree_model_get (GTK_TREE_MODEL(usergroup_permission_list_store), &iter, 
				    USERGROUP_PERMISSION_GROUP_COLUMN, &group,
				    -1);	
		
		effective = user || group;
		
		only_group = effective && (! user);

		gtk_tree_store_set (usergroup_permission_list_store, &iter, 
				    USERGROUP_PERMISSION_EFFECTIVE_COLUMN, effective,
				    USERGROUP_PERMISSION_ONLY_GROUP_COLUMN, only_group,
				    -1);
	}
	oseaclient_nul_free (data);
	osea_datalist_free (datalist);

	return;
}


void usergroup_permission_toggled (GtkCellRendererToggle *cell,
				   gchar                 *path_str,
				   gpointer               data)
{
	GtkTreeModel      * model           = (GtkTreeModel *)data;
	GtkTreePath       * path            = gtk_tree_path_new_from_string (path_str);
	StatusBarMessage  * status_message;
	AsplDataList      * datalist;
	GtkTreeIter         iter;
	gboolean            result          = FALSE;
	gboolean            value;
	gboolean            new_value;
	gint                id = 0;
	gchar             * depends;


	if ((usergroup_permission_selection == USERGROUP_PERMISSION_USER_SELECTION) &&
	    (! oseaclient_session_get_permission ("os-kernel", "permission_user_set")))
		return;

	if ((usergroup_permission_selection == USERGROUP_PERMISSION_GROUP_SELECTION) &&
	    (! oseaclient_session_get_permission ("os-kernel", "permission_group_set")))
		return;


	// We get the iterator of the toggled cell
 	gtk_tree_model_get_iter (model, &iter, path);
      	gtk_tree_path_free (path);


	gtk_tree_model_get (model, &iter,
			    USERGROUP_PERMISSION_USER_COLUMN, &value,
			    USERGROUP_PERMISSION_ID_COLUMN,      &id,
			    USERGROUP_PERMISSION_DEPEND_COLUMN,  &depends,
			    -1);
	if (value)
		new_value = FALSE;
	else
		new_value = TRUE;

	// We must check that, if new_value is TRUE, this permission
	// doesn't depend on another permission not owned by the
	// selected user/group

	

	#warning FIX ME

	// If new_value is FALSE, we must check that other permissions
	// owned by the selected user/group don't depend on this permission

	#warning FIX ME TOO

	// We set the new value, so we must set it (with all dependencies)

	usergroup_permission_last_transaction_event = event_source_arm_signal ("usergroup_permission_transaction",
									       (GSourceFunc) usergroup_permission_toggled_process);

	main_window_set_progressbar (TRUE);
	status_message = main_window_set_statusbar_message (_("Setting permission in os-kernel server..."),0);

	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", usergroup_permission_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);
	osea_datalist_set (datalist, "NEW_VALUE", GINT_TO_POINTER((gint) new_value));
	osea_datalist_set (datalist, "ID", GINT_TO_POINTER (id));


	switch (usergroup_permission_selection) {
	case USERGROUP_PERMISSION_USER_SELECTION:
		result = aos_kernel_permission_user_set_value (id, usergroup_permission_userlist_get_id_selected(), new_value,
								 (OseaClientNulFunc) event_process_oseaclient_data,
								 datalist);
		break;
	case USERGROUP_PERMISSION_GROUP_SELECTION:
 		result = aos_kernel_permission_group_set_value (id, usergroup_permission_userlist_get_id_selected(), new_value, 
								  (OseaClientNulFunc) event_process_oseaclient_data, 
								  datalist); 
		break;
	}		

	if (! result) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while trying to set permissions"));
		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);
	}
	return;

}



/* 
 * Functions for refreshing the treeview (after pressing refresh button...)
 */

gboolean __usergroup_permission_add_to_model (gpointer key, gpointer node, gpointer usr_data)
{
	GtkTreeIter             iter;
	AosKernelPermission * permission;
	GtkTreeStore          * store = GTK_TREE_STORE (usr_data);

	permission = (AosKernelPermission *) node;

	gtk_tree_store_append (store, &iter, NULL);
	gtk_tree_store_set (store, &iter,
			    USERGROUP_PERMISSION_ID_COLUMN, permission->id,
			    USERGROUP_PERMISSION_SERVER_COLUMN, permission->server_name,
			    USERGROUP_PERMISSION_NAME_COLUMN, permission->name,
			    USERGROUP_PERMISSION_DESCRIPTION_COLUMN, permission->description,
			    USERGROUP_PERMISSION_USER_COLUMN, FALSE,
			    USERGROUP_PERMISSION_GROUP_COLUMN, FALSE,
			    USERGROUP_PERMISSION_ONLY_GROUP_COLUMN, FALSE,
			    USERGROUP_PERMISSION_EFFECTIVE_COLUMN, FALSE,
			    USERGROUP_PERMISSION_DEPEND_COLUMN, permission->depends,
			    -1);


	return FALSE;
}


void usergroup_permission_refresh_process (AsplDataList *datalist)
{
	OseaClientData         * data = osea_datalist_get (datalist, "DATA");
	GtkTreeStore      * new_store = NULL, *old_store = NULL;
	GtkCellRenderer   * renderer;
	GtkTreeView       * view = NULL;
	GtkTreeViewColumn * column = NULL;
	StatusBarMessage  * statusbarmsg = NULL;
	OseaClientList             * old_tree = NULL;

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

	usergroup_permission_disable_selection_actions ();

	// Create a gtk-tree-store model
	new_store = gtk_tree_store_new (USERGROUP_PERMISSION_COLUMN_NUMBER,
					G_TYPE_INT,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_BOOLEAN,
					G_TYPE_BOOLEAN,
					G_TYPE_BOOLEAN,
					G_TYPE_BOOLEAN,
					G_TYPE_STRING);
	
	// Fill gtk-tree-store-model with our data
	oseaclient_list_foreach (data->data, __usergroup_permission_add_to_model, new_store); 

	view = GTK_TREE_VIEW (usergroup_get_widget ("permission_treeview"));
	
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));
		
	if (! usergroup_permission_list_store) {
		gtk_tree_view_set_headers_clickable (view, TRUE);

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes (_("Server"),
								   renderer,
								   "text", USERGROUP_PERMISSION_SERVER_COLUMN,
								   NULL);
		gtk_tree_view_append_column (view, column);

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes (_("Name"),
								   renderer,
								   "text", USERGROUP_PERMISSION_NAME_COLUMN,
								   NULL);
		gtk_tree_view_append_column (view, column);
		
		
		gtk_tree_view_set_search_column (view, USERGROUP_PERMISSION_NAME_COLUMN);
	}

	old_store = usergroup_permission_list_store;
	usergroup_permission_list_store = new_store;

	if (old_store) {
		g_object_unref (old_store);
	}

	old_tree = usergroup_permission_list;
	usergroup_permission_list = data->data;

	if (old_tree) {
		oseaclient_list_destroy (old_tree);
	}

	oseaclient_data_free (data, FALSE);
	osea_datalist_free (datalist);

	// Now we refresh the user/group view

	usergroup_permission_userlist_refresh (TRUE);

	return;
}



void usergroup_permission_refresh (void)
{
	StatusBarMessage * status_message;
	AsplDataList     * datalist;
	

	g_return_if_fail (oseaclient_session_server_exists ("os-kernel"));

	switch (usergroup_permission_selection) {
	case USERGROUP_PERMISSION_USER_SELECTION:
		if (! usergroup_user_list_store_get () && (oseaclient_session_get_permission("os-kernel", "list_user"))) 
			usergroup_user_refresh ();
		break;
	case USERGROUP_PERMISSION_GROUP_SELECTION:
 		if (! usergroup_group_list_store_get () && (oseaclient_session_get_permission("os-kernel", "list_group")))  
			usergroup_group_refresh ();
		break;
	}
	
	// Ask for data to our server
	usergroup_permission_last_transaction_event = event_source_arm_signal ("usergroup_permission_transaction",
									       (GSourceFunc) usergroup_permission_refresh_process);
		
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Asking os-kernel server for exiting permission..."),0);

	datalist = osea_datalist_new ();
	osea_datalist_set_full (datalist, "SIGNAL", usergroup_permission_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);
	
	if (! aos_kernel_permission_list (0, 0, event_process_oseaclient_data, datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while retrieving permission from kernel"));
		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);
	}
	return;
}


// Functions about the user/group list in this permissions tab

gint usergroup_permission_userlist_get_id_selected (void)
{
	GtkTreeSelection * selection;
	GtkTreeIter        iter;
	gint               id = -1;
	GtkTreeView      * treeview;

	treeview = GTK_TREE_VIEW (usergroup_get_widget ("permissions_item_treeview"));	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		switch (usergroup_permission_selection) {
		case USERGROUP_PERMISSION_USER_SELECTION:
			gtk_tree_model_get (GTK_TREE_MODEL (usergroup_permission_user_store), &iter, 
					    USERGROUP_USER_ID_COLUMN, &id, 
					    -1);
			break;
		case USERGROUP_PERMISSION_GROUP_SELECTION:
 			gtk_tree_model_get (GTK_TREE_MODEL (usergroup_permission_group_store), &iter, 
					    USERGROUP_GROUP_ID_COLUMN, &id, 
					    -1); 
			break;
		}
	}
	return id;
}


gboolean __usergroup_permission_update_permissions_enabled (GtkTreeModel *model,
							    GtkTreePath *path,
							    GtkTreeIter *iter,
							    gpointer data)
{
	AsplDataList          * datalist = (AsplDataList *) data;
	OseaClientData             * oseaclientdata = osea_datalist_get (datalist, "DATA");	
	OseaClientList                 * permissions = (AfDalList *) oseaclientdata->data;
	AosKernelPermission * permission = NULL;
	gint                    id;
	gint                    column = GPOINTER_TO_INT(osea_datalist_get (datalist, "COLUMN"));
	gboolean                user, group, effective, only_group;

	gtk_tree_model_get (model, iter, USERGROUP_PERMISSION_ID_COLUMN, &id, -1);

	permission = oseaclient_list_lookup (permissions, GINT_TO_POINTER(id));

	if (permission)
		gtk_tree_store_set (GTK_TREE_STORE(model), iter, column, TRUE, -1);
	else
		gtk_tree_store_set (GTK_TREE_STORE(model), iter, column, FALSE, -1);

	gtk_tree_model_get (model, iter, 
			    USERGROUP_PERMISSION_USER_COLUMN, &user,
			    USERGROUP_PERMISSION_GROUP_COLUMN, &group,
			    -1);	
	
	effective = user || group;

	only_group = effective && (! user);

	gtk_tree_store_set (GTK_TREE_STORE(model), iter, 
			    USERGROUP_PERMISSION_ONLY_GROUP_COLUMN, only_group,
			    USERGROUP_PERMISSION_EFFECTIVE_COLUMN, effective,
			    -1);

	return FALSE;
}

void usergroup_permission_get_permissions_process (AsplDataList *datalist)
{
	OseaClientData         * data = osea_datalist_get (datalist, "DATA");
	GtkTreeStore      * store = usergroup_permission_list_store;
	GtkCellRenderer   * renderer;
	GtkTreeView       * view = NULL;
	GtkTreeViewColumn * column = NULL;
	StatusBarMessage  * statusbarmsg = NULL;
	GList             * list = NULL;

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

	g_static_mutex_lock (&usergroup_permission_mutex);
	// We must modify the current store for showing the actual permissions
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), __usergroup_permission_update_permissions_enabled, datalist);

	g_static_mutex_unlock (&usergroup_permission_mutex);
	

	// Now we show the actual permissions
	view = GTK_TREE_VIEW (usergroup_get_widget ("permission_treeview"));

	list = gtk_tree_view_get_columns (view);
	
	if (g_list_length (list) <=2) {

		renderer = gtk_cell_renderer_toggle_new ();
		g_signal_connect (G_OBJECT (renderer), "toggled",
				  G_CALLBACK (usergroup_permission_toggled), store);

		switch (usergroup_permission_selection) {
		case USERGROUP_PERMISSION_USER_SELECTION:
			column = gtk_tree_view_column_new_with_attributes (_("Enabled"),
									   renderer,
									   "active", USERGROUP_PERMISSION_EFFECTIVE_COLUMN,
									   "radio", USERGROUP_PERMISSION_ONLY_GROUP_COLUMN,
									   NULL);
			break;
		case USERGROUP_PERMISSION_GROUP_SELECTION:
			column = gtk_tree_view_column_new_with_attributes (_("Enabled"),
									   renderer,
									   "active", USERGROUP_PERMISSION_USER_COLUMN,
									   NULL);
			break;
		}

		gtk_tree_view_column_set_sizing (column,  GTK_TREE_VIEW_COLUMN_FIXED);
		gtk_tree_view_column_set_fixed_width (column, 40);

		gtk_tree_view_append_column (view, column);
	}

	g_list_free (list);

	oseaclient_data_free (data, FALSE);
	osea_datalist_free (datalist);

	return;
}

void usergroup_permission_get_permissions (gint id)
{
	StatusBarMessage   * status_message;
	AsplDataList       * datalist;

	usergroup_permission_last_transaction_event = event_source_arm_signal ("usergroup_permission_transaction",
									       (GSourceFunc) usergroup_permission_get_permissions_process);
	
	main_window_set_progressbar (TRUE);
	status_message = main_window_set_statusbar_message (_("Asking os-kernel server for effective permissions..."),0);

	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", usergroup_permission_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);	

	switch (usergroup_permission_selection) {
	case USERGROUP_PERMISSION_USER_SELECTION:
		osea_datalist_set (datalist, "COLUMN", GINT_TO_POINTER(USERGROUP_PERMISSION_USER_COLUMN));
		
		if (! aos_kernel_permission_actual_list_by_user (0, 0, id, event_process_oseaclient_data,
								   datalist)) {
			dialog_close_run 
				(main_window_get (), GTK_MESSAGE_ERROR,
				 _("There was a problem while getting permission by user"));
			
			main_window_remove_statusbar_message (status_message);
			main_window_set_progressbar (FALSE);
		}				

			

		usergroup_permission_last_transaction_event = event_source_arm_signal ("usergroup_permission_transaction",
										       (GSourceFunc) usergroup_permission_get_permissions_process);
		
		// main_window_set_progressbar (TRUE);
		status_message = main_window_set_statusbar_message (_("Asking os-kernel server for actual permissions..."),0);
		
		datalist = osea_datalist_new ();
		
		osea_datalist_set_full (datalist, "SIGNAL", usergroup_permission_last_transaction_event, g_free);
		osea_datalist_set (datalist, "MESSAGE", status_message);
		
		osea_datalist_set (datalist, "COLUMN", GINT_TO_POINTER(USERGROUP_PERMISSION_GROUP_COLUMN));

		if (! aos_kernel_permission_from_group_list_by_user (0, 0, id, event_process_oseaclient_data,
							    datalist)) {
			dialog_close_run 
				(main_window_get (), GTK_MESSAGE_ERROR,
				 _("There was a problem while getting permission by user"));
			
			main_window_remove_statusbar_message (status_message);
			main_window_set_progressbar (FALSE);
		}		

		break;
	case USERGROUP_PERMISSION_GROUP_SELECTION:
		osea_datalist_set (datalist, "COLUMN", GINT_TO_POINTER(USERGROUP_PERMISSION_USER_COLUMN));

		if (! aos_kernel_permission_list_by_group (0, 0, id, event_process_oseaclient_data,
							     datalist)) {
			dialog_close_run 
				(main_window_get (), GTK_MESSAGE_ERROR,
				 _("There was a problem while getting permission by group"));
			
			main_window_remove_statusbar_message (status_message);
			main_window_set_progressbar (FALSE);
		}		
		break;
	}

	return;
}

void usergroup_permission_clean_permissions ()
{
	GtkTreeView       * view = NULL;
	GList             * list = NULL;
	
	view = GTK_TREE_VIEW (usergroup_get_widget ("permission_treeview"));

	list = gtk_tree_view_get_columns (view);

	if (GTK_IS_TREE_VIEW_COLUMN(g_list_nth_data (list, 2)))
		gtk_tree_view_remove_column (view, GTK_TREE_VIEW_COLUMN (g_list_nth_data (list, 2)));

	g_list_free (list);
	
}

void usergroup_permission_userlist_cursor_changed (GtkTreeView *treeview, gpointer user_data)
{
	GtkTreeSelection      * selection;
	GtkTreeIter             iter;

	selection = gtk_tree_view_get_selection (treeview);	

	usergroup_permission_clean_permissions();

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {

		usergroup_permission_get_permissions (usergroup_permission_userlist_get_id_selected ());		

	} 
	return;
}

void usergroup_permission_userlist_refresh (gboolean force)
{
	GtkTreeView       * view = NULL;
	GList             * list = NULL;
	GtkCellRenderer   * renderer;
	GtkTreeViewColumn * column = NULL;
	GtkTreeSelection  * selection = NULL;
	gint                i;	

	if (force 
	    || (usergroup_permission_user_store != usergroup_user_list_store_get())
	    || (usergroup_permission_user_store != usergroup_group_list_store_get())
		) {    

		view = GTK_TREE_VIEW (usergroup_get_widget ("permissions_item_treeview"));		

		selection = gtk_tree_view_get_selection (view);
		gtk_tree_selection_unselect_all (selection);

		usergroup_permission_clean_permissions ();

		switch (usergroup_permission_selection) {
		case USERGROUP_PERMISSION_USER_SELECTION:		
			usergroup_permission_user_store = usergroup_user_list_store_get();
			gtk_tree_view_set_model (view, GTK_TREE_MODEL(usergroup_permission_user_store));
			break;
		case USERGROUP_PERMISSION_GROUP_SELECTION:
			usergroup_permission_group_store = usergroup_group_list_store_get();
			gtk_tree_view_set_model (view, GTK_TREE_MODEL(usergroup_permission_group_store));
			break;
		}
		
		list = gtk_tree_view_get_columns (view);
		
		for (i = 0; i < g_list_length(list); i++) 
			if (GTK_IS_TREE_VIEW_COLUMN(g_list_nth_data (list, i)))
				gtk_tree_view_remove_column (view, GTK_TREE_VIEW_COLUMN (g_list_nth_data(list, i)));
		
		
		g_list_free (list);
		
		renderer = gtk_cell_renderer_text_new ();
		
		switch (usergroup_permission_selection) {
		case USERGROUP_PERMISSION_USER_SELECTION:
			column = gtk_tree_view_column_new_with_attributes (_("User nick"),
									   renderer,
									   "text", USERGROUP_USER_NICK_COLUMN,
									   NULL);
			break;
		case USERGROUP_PERMISSION_GROUP_SELECTION:
			column = gtk_tree_view_column_new_with_attributes (_("Group name"),
									   renderer,
									   "text", USERGROUP_GROUP_NAME_COLUMN,
									   NULL);
			break;
		}
		
		gtk_tree_view_append_column (view, column);				
		
	}
}



/**
 * usergroup_permission_free_data:
 * 
 * Frees all internal module data.
 **/
void           usergroup_permission_free_data                 (void)
{
	// Free gtk-list-store object
	if (usergroup_permission_list_store) {

		g_object_unref (usergroup_permission_list_store);

		usergroup_permission_list_store = NULL;
	}
	
	// Free Gtree recieved from liboseaclient level.
	if (usergroup_permission_list) {
		oseaclient_list_destroy (usergroup_permission_list);
		
		usergroup_permission_list = NULL;
	}

	return;
}
