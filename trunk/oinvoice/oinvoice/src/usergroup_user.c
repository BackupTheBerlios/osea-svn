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
#include "usergroup_user.h"
#include "main_window.h"
#include "login_window.h"
#include "event_source.h"
#include "event_process.h"
#include "dialog.h"
#include "transaction_progress_window.h"
#include "widgets.h"


gint           usergroup_user_transaction_number = 0;
gchar        * usergroup_user_last_transaction_event = NULL;

GtkTreeStore * usergroup_user_list_store = NULL;
OseaClientList        * usergroup_user_list = NULL;


GladeXML     * usergroup_user_window_xml = NULL;

typedef struct __UsergroupUserSignalMessage {
	gchar            * signal;
	StatusBarMessage * message;
} UsergroupUserSignalMessage;

void usergroup_user_ok_clicked (GtkButton *button, gpointer user_data);

GtkTreeStore * usergroup_user_list_store_get (void)
{
	return usergroup_user_list_store;
}


/*
 * Callbacks called when switching between tabs in the component.
 * 
 */


void usergroup_user_enable_selection_actions ()
{
	GtkWidget * widget;

	if (usergroup_get_active_notebook_tab() == USERGROUP_USER_TAB) {

		widget = usergroup_menubar_get_widget ("edit_user1");
		widgets_set_sensitive ("user", widget, TRUE);
		
		widget = usergroup_menubar_get_widget ("remove_user1");
		widgets_set_sensitive ("user", widget, TRUE);
		
		widget = usergroup_toolbar_get_widget ("remove_button");
		widgets_set_sensitive ("user", widget, TRUE);
		
		widget = usergroup_toolbar_get_widget ("new_button");
		widgets_set_sensitive ("user", widget, TRUE);

	}

	return;
}

void usergroup_user_disable_selection_actions ()
{
	GtkWidget * widget;

//	if (usergroup_get_active_notebook_tab() == USERGROUP_USER_TAB) {

		widget = usergroup_menubar_get_widget ("edit_user1");
		widgets_set_sensitive ("user", widget, FALSE);
		
		widget = usergroup_menubar_get_widget ("remove_user1");
		widgets_set_sensitive ("user", widget, FALSE);
		
		widget = usergroup_toolbar_get_widget ("new_button");
		widgets_set_sensitive ("user", widget, FALSE);
		
		widget = usergroup_toolbar_get_widget ("remove_button");
		widgets_set_sensitive ("user", widget, FALSE);

//	}

	return;
}

void usergroup_user_update_selection_actions ()
{
	GtkTreeView      * treeview;
	GtkTreeIter        iter;
	GtkTreeSelection * selection;
	GtkWidget        * widget;

	if (usergroup_get_active_notebook_tab() == USERGROUP_USER_TAB) {

		treeview = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));
		selection = gtk_tree_view_get_selection (treeview);
		
		if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
			usergroup_user_enable_selection_actions ();
		} else {
			usergroup_user_disable_selection_actions ();
		}
		
		widget = usergroup_toolbar_get_widget ("new_button");
		widgets_set_sensitive ("user", widget, TRUE);

		widget = usergroup_menubar_get_widget ("refresh1");
		widgets_set_sensitive ("user", widget, TRUE);

		widget = usergroup_toolbar_get_widget ("refresh_button");
		widgets_set_sensitive ("user", widget, TRUE);
	}

	return;
}


gboolean __usergroup_user_add_to_string (gpointer key, gpointer node, gpointer usr_data)
{
	AosKernelUser * user;
	GString         * string = (GString *) usr_data;

	user = (AosKernelUser *) node;

	if (string->len != 0)
		g_string_append_printf (string, ", ");
	
	g_string_append_printf (string, "%s", user->nick);

	return FALSE;
}


