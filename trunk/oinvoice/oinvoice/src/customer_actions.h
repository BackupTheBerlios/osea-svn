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

#ifndef __CUSTOMER_ACTIONS_H_
#define __CUSTOMER_ACTIONS_H_

#include <gtk/gtk.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>
#include "customer.h"

void customer_actions_update_selection_actions ();

void customer_actions_free_data                ();

gint customer_actions_get_id_selected          ();

void customer_actions_cursor_changed           ();

void customer_actions_new                      ();

void customer_actions_edit                     ();

void customer_actions_remove                   ();

void customer_actions_refresh                  ();

void customer_actions_listmodel_create         ();
#endif
