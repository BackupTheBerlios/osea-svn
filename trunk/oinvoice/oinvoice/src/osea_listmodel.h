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

#ifndef __OSEA_LISTMODEL_H__
#define __OSEA_LISTMODEL_H__

#include <glib-object.h>
#include <gtk/gtktreemodel.h>
#include <oseaclient/afdal.h>


#define OSEA_LISTMODEL_TYPE             (osea_listmodel_get_type ())
#define OSEA_LISTMODEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ASPL_LISTMODEL_TYPE, AsplListmodel))
#define OSEA_LISTMODEL_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), ASPL_LISTMODEL_TYPE, AsplListmodelClass))
#define ASPL_IS_LISTMODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OSEA_LISTMODEL_TYPE))
#define ASPL_IS_LISTMODEL_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), OSEA_LISTMODEL_TYPE))
#define OSEA_LISTMODEL_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), ASPL_LISTMODEL_TYPE, AsplListmodelClass))


typedef struct   _AsplListmodel AsplListmodel;
typedef struct   _AsplListmodelClass AsplListmodelClass;
typedef struct   _AsplListmodelPrivate AsplListmodelPrivate;

struct _AsplListmodel {
        GObject parent;

	/*< private >*/
	AsplListmodelPrivate *private;
};

struct _AsplListmodelClass {
        GObjectClass parent;
};

/* Used by OSEA_LISTMODEL_TYPE */
GType osea_listmodel_get_type (void);


/* Other type definitions */

typedef gboolean (* AsplListmodelGetDataFunction)       (gint           initial_user, 
						         gint           max_row_number,
						         OseaClientDataFunc  usr_function, 
						         gpointer       usr_data);

typedef gboolean (* AsplListmodelRemoveFunction)        (gint           id, 
						         OseaClientNulFunc   usr_function, 
						         gpointer       usr_data);

typedef gpointer (* AsplListmodelStartFunc)  (gpointer       usr_data);
typedef void     (* AsplListmodelEndFunc)    (gpointer       start_refresh_data,
					      gpointer       usr_data);


typedef struct _AsplListmodelConfig AsplListmodelConfig;
struct  _AsplListmodelConfig {
	gchar                         * server_name;

	AsplListmodelGetDataFunction    refresh_service;
	AsplListmodelStartFunc          start_refresh_callback;
	gpointer                        start_refresh_user_data;
	AsplListmodelEndFunc            end_refresh_callback;
	gpointer                        end_refresh_user_data;

	AsplListmodelRemoveFunction     remove_service;
	gchar                         * remove_confirmation_question;
	AsplListmodelStartFunc          start_remove_callback;
	gpointer                        start_remove_user_data;
	AsplListmodelEndFunc            end_remove_callback;
	gpointer                        end_remove_user_data;
};

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
						 gpointer                        end_remove_user_data);

/* Methods */

AsplListmodel * osea_listmodel_new         (AsplListmodelConfig * cfg, GType object_type);

void            osea_listmodel_refresh     (AsplListmodel *model);

void            osea_listmodel_remove      (AsplListmodel *model, GtkTreeIter *iter);

OseaClientDataNode * osea_listmodel_get_node    (AsplListmodel *model, GtkTreeIter *iter);

void            osea_listmodel_insert_node (AsplListmodel *model, OseaClientDataNode *node);

void            osea_listmodel_edit_node   (AsplListmodel *model, GtkTreeIter *iter, OseaClientDataNode *node);

gint            osea_listmodel_get_id      (AsplListmodel *model, GtkTreeIter *iter);

gint            osea_listmodel_get_column  (AsplListmodel *model, const gchar * column_name);

GType           osea_listmodel_get_node_type (AsplListmodel * model);

GtkTreeIter   * osea_listmodel_copy_iter  (GtkTreeIter * iter);
#endif
