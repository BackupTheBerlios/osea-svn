//
//  LibAosTax: interface library to the tax daemon
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

#ifndef __AOS_TAX_VAT_VALUE_H__
#define __AOS_TAX_VAT_VALUE_H__

#include "aos_tax.h"

typedef struct __AosTaxVatValueHeader {
	gint id;
	gchar *reference;
} AosTaxVatValueHeader;

typedef struct __AosTaxVatValueKey {
	gint id_item;
	gint id_customer;
} AosTaxVatValueKey;

typedef struct __AosTaxVatValueData {
	AosTaxVatValueKey *key;
	glong value;
} AosTaxVatValueData;


gboolean           aos_tax_vat_value_list        (OseaClientMultiFunc   usr_function, 
						    gpointer         usr_data);

gboolean           aos_tax_vat_value_set         (gint             id_customer,
						    gint             id_item,
						    glong            value,
						    OseaClientNulFunc     usr_function, 
						    gpointer         usr_data);

gboolean           aos_tax_vat_value_get         (gint             id_customer,
						    gint             id_item,
						    OseaClientSimpleFunc  usr_function,
						    gpointer         usr_data);

#endif
