//  customer: LibAfDal layer for client-side
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

#ifndef __AFDAL_CUSTOMER_ADDRESS_H__
#define __AFDAL_CUSTOMER_ADDRESS_H__

#include "afdal_customer.h"

typedef struct __AfDalCustomerAddress {
	gint                id;
	gint                enum_values_id;
	gint                customer_id;
	gchar              *address;
	gchar              *city;
	gchar              *state;
	gchar              *zip_code;
	gchar              *country;
} AfDalCustomerAddress;

gboolean            afdal_customer_address_new (gchar * address,
						gchar * city,
						gchar * state,
						gchar * zip_code,
						gchar * country,
						gint customer_id,
						gint enum_values_id,
						AfDalSimpleFunc usr_function,
						gpointer usr_data);

gboolean            afdal_customer_address_remove (gint address_id,
						   AfDalNulFunc usr_function,
						   gpointer usr_data);

gboolean            afdal_customer_address_edit (gint address_id,
						 gchar * new_address,
						 gchar * new_city,
						 gchar * new_state,
						 gchar * new_zip_code,
						 gchar * new_country,
						 gint new_customer_id,
						 gint new_enum_values_id,
						 AfDalNulFunc usr_function,
						 gpointer usr_data);

gboolean            afdal_customer_address_list (gint max_row_number,
						 gint initial_address,
						 AfDalDataFunc usr_function,
						 gpointer usr_data);

#endif
