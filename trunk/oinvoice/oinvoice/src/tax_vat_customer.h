//  ASPL Fact: invoicing client program for ASPL Fact System
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
//  Copyright (C) 2004 David Mar�n Carre�o
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

#ifndef __TAX_VAT_CUSTOMER_H__
#define __TAX_VAT_CUSTOMER_H__

#include <gtk/gtk.h>
#include <oseacomm/coyote.h>
#include <oseaclientkernel/aos_kernel.h>
#include "tax_vat_customer.h"
#include "osea_listmodel.h"

GtkTreeStore  * tax_vat_customer_list_store_get            (void);

void            tax_vat_customer_cursor_changed            (GtkTreeView *treeview, gpointer user_data);

void            tax_vat_customer_free_selection            (void);

void            tax_vat_customer_refresh                   (void);

gint            tax_vat_customer_get_id_selected           (void);

void            tax_vat_customer_disable_selection_actions ();

void            tax_vat_customer_update_selection_actions  ();

void            tax_vat_customer_new                       (void);

void            tax_vat_customer_edit                      (void);

void            tax_vat_customer_remove                    (void);

void            tax_vat_customer_free_data                 (void);

void            tax_vat_customer_listmodel_create          ();

AsplListmodel * tax_vat_customer_listmodel_get             (void);

#endif
