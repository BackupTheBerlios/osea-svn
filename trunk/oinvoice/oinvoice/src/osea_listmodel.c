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

#include <gtk/gtktreemodel.h>
#include <gtk/gtktreesortable.h>
#include <liboseaclient/oseaclient.h>
#include "osea_listmodel.h"
#include "osea_datalist.h"
#include "event_source.h"
#include "event_process.h"
#include "dialog.h"
#include "main_window.h"
#include "support.h"


struct _AsplListmodelPrivate {
	AsplListmodelConfig  * cfg;          /* Model configuration (services for refreshing the model, and removing elements from it) */
	GType                  object_type;

	OseaClientList            * list;         /* Real datalist */		
	gint                   stamp;
};

gint           osea_listmodel_transaction_number = 0;
gchar        * osea_listmodel_last_transaction_event = NULL;


static void    osea_listmodel_instance_init    (AsplListmodel *list_model);
static void    osea_listmodel_tree_model_init  (GtkTreeModelIface *iface);


/* Auxiliar functions for methods */

void           osea_listmodel_refresh_process (AsplDataList *datalist);
void           osea_listmodel_remove_process (AsplDataList *datalist);


/* Definition functions for TreeModel interface */

static GtkTreeModelFlags osea_listmodel_get_flags       (GtkTreeModel      *tree_model);
static gint              osea_listmodel_get_n_columns   (GtkTreeModel      *tree_model);
static GType             osea_listmodel_get_column_type (GtkTreeModel      *tree_model,
							 gint               index);
static gboolean          osea_listmodel_get_iter        (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter,
							 GtkTreePath       *path);
static GtkTreePath     * osea_listmodel_get_path        (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter);
static void              osea_listmodel_get_value       (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter,
							 gint               column,
							 GValue            *value);
static gboolean          osea_listmodel_iter_next       (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter);
static gboolean          osea_listmodel_iter_children   (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter,
							 GtkTreeIter       *parent);
static gboolean          osea_listmodel_iter_has_child  (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter);
static gint              osea_listmodel_iter_n_children (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter);
static gboolean          osea_listmodel_iter_nth_child  (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter,
							 GtkTreeIter       *parent,
							 gint               n);
static gboolean          osea_listmodel_iter_parent     (GtkTreeModel      *tree_model,
							 GtkTreeIter       *iter,
							 GtkTreeIter       *child);



/* ********************************************************************* */
/*                         GObject functions                             */
/* ********************************************************************* */


GType osea_listmodel_get_type (void)
{
        static GType type = 0;
	
        if (type == 0) {
                static const GTypeInfo info = {
                        sizeof (AsplListmodelClass),
                        NULL,   /* base_init */
                        NULL,   /* base_finalize */
                        NULL,   /* class_init */
                        NULL,   /* class_finalize */
                        NULL,   /* class_data */
                        sizeof (AsplListmodel),
                        0,      /* n_preallocs */
                        (GInstanceInitFunc) osea_listmodel_instance_init /* instance_init */
                };
                static const GInterfaceInfo tree_model_info = {
                        (GInterfaceInitFunc) osea_listmodel_tree_model_init, /* interface_init */
                        NULL,                                       /* interface_finalize */
                        NULL                                        /* interface_data */
                };
                type = g_type_register_static (G_TYPE_OBJECT,
                                               "AsplListmodelType",
                                               &info, 0);
                g_type_add_interface_static (type,
                                             GTK_TYPE_TREE_MODEL,
                                             &tree_model_info);
        }
        return type;
}


static void        osea_listmodel_instance_init (AsplListmodel *list_model)
{
	/* Object initialization function (constructor) */
	list_model->private = g_new0 (AsplListmodelPrivate, 1);
	list_model->private->stamp = g_random_int ();
}