void usergroup_user_cursor_changed_process (AsplDataList *datalist)
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

	oseaclient_list_foreach (data->data, __usergroup_user_add_to_string, string);

	label =  GTK_LABEL (usergroup_get_widget ("usergroup_user_belong_label"));
	gtk_label_set_text (label, (const gchar *) string->str);
	g_string_free (string, TRUE);
	
	oseaclient_data_free (data, TRUE);
	osea_datalist_free (datalist);

	return;	
}

void usergroup_user_cursor_changed (GtkTreeView *treeview, gpointer user_data)
{
	GtkTreeSelection    * selection;
	GtkTreeIter           iter;
	GtkLabel            * label;
	AosKernelUser     * user;
	gint                  id;
	StatusBarMessage    * status_message;
	AsplDataList        * datalist;
	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {

		usergroup_user_enable_selection_actions ();
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_user_list_store),
				    &iter, USERGROUP_USER_ID_COLUMN, &id, -1);
		
		user = (AosKernelUser *) oseaclient_list_lookup (usergroup_user_list, GINT_TO_POINTER (id));

		label =  GTK_LABEL (usergroup_get_widget ("usergroup_user_nick_label"));
		gtk_label_set_text (label, (const gchar *) user->nick);

		label =  GTK_LABEL (usergroup_get_widget ("usergroup_user_belong_label"));
		gtk_label_set_text (label, "...");

		if (! oseaclient_session_get_permission ("os-kernel", "list_group"))
			return;

		usergroup_user_last_transaction_event = event_source_arm_signal ("user_transaction",  
										 (GSourceFunc) usergroup_user_cursor_changed_process);

		main_window_set_progressbar (TRUE);
		status_message = main_window_set_statusbar_message (_("Getting user's group..."),0);
		
		datalist = osea_datalist_new ();
		
		osea_datalist_set_full (datalist, "SIGNAL", usergroup_user_last_transaction_event, g_free);
		osea_datalist_set (datalist, "MESSAGE", status_message);

		if (! aos_kernel_user_list_group (0, 0, id, event_process_oseaclient_data, datalist)) {
			dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
					  _("There was a problem while getting user's groups"));
			main_window_remove_statusbar_message (status_message);
			main_window_set_progressbar (FALSE);
			
		}
	}
	
	return;
}


void usergroup_user_free_selection (void)
{
	GtkLabel    * label;
	GtkTreeView * treeview;

	treeview = GTK_TREE_VIEW (usergroup_get_widget ("usergroup_user_treeview"));
	gtk_tree_selection_unselect_all (gtk_tree_view_get_selection (treeview));

	label =  GTK_LABEL (usergroup_get_widget ("usergroup_user_nick_label"));
	gtk_label_set_text (label, "");

	label =  GTK_LABEL (usergroup_get_widget ("usergroup_user_belong_label"));
	gtk_label_set_text (label, "");

	usergroup_user_disable_selection_actions ();
	usergroup_user_update_selection_actions ();

	return;
}



/*
 * Function that returns the selected user's id 
 */

gint usergroup_user_get_id_selected (void)
{
	GtkTreeSelection * selection;
	GtkTreeIter        iter;
	gint               id = -1;
	GtkTreeView      * treeview;

	treeview = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_user_list_store),
				    &iter, USERGROUP_USER_ID_COLUMN, &id, -1);
	}

	return id;
}


/* 
 * Functions for refreshing the treeview (after pressing refresh button...)
 */

gboolean __usergroup_user_add_to_model (gpointer key, gpointer node, gpointer usr_data)
{
	GtkTreeIter           iter;
	AosKernelUser * user;
	GtkTreeStore        * store = GTK_TREE_STORE (usr_data);

	user = (AosKernelUser *) node;

	gtk_tree_store_append (store, &iter, NULL);
	gtk_tree_store_set (store, &iter,
                    USERGROUP_USER_ID_COLUMN, user->id,
                    USERGROUP_USER_NICK_COLUMN, g_strdup(user->nick),
                    USERGROUP_USER_DESCRIPTION_COLUMN, g_strdup(user->description),
                    -1);


	return FALSE;
}

