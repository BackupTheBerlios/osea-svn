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

#ifndef __USERGROUP_GROUP_H__
#define __USERGROUP_GROUP_H__

#include <gtk/gtk.h>
#include <liboseacomm/oseacomm.h>
#include <aoskernel/aos_kernel.h>

enum { USERGROUP_GROUP_ID_COLUMN, 
       USERGROUP_GROUP_NAME_COLUMN,
       USERGROUP_GROUP_DESCRIPTION_COLUMN,
       USERGROUP_GROUP_COLUMN_NUMBER };

enum { USERGROUP_GROUP_USER_NAME,
       USERGROUP_GROUP_USER_SELECTED,
       USERGROUP_GROUP_USER_ID,
       USERGROUP_GROUP_USER_ACTIVATABLE,
       USERGROUP_GROUP_USER_COLUMN_NUMBER};

GtkTreeStore * usergroup_group_list_store_get            (void);

void           usergroup_group_cursor_changed            (GtkTreeView *treeview, gpointer user_data);

void           usergroup_group_free_selection            (void);

void           usergroup_group_refresh                   (void);

gint           usergroup_group_get_id_selected           (void);

void           usergroup_group_disable_selection_actions ();

void           usergroup_group_update_selection_actions  ();

void           usergroup_group_new                       (void);

void           usergroup_group_edit                      (void);

void           usergroup_group_remove                    (void);

void           usergroup_group_free_data                 (void);

#endif