static void        osea_listmodel_tree_model_init (GtkTreeModelIface *iface)
{
	iface->get_flags = osea_listmodel_get_flags;
	iface->get_n_columns = osea_listmodel_get_n_columns;
	iface->get_column_type = osea_listmodel_get_column_type;
	iface->get_iter = osea_listmodel_get_iter;
	iface->get_path = osea_listmodel_get_path;
	iface->get_value = osea_listmodel_get_value;
	iface->iter_next = osea_listmodel_iter_next;
	iface->iter_children = osea_listmodel_iter_children;
	iface->iter_has_child = osea_listmodel_iter_has_child;
	iface->iter_n_children = osea_listmodel_iter_n_children;
	iface->iter_nth_child = osea_listmodel_iter_nth_child;
	iface->iter_parent = osea_listmodel_iter_parent;
}


/* ************************************************************************** */
/*                         Configuration functions                            */
/* ************************************************************************** */
AsplListmodelConfig * osea_listmodel_config_new (gchar                         * server_name,
						 AsplListmodelGetDataFunction    refresh_service,
						 AsplListmodelStartFunc          start_refresh_callback,
						 gpointer                        start_refresh_user_data,
						 AsplListmodelEndFunc            end_refresh_callback,
						 gpointer                        end_refresh_user_data,
						 AsplListmodelRemoveFunction     remove_service,
						 gchar                         * remove_confirmation_question,
						 AsplListmodelStartFunc          start_remove_callback,
						 gpointer                        start_remove_user_data,
						 AsplListmodelEndFunc            end_remove_callback,
						 gpointer                        end_remove_user_data)
{
	AsplListmodelConfig * cfg = g_new0 (AsplListmodelConfig, 1);

	cfg->server_name = server_name;

	cfg->refresh_service = refresh_service;
	cfg->start_refresh_callback = start_refresh_callback;
	cfg->start_refresh_user_data = start_refresh_user_data;
	cfg->end_refresh_callback = end_refresh_callback;
	cfg->end_refresh_user_data = end_refresh_user_data;

	cfg->remove_service = remove_service;
	cfg->remove_confirmation_question = remove_confirmation_question;
	cfg->start_remove_callback = start_remove_callback;
	cfg->start_remove_user_data = start_remove_user_data;
	cfg->end_remove_callback = end_remove_callback;
	cfg->end_remove_user_data = end_remove_user_data;

	return cfg;
}
	






/* ************************************************************************** */
/*                                  Methods                                   */
/* ************************************************************************** */

gint _osea_listmodel_get_node_id (OseaClientDataNode * node)
{
	GValue   value_int  = {0, };

	g_return_val_if_fail (node, -1);
	g_return_val_if_fail (OSEACLIENT_IS_DATANODE (node), -1);
	g_return_val_if_fail (g_value_init (&value_int, G_TYPE_INT), -1);
	
	g_object_get_property (G_OBJECT(node), "id", &value_int);

	return g_value_get_int (&value_int);
}


AsplListmodel * osea_listmodel_new      (AsplListmodelConfig * cfg, GType object_type)
{
	AsplListmodel * result;
	
	result = g_object_new (OSEA_LISTMODEL_TYPE, NULL);
	
	result->private->cfg = cfg;
	result->private->object_type = object_type;
	
	osea_listmodel_refresh (result);

	return result;
}


void            osea_listmodel_refresh  (AsplListmodel *model)
{
	AsplDataList                 * datalist;
	gpointer                       status_data = NULL;
	AsplListmodelStartFunc         start;
	AsplListmodelGetDataFunction   refresh;
	AsplListmodelEndFunc           end;


	g_return_if_fail (model);
	g_return_if_fail (oseaclient_session_server_exists (model->private->cfg->server_name));
	g_return_if_fail (model->private->cfg->refresh_service);

	// Ask for data to our server
	refresh = model->private->cfg->refresh_service;
	end     = model->private->cfg->end_refresh_callback;
	start   = model->private->cfg->start_refresh_callback;
	if (start)
		status_data = start (model->private->cfg->start_refresh_user_data);
	
	datalist = support_create_callback (osea_listmodel_refresh_process);
	osea_datalist_set (datalist, "LISTMODEL", model);
	osea_datalist_set (datalist, "START_REFRESH_DATA", status_data);
	
	if (! refresh (0, 0, event_process_oseaclient_data, datalist)) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR, 
				  _("There was a problem while refreshing \nfrom server '%s'"), 
				  model->private->cfg->server_name);
		if (end)
			end (status_data, model->private->cfg->end_refresh_user_data);
	}
	return;	
}

