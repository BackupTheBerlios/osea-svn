//  customer: LibOseaClient layer for client-side
//  Copyright (C) 2002,2003 Advanced Software Production Line, S.L.

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

#ifndef __AOS_CUSTOMER_TELEPHONE_H__
#define __AOS_CUSTOMER_TELEPHONE_H__

#include "aos_customer.h"

typedef struct __AosCustomerTelephone {
	gint                id;
	gint                enum_values_id;
	gint                customer_id;
	gchar              *phone;
	gchar              *description;
} AosCustomerTelephone;

gboolean            aos_customer_telephone_new (gchar * phone,
						  gchar * description,
						  gint customer_id,
						  gint enum_values_id,
						  OseaClientSimpleFunc usr_function,
						  gpointer usr_data);

gboolean            aos_customer_telephone_remove (gint telephone_id,
						     OseaClientNulFunc usr_function,
						     gpointer usr_data);

gboolean            aos_customer_telephone_edit (gint telephone_id,
						   gchar * new_phone,
						   gchar * new_description,
						   gint new_customer_id,
						   gint new_enum_values_id,
						   OseaClientNulFunc usr_function,
						   gpointer usr_data);

gboolean            aos_customer_telephone_list (gint max_row_number,
						   gint initial_telephone,
						   OseaClientDataFunc usr_function,
						   gpointer usr_data);

#endif
