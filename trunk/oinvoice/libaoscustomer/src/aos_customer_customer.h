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

#ifndef __AFDAL_CUSTOMER_CUSTOMER_INTERFACE_H__
#define __AFDAL_CUSTOMER_CUSTOMER_INTERFACE_H__

#include "afdal_customer.h"

gboolean            afdal_customer_customer_address_set (gint customer_id,
							 gint address_id,
							 gboolean value_to_set,
							 AfDalNulFunc usr_function,
							 gpointer usr_data);

gboolean            afdal_customer_customer_address_update_all (gint address_id,
								AfDalList * setof_customer,
								AfDalNulFunc usr_function,
								gpointer usr_data);

gboolean            afdal_customer_customer_address_list (gint initial_customer,
							  gint max_row_number,
							  gint address_id,
							  AfDalDataFunc usr_function,
							  gpointer usr_data);

gboolean            afdal_customer_customer_address_remove (gint customer_id,
							    gint address_id,
							    AfDalNulFunc usr_function,
							    gpointer usr_data);

gboolean            afdal_customer_customer_address_add (gint customer_id,
							 gint address_id,
							 AfDalNulFunc usr_function,
							 gpointer usr_data);

gboolean            afdal_customer_customer_telephone_set (gint customer_id,
							   gint telephone_id,
							   gboolean value_to_set,
							   AfDalNulFunc usr_function,
							   gpointer usr_data);

gboolean            afdal_customer_customer_telephone_update_all (gint telephone_id,
								  AfDalList * setof_customer,
								  AfDalNulFunc usr_function,
								  gpointer usr_data);

gboolean            afdal_customer_customer_telephone_list (gint initial_customer,
							    gint max_row_number,
							    gint telephone_id,
							    AfDalDataFunc usr_function,
							    gpointer usr_data);

gboolean            afdal_customer_customer_telephone_remove (gint customer_id,
							      gint telephone_id,
							      AfDalNulFunc usr_function,
							      gpointer usr_data);

gboolean            afdal_customer_customer_telephone_add (gint customer_id,
							   gint telephone_id,
							   AfDalNulFunc usr_function,
							   gpointer usr_data);

gboolean            afdal_customer_customer_inet_data_set (gint customer_id,
							   gint inet_data_id,
							   gboolean value_to_set,
							   AfDalNulFunc usr_function,
							   gpointer usr_data);

gboolean            afdal_customer_customer_inet_data_update_all (gint inet_data_id,
								  AfDalList * setof_customer,
								  AfDalNulFunc usr_function,
								  gpointer usr_data);

gboolean            afdal_customer_customer_inet_data_list (gint initial_customer,
							    gint max_row_number,
							    gint inet_data_id,
							    AfDalDataFunc usr_function,
							    gpointer usr_data);

gboolean            afdal_customer_customer_inet_data_remove (gint customer_id,
							      gint inet_data_id,
							      AfDalNulFunc usr_function,
							      gpointer usr_data);

gboolean            afdal_customer_customer_inet_data_add (gint customer_id,
							   gint inet_data_id,
							   AfDalNulFunc usr_function,
							   gpointer usr_data);

gboolean            afdal_customer_customer_contact_set (gint customer_id,
							 gint contact_id,
							 gboolean value_to_set,
							 AfDalNulFunc usr_function,
							 gpointer usr_data);

gboolean            afdal_customer_customer_contact_update_all (gint contact_id,
								AfDalList * setof_customer,
								AfDalNulFunc usr_function,
								gpointer usr_data);

gboolean            afdal_customer_customer_contact_list (gint initial_customer,
							  gint max_row_number,
							  gint contact_id,
							  AfDalDataFunc usr_function,
							  gpointer usr_data);

gboolean            afdal_customer_customer_contact_remove (gint customer_id,
							    gint contact_id,
							    AfDalNulFunc usr_function,
							    gpointer usr_data);

gboolean            afdal_customer_customer_contact_add (gint customer_id,
							 gint contact_id,
							 AfDalNulFunc usr_function,
							 gpointer usr_data);

gboolean            afdal_customer_customer_new (gchar * legal_id,
						 gchar * name,
						 gchar * surname,
						 gchar * salesman,
						 gint vat_customer_type,
						 gchar * customer_ref,
						 gint enum_values_id,
						 AfDalSimpleFunc usr_function,
						 gpointer usr_data);

gboolean            afdal_customer_customer_remove (gint customer_id,
						    AfDalNulFunc usr_function,
						    gpointer usr_data);

gboolean            afdal_customer_customer_edit (gint customer_id,
						  gchar * new_legal_id,
						  gchar * new_name,
						  gchar * new_surname,
						  gchar * new_salesman,
						  gint new_vat_customer_type,
						  gchar * new_customer_ref,
						  gint new_enum_values_id,
						  AfDalNulFunc usr_function,
						  gpointer usr_data);

gboolean            afdal_customer_customer_list (gint max_row_number,
						  gint initial_customer,
						  AfDalDataFunc usr_function,
						  gpointer usr_data);

#endif