void            osea_listmodel_remove  (AsplListmodel *model, GtkTreeIter * iter)
{
	AsplDataList                * datalist;
	gpointer                      status_data = NULL;
	AsplListmodelStartFunc        remove_cb;
	OseaClientDataNode               * node;
	gint                          id, response;
	GtkTreeIter                 * iter_copy;

	g_return_if_fail (model);
	g_return_if_fail (model->private->stamp == iter->stamp);
	g_return_if_fail (OSEACLIENT_IS_DATANODE (iter->user_data));
	g_return_if_fail (oseaclient_session_server_exists (model->private->cfg->server_name));
	g_return_if_fail (model->private->cfg->remove_service);

	if (model->private->cfg->remove_confirmation_question) {
		// Ask user if he wants to delete the item
		response = dialog_cancel_ok_run (main_window_get (), GTK_MESSAGE_WARNING,
						 model->private->cfg->remove_confirmation_question);
		
		if (response == GTK_RESPONSE_CANCEL)
			return;
	}
	
	// Ask for data to our server
	remove_cb = model->private->cfg->start_remove_callback;
	if (remove_cb)
		status_data = remove_cb (model->private->cfg->start_refresh_user_data);
	
	
	datalist = support_create_callback (osea_listmodel_remove_process);	
	iter_copy = osea_listmodel_copy_iter (iter);
	osea_datalist_set (datalist, "ITER", iter_copy);
	osea_datalist_set (datalist, "LISTMODEL", model);
	osea_datalist_set (datalist, "START_REFRESH_DATA", status_data);
	
	node = OSEACLIENT_DATANODE (iter->user_data);
	id = _osea_listmodel_get_node_id (node);
	osea_datalist_set (datalist, "ID", GINT_TO_POINTER (id));
	if (! model->private->cfg->remove_service (id ,(OseaClientNulFunc) event_process_oseaclient_data, datalist)) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR, 
				  _("There was a problem while removing element \nfrom server '%s'"), 
				  model->private->cfg->server_name);

		if (model->private->cfg->end_remove_callback) 
			model->private->cfg->end_remove_callback (status_data, model->private->cfg->end_refresh_user_data);
	}
	
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "REMOVE: element id: %d", id);
	return;
}


OseaClientDataNode * osea_listmodel_get_node (AsplListmodel *model, GtkTreeIter * iter)
{

	g_return_val_if_fail (ASPL_IS_LISTMODEL (model), NULL);
	g_return_val_if_fail (OSEA_LISTMODEL (model)->private->stamp == iter->stamp, NULL);
	g_return_val_if_fail (OSEACLIENT_IS_DATANODE (iter->user_data), NULL);
	
	return OSEACLIENT_DATANODE (iter->user_data);
}

void            osea_listmodel_insert_node (AsplListmodel *model, OseaClientDataNode *node)
{
	GtkTreePath * path;
	GtkTreeIter   iter;
	gint          id;

	g_return_if_fail (ASPL_IS_LISTMODEL (model));
	g_return_if_fail (OSEACLIENT_IS_DATANODE (node));

	id = _osea_listmodel_get_node_id (node);
	oseaclient_list_insert (model->private->list, GINT_TO_POINTER (id), node);

	iter.stamp     = model->private->stamp;
	iter.user_data = node;

	path = osea_listmodel_get_path (GTK_TREE_MODEL (model), &iter);
	gtk_tree_model_row_inserted (GTK_TREE_MODEL(model), path, &iter);	
	return;
}

void            osea_listmodel_edit_node   (AsplListmodel *model, GtkTreeIter *iter, OseaClientDataNode *node)
{

	GtkTreePath * path;
	gint          id;

	g_return_if_fail (ASPL_IS_LISTMODEL (model));
	g_return_if_fail (OSEACLIENT_IS_DATANODE (node));
	g_return_if_fail (model->private->stamp == iter->stamp);
	g_return_if_fail (node == OSEACLIENT_DATANODE (iter->user_data));
	
	id = _osea_listmodel_get_node_id (node);
	oseaclient_list_replace (model->private->list, GINT_TO_POINTER (id), node);
	
	path = osea_listmodel_get_path (GTK_TREE_MODEL (model), iter);
	gtk_tree_model_row_changed (GTK_TREE_MODEL(model), path, iter);
	return;

}

