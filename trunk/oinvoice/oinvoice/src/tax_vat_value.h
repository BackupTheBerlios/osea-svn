//  ASPL Fact: invoicing client program for ASPL Fact System
//  Copyright (C) 2003 Advanced Software Production Line, S.L.
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

#ifndef __TAX_VAT_VALUE_H__
#define __TAX_VAT_VALUE_H__

#include <gtk/gtk.h>
#include <liboseacomm/oseacomm.h>
#include <aoskernel/aos_kernel.h>

GtkTreeStore * tax_vat_value_tree_store_get   (void);

void           tax_vat_value_refresh          (void);

void           tax_vat_value_set              (gint id_customer, gint id_item, glong value, GtkTreeIter *iter);

GtkTreeStore * tax_vat_value_regenerate_model (void);

void           tax_vat_value_free_data        (void);

#endif