void usergroup_user_refresh_process (AsplDataList *datalist)
{
	OseaClientData         * data = osea_datalist_get (datalist, "DATA");
	GtkTreeStore      * new_store = NULL, *old_store = NULL;
	GtkCellRenderer   * renderer;
	GtkTreeView       * view = NULL;
	GtkTreeViewColumn * column;
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

	// Create a gtk-tree-store model
	new_store = gtk_tree_store_new (USERGROUP_USER_COLUMN_NUMBER,
					G_TYPE_INT,
					G_TYPE_STRING,
					G_TYPE_STRING);
	
	// Fill gtk-tree-store-model with our data
	oseaclient_list_foreach (data->data, __usergroup_user_add_to_model, new_store); 

	view = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));
	
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));
		
	if (! usergroup_user_list_store) {
		gtk_tree_view_set_headers_clickable (view, TRUE);

		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes (_("Nick"),
								   renderer,
								   "text", USERGROUP_USER_NICK_COLUMN,
								   NULL);
		gtk_tree_view_append_column (view, column);
		
		renderer = gtk_cell_renderer_text_new ();
		column = gtk_tree_view_column_new_with_attributes (_("Description"),
								   renderer,
								   "text", USERGROUP_USER_DESCRIPTION_COLUMN,
								   NULL);
		gtk_tree_view_append_column (view, column);
		
		gtk_tree_view_set_search_column (view, USERGROUP_USER_NICK_COLUMN);
	}

	old_store = usergroup_user_list_store;
	usergroup_user_list_store = new_store;

	if (old_store) {
		g_object_unref (old_store);
	}

	old_tree = usergroup_user_list;
	usergroup_user_list = data->data;

	if (old_tree) {
		oseaclient_list_destroy (old_tree);
	}

	oseaclient_data_free (data, FALSE);
	osea_datalist_free (datalist);

	usergroup_user_free_selection ();

	usergroup_user_update_selection_actions ();


	return;
}



void usergroup_user_refresh (void)
{
	StatusBarMessage            * status_message;
	AsplDataList                       * datalist;

	g_return_if_fail (oseaclient_session_server_exists ("os-kernel"));
	
	// Ask for data to our server
		
	usergroup_user_last_transaction_event = event_source_arm_signal ("usergroup_user_transaction", (GSourceFunc) usergroup_user_refresh_process);
	
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Asking os-kernel server for users..."),0);

	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", usergroup_user_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);
	
	if (! aos_kernel_user_list (0, 0, event_process_oseaclient_data,
					   datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while refreshing \nfrom server 'os-kernel'"));

		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);	
	}
	return;
}


/*
 * Functions for removing users 
 *
 */


void usergroup_user_remove_process (AsplDataList *datalist)
{
	OseaClientNulData     * data = osea_datalist_get (datalist, "DATA");
	GtkTreeStore     * new_store = NULL;
	StatusBarMessage * statusbarmsg = NULL;
	GtkTreeView      * view = NULL;
	GtkTreeSelection * selection = NULL;
	GtkTreeIter        iter;
	gint               id;


	statusbarmsg = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");

	main_window_remove_statusbar_message (statusbarmsg);

	main_window_set_progressbar (FALSE);		

	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while removing user:\n%s"),
				  data->text_response);
		oseaclient_nul_free (data);
		return;
	}

	view = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));
	
	selection = gtk_tree_view_get_selection (view);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_user_list_store),
				    &iter, USERGROUP_USER_ID_COLUMN, &id, -1);

		oseaclient_list_remove (usergroup_user_list, GINT_TO_POINTER (id));

		new_store = gtk_tree_store_new (USERGROUP_USER_COLUMN_NUMBER,
						G_TYPE_INT,
						G_TYPE_STRING,
						G_TYPE_STRING,
						G_TYPE_STRING);
		
		oseaclient_list_foreach (usergroup_user_list, __usergroup_user_add_to_model, new_store); 

	
		gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));

		if (usergroup_user_list_store) {
			g_object_unref (usergroup_user_list_store);
		}
		
		usergroup_user_list_store = new_store;
	}

	usergroup_user_free_selection ();

	oseaclient_nul_free (data);
	osea_datalist_free (datalist);
	return;
}