gint            osea_listmodel_get_id      (AsplListmodel *model, GtkTreeIter *iter)
{
	g_return_val_if_fail (ASPL_IS_LISTMODEL (model), -1);
	g_return_val_if_fail (iter, -1);
	g_return_val_if_fail (model->private->stamp == iter->stamp, -1);
	g_return_val_if_fail (OSEACLIENT_IS_DATANODE (iter->user_data), -1);
	
	return _osea_listmodel_get_node_id (OSEACLIENT_DATANODE (iter->user_data));
}

gint            osea_listmodel_get_column  (AsplListmodel *model, const gchar * column_name)
{
	GParamSpec    ** paramspecs = NULL;
	gint             param_number = 0;
	GObject        * object;
	GObjectClass   * object_class;
	gint             i;
	
	g_return_val_if_fail (ASPL_IS_LISTMODEL (model), -1);
	g_return_val_if_fail (model->private->object_type, -1);
	
	object       = g_object_new (model->private->object_type, NULL);
	object_class = G_OBJECT_CLASS (OSEACLIENT_DATANODE_GET_CLASS(object));
	paramspecs   = g_object_class_list_properties (object_class, &param_number);
	g_object_unref (object);

	for (i = 0; i < param_number; i++) {
		if (! g_strcasecmp (column_name, paramspecs[i]->name))
			return i;
	}
	return -1;
}



/* ********************************************************************* */
/*                     Auxiliar functions for methods                    */
/* ********************************************************************* */

void           osea_listmodel_refresh_process (AsplDataList *datalist)
{
	OseaClientData         * data        = osea_datalist_get (datalist, "DATA");
	AsplListmodel     * model       = osea_datalist_get (datalist, "LISTMODEL");
	gpointer            status_data = osea_datalist_get (datalist, "START_REFRESH_DATA");
	OseaClientList         * old_list    = NULL;
	OseaClientDataNode     * node        = NULL;
	GtkTreeIter         iter;
	GtkTreePath       * path        = NULL;
						       


	if (model->private->cfg->end_refresh_callback)
		model->private->cfg->end_refresh_callback (status_data, model->private->cfg->end_refresh_user_data);
	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while refreshing data:\n%s"),
				  data->text_response);
		oseaclient_data_free (data, TRUE);
		return;
	}
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "REFRESH:PROCESS: recieved ok");

	// Check if we already got a list. In that case, first we refresh rows
	if (model->private->list) 
		old_list = model->private->list;
	
	model->private->list = data->data;
	oseaclient_list_first (model->private->list);

	if (old_list) {
		oseaclient_list_first (old_list);
		do {
			node = oseaclient_list_data (model->private->list);
			iter.stamp = model->private->stamp;
			iter.user_data = node;
			path = osea_listmodel_get_path (GTK_TREE_MODEL (model), &iter);
			gtk_tree_model_row_changed (GTK_TREE_MODEL(model), path, &iter);
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "REFRESH:PROCESS: row changed: path %s node id %d",
				 gtk_tree_path_to_string (path),
				 _osea_listmodel_get_node_id (node));
			gtk_tree_path_free (path);

		} while (oseaclient_list_next (old_list) && oseaclient_list_next (model->private->list));

		if (oseaclient_list_length (model->private->list) < oseaclient_list_length (old_list)) {
			do {
				node = oseaclient_list_data (old_list);
				iter.stamp = model->private->stamp;
				iter.user_data = node;
				path = gtk_tree_path_new_from_indices (oseaclient_list_get_cursor_position (old_list), -1);
				gtk_tree_model_row_deleted (GTK_TREE_MODEL(model), path);
				g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "REFRESH:PROCESS: row deleted: path %s node id %d",
					 gtk_tree_path_to_string (path),
					 _osea_listmodel_get_node_id (node));
				gtk_tree_path_free (path);
			} while (oseaclient_list_next (old_list));
		} else
			oseaclient_list_next (model->private->list);
	}
	
	if ((!old_list) || (oseaclient_list_length (model->private->list) > oseaclient_list_length (old_list))) {
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "REFRESH:PROCESS: insert elements");
		do {
			node = oseaclient_list_data (model->private->list);
			iter.stamp     = model->private->stamp;
			iter.user_data = node;
			
			path = osea_listmodel_get_path (GTK_TREE_MODEL (model), &iter);
			
			gtk_tree_model_row_inserted (GTK_TREE_MODEL(model), path, &iter);
			g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "REFRESH:PROCESS: row inserted: path %s node id %d",
				 gtk_tree_path_to_string (path),
				 _osea_listmodel_get_node_id (node));
			gtk_tree_path_free (path);
			
		} while (oseaclient_list_next (model->private->list));
	}

	if (old_list)
		oseaclient_list_destroy (old_list);

	return;
}

