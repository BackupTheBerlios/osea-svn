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

#ifndef __AFDAL_CUSTOMER_ENUM_VALUES_H__
#define __AFDAL_CUSTOMER_ENUM_VALUES_H__

#include "afdal_customer.h"

typedef struct __AfDalCustomerEnum_values {
	gint                id;
	gchar              *enum_type;
	gint                enum_order_in_type;
	gchar              *enum_value;
} AfDalCustomerEnum_values;

gboolean            afdal_customer_enum_values_address_set (gint enum_values_id,
							    gint address_id,
							    gboolean value_to_set,
							    AfDalNulFunc usr_function,
							    gpointer usr_data);

gboolean            afdal_customer_enum_values_address_update_all (gint address_id,
								   AfDalList * setof_enum_values,
								   AfDalNulFunc usr_function,
								   gpointer usr_data);

gboolean            afdal_customer_enum_values_address_list (gint initial_enum_values,
							     gint max_row_number,
							     gint address_id,
							     AfDalDataFunc usr_function,
							     gpointer usr_data);

gboolean            afdal_customer_enum_values_address_remove (gint enum_values_id,
							       gint address_id,
							       AfDalNulFunc usr_function,
							       gpointer usr_data);

gboolean            afdal_customer_enum_values_address_add (gint enum_values_id,
							    gint address_id,
							    AfDalNulFunc usr_function,
							    gpointer usr_data);

gboolean            afdal_customer_enum_values_telephone_set (gint enum_values_id,
							      gint telephone_id,
							      gboolean value_to_set,
							      AfDalNulFunc usr_function,
							      gpointer usr_data);

gboolean            afdal_customer_enum_values_telephone_update_all (gint telephone_id,
								     AfDalList *
								     setof_enum_values,
								     AfDalNulFunc usr_function,
								     gpointer usr_data);

gboolean            afdal_customer_enum_values_telephone_list (gint initial_enum_values,
							       gint max_row_number,
							       gint telephone_id,
							       AfDalDataFunc usr_function,
							       gpointer usr_data);

gboolean            afdal_customer_enum_values_telephone_remove (gint enum_values_id,
								 gint telephone_id,
								 AfDalNulFunc usr_function,
								 gpointer usr_data);

gboolean            afdal_customer_enum_values_telephone_add (gint enum_values_id,
							      gint telephone_id,
							      AfDalNulFunc usr_function,
							      gpointer usr_data);

gboolean            afdal_customer_enum_values_inet_data_set (gint enum_values_id,
							      gint inet_data_id,
							      gboolean value_to_set,
							      AfDalNulFunc usr_function,
							      gpointer usr_data);

gboolean            afdal_customer_enum_values_inet_data_update_all (gint inet_data_id,
								     AfDalList *
								     setof_enum_values,
								     AfDalNulFunc usr_function,
								     gpointer usr_data);

gboolean            afdal_customer_enum_values_inet_data_list (gint initial_enum_values,
							       gint max_row_number,
							       gint inet_data_id,
							       AfDalDataFunc usr_function,
							       gpointer usr_data);

gboolean            afdal_customer_enum_values_inet_data_remove (gint enum_values_id,
								 gint inet_data_id,
								 AfDalNulFunc usr_function,
								 gpointer usr_data);

gboolean            afdal_customer_enum_values_inet_data_add (gint enum_values_id,
							      gint inet_data_id,
							      AfDalNulFunc usr_function,
							      gpointer usr_data);

gboolean            afdal_customer_enum_values_customer_set (gint enum_values_id,
							     gint customer_id,
							     gboolean value_to_set,
							     AfDalNulFunc usr_function,
							     gpointer usr_data);

gboolean            afdal_customer_enum_values_customer_update_all (gint customer_id,
								    AfDalList * setof_enum_values,
								    AfDalNulFunc usr_function,
								    gpointer usr_data);

gboolean            afdal_customer_enum_values_customer_list (gint initial_enum_values,
							      gint max_row_number,
							      gint customer_id,
							      AfDalDataFunc usr_function,
							      gpointer usr_data);

gboolean            afdal_customer_enum_values_customer_remove (gint enum_values_id,
								gint customer_id,
								AfDalNulFunc usr_function,
								gpointer usr_data);

gboolean            afdal_customer_enum_values_customer_add (gint enum_values_id,
							     gint customer_id,
							     AfDalNulFunc usr_function,
							     gpointer usr_data);

gboolean            afdal_customer_enum_values_new (gchar * enum_type,
						    gint enum_order_in_type,
						    gchar * enum_value,
						    AfDalSimpleFunc usr_function,
						    gpointer usr_data);

gboolean            afdal_customer_enum_values_remove (gint enum_values_id,
						       AfDalNulFunc usr_function,
						       gpointer usr_data);

gboolean            afdal_customer_enum_values_edit (gint enum_values_id,
						     gchar * new_enum_type,
						     gint new_enum_order_in_type,
						     gchar * new_enum_value,
						     AfDalNulFunc usr_function,
						     gpointer usr_data);

gboolean            afdal_customer_enum_values_list (gint max_row_number,
						     gint initial_enum_values,
						     AfDalDataFunc usr_function,
						     gpointer usr_data);

#endif