void usergroup_user_remove (void)
{
	StatusBarMessage            * status_message;
	AsplDataList                       * datalist;
	GtkTreeSelection            * selection;
	GtkTreeView                 * treeview;
	AosKernelUser             * user;
	GtkTreeIter                   iter;
	gint                          id, response;


	treeview = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));
	
	selection = gtk_tree_view_get_selection (treeview);
	
	// Try to get an iter to the user
	if (!gtk_tree_selection_get_selected (selection, NULL, &iter)) 
		return;
	
	
	gtk_tree_model_get (GTK_TREE_MODEL (usergroup_user_list_store),
			    &iter, USERGROUP_USER_ID_COLUMN, &id, -1);
	
	user = (AosKernelUser *) oseaclient_list_lookup (usergroup_user_list, GINT_TO_POINTER (id));

	// Ask user if he wants to delete the user
	response = dialog_cancel_ok_run (main_window_get (), GTK_MESSAGE_WARNING,
					 _("Are you sure you want to delete '%s' user?\n"),
					 user->nick);

	if (response == GTK_RESPONSE_CANCEL)
		return;
	
	g_return_if_fail (oseaclient_session_server_exists ("os-kernel"));

	// Ask for data to our server
	
	usergroup_user_last_transaction_event = event_source_arm_signal ("user_transaction",  (GSourceFunc) usergroup_user_remove_process);
	
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Removing User..."),0);
	
	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", usergroup_user_last_transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);

	if (! aos_kernel_user_remove (id, event_process_oseaclient_nuldata, datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while removing User"));
		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);	
		
	}
	return;
}



/*
 * Functions for managing the user's properties window (for creating
 * and editing users).
 *
 */


void usergroup_user_window_show (gchar *nick, gchar *description, gboolean is_new)
{
	gchar         * xml_file = NULL;
	gchar         * title = NULL;
	GtkWidget     * widget = NULL;
	GtkWidget     * name_entry = NULL;
	GtkTextBuffer * txtbuf = NULL;

	g_return_if_fail (! usergroup_user_window_xml);

	xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "user", NULL );
		
	usergroup_user_window_xml = widgets_load (xml_file, "user_window", NULL);
		
	if (nick) {
		widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_nick_entry");
		gtk_entry_set_text (GTK_ENTRY(widget), nick);
	}

	if (description) {
		widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_description_textview");
		txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
		gtk_text_buffer_set_text (txtbuf, description, -1);
	}

	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window");

	gtk_window_set_transient_for (GTK_WINDOW(widget), main_window_get ());
	main_window_set_active_window (GTK_WINDOW(widget));

	if (nick) {
		title = g_strdup_printf (_("%s properties - ASPL Fact"), nick);
		gtk_window_set_title (GTK_WINDOW(widget), (const gchar *) title);
	}

	// set the focus
  	name_entry =  glade_xml_get_widget (usergroup_user_window_xml, "user_window_nick_entry"); 
	gtk_widget_grab_focus (name_entry);

	gtk_widget_show (widget);

	// Reconnect clicked-signal callback to update is_new param
	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_button_ok");
	g_signal_handlers_disconnect_matched (widget, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, 
					      usergroup_user_ok_clicked, NULL);
	g_signal_connect (widget, "clicked", (GCallback) usergroup_user_ok_clicked, GINT_TO_POINTER (is_new));

	return;

}

void usergroup_user_window_destroy ()
{
	GtkWidget  * user_window = NULL;

	user_window = glade_xml_get_widget (usergroup_user_window_xml, "user_window");

	g_object_unref (G_OBJECT (usergroup_user_window_xml));
	usergroup_user_window_xml = NULL;
	
	gtk_object_destroy (GTK_OBJECT (user_window));
	user_window = NULL;	
	
	main_window_unset_active_window ();

	return;
}




