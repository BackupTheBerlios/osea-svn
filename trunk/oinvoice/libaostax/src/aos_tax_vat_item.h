//
//  LibAfdalTax: interface library to the tax daemon
//  Copyright (C) 2002  Advanced Software Production Line, S.L.
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

#ifndef __AOS_TAX_VAT_ITEM_H__
#define __AOS_TAX_VAT_ITEM_H__

#include "aos-tax-vatitem.h"

gboolean           aos_tax_vat_item_list     (gint           initial_item, 
						gint           max_row_number,
						OseaClientDataFunc  usr_function, 
						gpointer       usr_data);

gboolean           aos_tax_vat_item_new      (gchar           * reference, 
						gchar           * name, 
						gchar           * description,
						OseaClientSimpleFunc   usr_function, 
						gpointer          usr_data);

gboolean           aos_tax_vat_item_remove   (gint id,
						OseaClientNulFunc   usr_function,
						gpointer       usr_data);


gboolean           aos_tax_vat_item_edit     (gint           id,
						gchar        * new_reference, 
						gchar        * new_name, 
						gchar        * new_description,
						OseaClientNulFunc   usr_function, 
						gpointer       usr_data);

#endif