void           osea_listmodel_remove_process (AsplDataList *datalist)
{
	OseaClientNulData      * data        = osea_datalist_get (datalist, "DATA");
	AsplListmodel     * model       = osea_datalist_get (datalist, "LISTMODEL");
	gpointer            status_data = osea_datalist_get (datalist, "START_REFRESH_DATA");
	GtkTreeIter       * iter        = (GtkTreeIter *) osea_datalist_get (datalist, "ITER");
	GtkTreePath       * path        = NULL;
	gint                id          = GPOINTER_TO_INT(osea_datalist_get (datalist, "ID"));

	model->private->cfg->end_remove_callback (status_data, model->private->cfg->end_remove_user_data);

	if (data->state == OSEACLIENT_ERROR) {
		dialog_close_run (main_window_get (), GTK_MESSAGE_ERROR,
				  _("There was a problem while removing this node:\n%s"),
				  data->text_response);
		oseaclient_nul_free (data);
		return;
	}
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "REMOVE: element id: %d", id);

	path = osea_listmodel_get_path (GTK_TREE_MODEL(model), iter);
	gtk_tree_model_row_deleted (GTK_TREE_MODEL(model), path);

	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "REMOVE:PROCESS: path %s", gtk_tree_path_to_string (path));
	oseaclient_list_remove (model->private->list, GINT_TO_POINTER(id));	
	g_free (iter);
	return;
}


/* ********************************************************************* */
/*          Functions for implementing gtk_tree_model interface          */
/* ********************************************************************* */

static GtkTreeModelFlags osea_listmodel_get_flags  (GtkTreeModel      *tree_model)
{
	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), 0);

	return GTK_TREE_MODEL_LIST_ONLY;
}

static gint         osea_listmodel_get_n_columns   (GtkTreeModel      *tree_model)
{
	AsplListmodel *  model = NULL;
	GParamSpec    ** paramspecs = NULL;
	GObject        * object;
	GObjectClass   * object_class = NULL;
	gint             param_number = 0;

	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), 0);
	model = OSEA_LISTMODEL(tree_model);
	g_return_val_if_fail (model->private->object_type, 0);

	object       = g_object_new (model->private->object_type, NULL);
	object_class = G_OBJECT_CLASS (OSEACLIENT_DATANODE_GET_CLASS(object));
	paramspecs   = g_object_class_list_properties (object_class, &param_number);
	g_object_unref (object);
	
	return param_number;

}

static GType        osea_listmodel_get_column_type (GtkTreeModel      *tree_model,
						    gint               index)
{
	AsplListmodel *  model = NULL;
	GParamSpec    ** paramspecs = NULL;
	GObject        * object;
	GObjectClass   * object_class;
	gint             param_number = 0;

	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), G_TYPE_INVALID);

	model = OSEA_LISTMODEL(tree_model);
	g_return_val_if_fail (model->private->object_type, G_TYPE_INVALID);

	object       = g_object_new (model->private->object_type, NULL);
	object_class = G_OBJECT_CLASS (OSEACLIENT_DATANODE_GET_CLASS(object));
	paramspecs   = g_object_class_list_properties (object_class, &param_number);
	g_object_unref (object);
	
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GET:COLUMN:TYPE: index %d param_number %d", index, param_number);

	g_return_val_if_fail (index <= param_number, G_TYPE_INVALID);

	return paramspecs[index]->value_type;


}