void usergroup_user_new (void)
{
	usergroup_user_free_selection ();
	usergroup_user_window_show (NULL, NULL, TRUE);
	return;
}

void usergroup_user_edit (void)
{
	GtkTreeSelection    * selection;
	GtkTreeView         * treeview;
	GtkWidget           * widget;
	AosKernelUser     * user;
	GtkTreeIter           iter;
	gint                  id;


	treeview = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));	
	selection = gtk_tree_view_get_selection (treeview);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_user_list_store),
				    &iter, USERGROUP_USER_ID_COLUMN, &id, -1);
		user = (AosKernelUser *) oseaclient_list_lookup (usergroup_user_list, GINT_TO_POINTER (id));

		usergroup_user_window_show (user->nick, user->description, FALSE);
	} 

	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_password_notice_label");

	g_object_set (G_OBJECT(widget), "visible", TRUE, NULL);

	

	return;
}






//*****************************************************************************************
// User editing window






void usergroup_user_window_name_entry_changed (GtkEntry *name_entry, gpointer user_data)
{
	const gchar * text;
	gchar       * title;//, * old_title;
	GtkWidget   * widget = NULL;

	text = gtk_entry_get_text (name_entry);
	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window");

//	old_title = (gchar *) gtk_window_get_title (GTK_WINDOW(widget));
	
	if (!text || !text[0])
		gtk_window_set_title (GTK_WINDOW(widget), (const gchar *) g_strdup ("User properties - ASPL Fact"));
	else {
		title = g_strdup_printf (_("%s properties - ASPL Fact"), text);
		gtk_window_set_title (GTK_WINDOW(widget), (const gchar *) title);
	}

//	g_free (old_title);
		
	return;
}


void usergroup_user_new_process (AsplDataList *datalist)
{
	OseaClientSimpleData     * data;
	GtkWindow           * user_window = NULL;
	GtkWidget           * widget = NULL;
	GtkTreeStore        * new_store = NULL;
	GtkTextBuffer       * txtbuf = NULL;
	GtkTreeView         * view = NULL;
	AosKernelUser     * user = NULL;
	GtkTextIter           iter1, iter2;


	data = osea_datalist_get (datalist, "DATA");

	transaction_progress_window_destroy ();
	user_window = GTK_WINDOW(glade_xml_get_widget (usergroup_user_window_xml, "user_window"));
	
	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (user_window, GTK_MESSAGE_ERROR,
				  _("There was a problem while creating user:\n%s"),
				  data->text_response);
		oseaclient_simple_free (data);
		return;
	}

	user = g_new0 (AosKernelUser, 1);
	user->id = data->id;

	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_nick_entry");
	user->nick = g_strdup (gtk_entry_get_text (GTK_ENTRY(widget)));

	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_description_textview");
	txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
	gtk_text_buffer_get_start_iter (txtbuf, &iter1);
	gtk_text_buffer_get_end_iter (txtbuf, &iter2);
	user->description = gtk_text_buffer_get_text (txtbuf, &iter1, &iter2, FALSE);	

	usergroup_user_window_destroy ();

	oseaclient_list_insert (usergroup_user_list, GINT_TO_POINTER (user->id), user);

	new_store = gtk_tree_store_new (USERGROUP_USER_COLUMN_NUMBER,
					G_TYPE_INT,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING);
	
	oseaclient_list_foreach (usergroup_user_list, __usergroup_user_add_to_model, new_store); 

	view = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));
	
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));

	if (usergroup_user_list_store) {
		g_object_unref (usergroup_user_list_store);
	}

	usergroup_user_list_store = new_store;

	oseaclient_simple_free (data);
	osea_datalist_free (datalist);

	return;
	
}

