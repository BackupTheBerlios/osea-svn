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

#ifndef __USERGROUP_USER_H__
#define __USERGROUP_USER_H__

#include <gtk/gtk.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>

enum { USERGROUP_USER_ID_COLUMN, 
       USERGROUP_USER_NICK_COLUMN,
       USERGROUP_USER_DESCRIPTION_COLUMN,
       USERGROUP_USER_COLUMN_NUMBER };

GtkTreeStore * usergroup_user_list_store_get            (void);

void           usergroup_user_cursor_changed            (GtkTreeView *treeview, gpointer user_data);

void           usergroup_user_free_selection            (void);

void           usergroup_user_refresh                   (void);

gint           usergroup_user_get_id_selected           (void);

void           usergroup_user_enable_selection_actions  ();

void           usergroup_user_disable_selection_actions ();

void           usergroup_user_update_selection_actions  ();

void           usergroup_user_new                       (void);

void           usergroup_user_edit                      (void);

void           usergroup_user_remove                    (void);

void           usergroup_user_free_data                 (void);

#endif