static gboolean     osea_listmodel_get_iter        (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    GtkTreePath       *path)
{
	// Sets iter to a valid iterator pointing to path.

	OseaClientList     *  list = NULL;
	AsplListmodel *  model = OSEA_LISTMODEL(tree_model);
	gpointer         node = NULL;
	gint             length = 0;
	gint             index;
	
	if (path)
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GET:ITER: getting iter for path: %s", 
			 gtk_tree_path_to_string (path));
	else
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GET:ITER getting iter..");

	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), FALSE);
	g_return_val_if_fail (gtk_tree_path_get_depth (path) == 1, FALSE);

	iter->stamp = -1;
	if (!model->private->list)
		return FALSE;

	
	index = gtk_tree_path_get_indices (path)[0];
	list = model->private->list;
	length = oseaclient_list_length (list);

	if (index >= length)
		return FALSE;
	
	node = oseaclient_list_nth_data (list, index);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GET:ITER: elements: path %d node id %d", index, _osea_listmodel_get_node_id (node));
	iter->stamp = model->private->stamp;
	iter->user_data = node;

	return TRUE;
	
}

static GtkTreePath *osea_listmodel_get_path        (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter)
{
	AsplListmodel *  model  = NULL;
	gint             index;
	GtkTreePath   *  result = NULL;
	OseaClientDataNode *  node;
	gint             id;

	// Check incoming tree_model is a OSEA_LISTMODEL
	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), NULL);

	// Check incoming GtkTreeIter refers to elements of this specific
	// model instance.
	model = OSEA_LISTMODEL(tree_model);
	g_return_val_if_fail (model->private->stamp == iter->stamp, NULL);

	node = OSEACLIENT_DATANODE (iter->user_data);
	id = _osea_listmodel_get_node_id (node);
	index = oseaclient_list_index (model->private->list,  GINT_TO_POINTER (id));
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GET:PATH: looking for element id: %d located at index %d",
		 id, index);

	if (index == -1) {
		// Element not found, so this iter maybe was valid but not right now
		return NULL;
	}
	result = gtk_tree_path_new ();
	gtk_tree_path_append_index (result, index);

	return result;
	
}

static void         osea_listmodel_get_value       (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    gint               column,
						    GValue            *value)
{
	OseaClientDataNode  * node  = NULL;
	AsplListmodel  * model = NULL;
	GParamSpec    ** paramspecs = NULL;
	gint             param_number = 0;
	GObjectClass   * object_class;

	g_return_if_fail (ASPL_IS_LISTMODEL (tree_model));
	g_return_if_fail (column >= 0);

	model = OSEA_LISTMODEL(tree_model);
	g_return_if_fail (model->private->stamp == iter->stamp);

	g_return_if_fail (OSEACLIENT_IS_DATANODE (iter->user_data));
	node = OSEACLIENT_DATANODE (iter->user_data);
	object_class = G_OBJECT_CLASS (OSEACLIENT_DATANODE_GET_CLASS(node));

	paramspecs   = g_object_class_list_properties (object_class, &param_number);
	g_return_if_fail (column < param_number);

	g_value_init (value, paramspecs[column]->value_type);
	
	g_object_get_property (G_OBJECT(node), paramspecs[column]->name, value);
	
	return;

}