void usergroup_user_edit_process (AsplDataList *datalist)
{
	OseaClientNulData        * data;
	GtkWindow           * user_window = NULL;
	GtkWidget           * widget = NULL;
	GtkTreeStore        * new_store = NULL;
	GtkTextBuffer       * txtbuf = NULL;
	GtkTreeView         * view = NULL;
	GtkTreeSelection    * selection;
	AosKernelUser     * user = NULL, *old_user = NULL;
	GtkTextIter           iter1, iter2;
	GtkTreeIter           iter;
	gint                  id = -65535;

	data = osea_datalist_get (datalist, "DATA");

	view = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));	
	selection = gtk_tree_view_get_selection (view);

	if (gtk_tree_selection_get_selected (selection, NULL, &iter))
		gtk_tree_model_get (GTK_TREE_MODEL (usergroup_user_list_store),
				    &iter, USERGROUP_USER_ID_COLUMN, &id, -1);

	if (id == -65535) {
		oseaclient_nul_free (data);
		return;
	}

	transaction_progress_window_destroy ();
	user_window = GTK_WINDOW(glade_xml_get_widget (usergroup_user_window_xml, "user_window"));
	
	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (user_window, GTK_MESSAGE_ERROR,
				  _("There was a problem while editing user:\n%s"),
				  data->text_response);
		oseaclient_nul_free (data);
		return;
	}

	user = g_new0 (AosKernelUser, 1);
	user->id = id;

	old_user = oseaclient_list_lookup (usergroup_user_list, GINT_TO_POINTER (id));

	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_nick_entry");
	user->nick = g_strdup (gtk_entry_get_text (GTK_ENTRY(widget)));

	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_description_textview");
	txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
	gtk_text_buffer_get_start_iter (txtbuf, &iter1);
	gtk_text_buffer_get_end_iter (txtbuf, &iter2);
	user->description = gtk_text_buffer_get_text (txtbuf, &iter1, &iter2, FALSE);	

	usergroup_user_window_destroy ();

	oseaclient_list_replace (usergroup_user_list, GINT_TO_POINTER (user->id), user);

	new_store = gtk_tree_store_new (USERGROUP_USER_COLUMN_NUMBER,
					G_TYPE_INT,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING);
	
	oseaclient_list_foreach (usergroup_user_list, __usergroup_user_add_to_model, new_store); 

	view = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));
	
	gtk_tree_view_set_model (view, GTK_TREE_MODEL(new_store));

	if (usergroup_user_list_store) {
		g_object_unref (usergroup_user_list_store);
	}

	usergroup_user_list_store = new_store;

	oseaclient_nul_free (data);
	osea_datalist_free (datalist);
	return;

}