static gboolean     osea_listmodel_iter_next       (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter)
{
	OseaClientList     *  list  = NULL;
	AsplListmodel *  model = NULL;
	gint             index = -1;
	OseaClientDataNode *  node  = NULL;
	gint             id    = -1;


	// Check we got a AsplListModel
	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), FALSE);

	// Check we got data and iter is valid for this model
	model = OSEA_LISTMODEL(tree_model);	
	list = model->private->list;
	g_return_val_if_fail (model->private->stamp == iter->stamp, FALSE);
	g_return_val_if_fail (list, FALSE);
	
	g_return_val_if_fail (OSEACLIENT_IS_DATANODE (iter->user_data), FALSE);
	node = OSEACLIENT_DATANODE (iter->user_data);

	// Find element pointed by iter
	id = _osea_listmodel_get_node_id (node);
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "ITER:NEXT: getting next for %d ", id);
	index = oseaclient_list_index (list, GINT_TO_POINTER (id));
	
	// Find, if posible, the next element pointed by iter and update it.
	if (oseaclient_list_set_cursor_position (list, index + 1)) {
		iter->user_data = oseaclient_list_data (list);
		node = OSEACLIENT_DATANODE (iter->user_data);
		id = _osea_listmodel_get_node_id (node);
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "element selected %d ", id);
	}else {
		g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "element not found");
		return FALSE;
	}
	return TRUE;

}

static gboolean     osea_listmodel_iter_children   (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    GtkTreeIter       *parent)
{
	AsplListmodel * model;
	OseaClientList     * list;
	if (parent)
		return FALSE;

	// if parent is NULL returns the first node, equivalent to
	// gtk_tree_model_get_iter_first (tree_model, iter);
	iter->stamp = -1;
	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), FALSE);
	model = OSEA_LISTMODEL (tree_model);

	list = model->private->list;
	g_return_val_if_fail (list, FALSE);

	oseaclient_list_first (list);
	iter->user_data = oseaclient_list_data (list);
	g_return_val_if_fail (iter->user_data, FALSE);
	
	iter->stamp = model->private->stamp;
	return TRUE;

}

static gboolean     osea_listmodel_iter_has_child  (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter)
{
	return FALSE;
}

static gint         osea_listmodel_iter_n_children (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter)
{
	// Returns the number of children that iter has. As a special
	// case, if iter is NULL, then the number of toplevel nodes is
	// returned.
	OseaClientList     *  list = NULL;
	AsplListmodel *  model = NULL;
	
	// Check incoming model
	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), -1);	
	model = OSEA_LISTMODEL(tree_model);
	list = model->private->list;
	g_return_val_if_fail (list, -1);

	// Check special case
	if (iter == NULL)
		oseaclient_list_length (list);

	// Check if iter is valid for this model.
	g_return_val_if_fail (model->private->stamp == iter->stamp, -1);

	return 0;
}

static gboolean     osea_listmodel_iter_nth_child  (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    GtkTreeIter       *parent,
						    gint               n)
{
	AsplListmodel * model;
	OseaClientList     * list;
	//Sets iter to be the child of parent, using the given
	//index. The first index is 0. If n is too big, or parent has
	//no children, iter is set to an invalid iterator and FALSE is
	//returned. parent will remain a valid node after this
	//function has been called. As a special case, if parent is
	//NULL, then the nth root node is set.
	
	g_return_val_if_fail (ASPL_IS_LISTMODEL (tree_model), FALSE);

	model = OSEA_LISTMODEL (tree_model);
	list = model->private->list;
	g_return_val_if_fail (list, FALSE);
	g_return_val_if_fail (n < oseaclient_list_length (list), FALSE);

	if (parent)
		return FALSE;

	oseaclient_list_set_cursor_position (list, n);
	iter->stamp     = model->private->stamp;
	iter->user_data = oseaclient_list_data (list);

	return TRUE;

}

static gboolean     osea_listmodel_iter_parent     (GtkTreeModel      *tree_model,
						    GtkTreeIter       *iter,
						    GtkTreeIter       *child)
{
	iter->stamp = -1;
	return FALSE;
}

GType           osea_listmodel_get_node_type (AsplListmodel * model)
{
	return model->private->object_type;
}

GtkTreeIter   * osea_listmodel_copy_iter  (GtkTreeIter * iter)
{
	GtkTreeIter * result;

	g_return_val_if_fail (iter, NULL);

	result = g_new0 (GtkTreeIter, 1);
	result->stamp     = iter->stamp;
	result->user_data = iter->user_data;
	result->user_data3 = GINT_TO_POINTER (TRUE);

	return result;
}