void usergroup_user_ok_clicked (GtkButton *button, gpointer user_data)
{
	GtkTreeSelection   * selection;
	GtkTreeView        * treeview;
	GtkWindow          * user_window = NULL;
	GtkWidget          * widget = NULL, * widget2 = NULL;
	GtkTextBuffer      * txtbuf = NULL;
	gchar              * nick = NULL, * passwd = NULL, * description = NULL;
	gboolean             is_new = GPOINTER_TO_INT (user_data);
	GtkTreeIter          iter;
	GtkTextIter          iter1, iter2;
	AsplDataList              * datalist = NULL;
	gint                 id;

	user_window = GTK_WINDOW(glade_xml_get_widget (usergroup_user_window_xml, "user_window"));

	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_name_entry");

	// Gets nick entry's content
	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_nick_entry");
	nick = g_strdup (gtk_entry_get_text (GTK_ENTRY(widget)));
	if (!nick || !strlen (nick)) {
		dialog_close_run (user_window, GTK_MESSAGE_ERROR,
				  _("You must provide a valid user nick"));

		return;
	}

	// Gets description entry's content
	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_description_textview");
	txtbuf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));


	gtk_text_buffer_get_start_iter (txtbuf, &iter1);
	gtk_text_buffer_get_end_iter (txtbuf, &iter2);
	description = gtk_text_buffer_get_text (txtbuf, &iter1, &iter2, FALSE);	

	// Check if passwords are null or if passwords are the same
	widget = glade_xml_get_widget (usergroup_user_window_xml, "user_window_password_entry");
	widget2 = glade_xml_get_widget (usergroup_user_window_xml, "user_window_password_check_entry");

	if (strcmp (gtk_entry_get_text (GTK_ENTRY(widget)), gtk_entry_get_text (GTK_ENTRY(widget2)))) {
		dialog_close_run (user_window, GTK_MESSAGE_ERROR,
				  _("Passwords differ. You must enter the same password in both entries."));
		return;
	}	       

	passwd = g_strdup (gtk_entry_get_text (GTK_ENTRY(widget)));
	

	if (!is_new) {
		// Get the user id to edit.
		treeview = GTK_TREE_VIEW (usergroup_get_widget ("user_treeview"));
		
		selection = gtk_tree_view_get_selection (treeview);

		if (gtk_tree_selection_get_selected (selection, NULL, &iter)) {
			
			gtk_tree_model_get (GTK_TREE_MODEL (usergroup_user_list_store),
					    &iter, USERGROUP_USER_ID_COLUMN, &id, -1);		
		}
	}	

	// If this is "new", we must enter a valid password

	if (is_new) {
		if (!passwd || !strlen (passwd)) {
		dialog_close_run (user_window, GTK_MESSAGE_ERROR,
				  _("You must provide a valid password"));

		return;
		}
	}


	g_return_if_fail (oseaclient_session_server_exists ("os-kernel"));
	

	// Start transaction
	usergroup_user_last_transaction_event = g_strdup_printf ("user_transaction %d", 
								 ++usergroup_user_transaction_number);


	datalist = osea_datalist_new ();
	osea_datalist_set_full (datalist, "SIGNAL", usergroup_user_last_transaction_event, g_free);
	
	event_source_add_signal (usergroup_user_last_transaction_event);
	
	if (is_new) {
		transaction_progress_window_show (GTK_WINDOW (user_window), 
						  _("Creating user..."), 
						  usergroup_user_last_transaction_event);
		event_source_set_callback (usergroup_user_last_transaction_event, 
					   (GSourceFunc) usergroup_user_new_process);

		if (! aos_kernel_user_new (nick, passwd, description, 
				      event_process_oseaclient_simpledata,
				      datalist)) {
			dialog_close_run 
				(main_window_get (), GTK_MESSAGE_ERROR,
				 _("There was a problem while refreshing \nfrom server 'os-kernel'"));
			transaction_progress_window_destroy ();
		}
		
	} else {
		if (!passwd || !strlen (passwd)) {
			g_free (passwd);
			passwd = NULL;
		}
			
		transaction_progress_window_show (GTK_WINDOW (user_window), 
						  _("Updating user..."), 
						  usergroup_user_last_transaction_event);
		event_source_set_callback (usergroup_user_last_transaction_event, 
					   (GSourceFunc) usergroup_user_edit_process);
		if (! aos_kernel_user_edit (id, nick, passwd, description, 
					      event_process_oseaclient_nuldata,
					      datalist)) {
			dialog_close_run 
				(main_window_get (), GTK_MESSAGE_ERROR,
				 _("There was a problem while refreshing \nfrom server 'os-kernel'"));
			transaction_progress_window_destroy ();
		}
		
	}
	
	g_free (nick);
	g_free (passwd);
	g_free (description);
	
	return;
}

gboolean usergroup_user_window_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	usergroup_user_window_destroy ();
	return FALSE;
}

/**
 * usergroup_user_free_data:
 * 
 * Frees all internal module data.
 **/
void           usergroup_user_free_data                 (void)
{
	// Free gtk-list-store object
	if (usergroup_user_list_store) {

		g_object_unref (usergroup_user_list_store);

		usergroup_user_list_store = NULL;
	}
	
	// Free Gtree recieved from liboseaclient level.
	if (usergroup_user_list) {
		oseaclient_list_destroy (usergroup_user_list);
		
		usergroup_user_list = NULL;
	}

	return;
}
