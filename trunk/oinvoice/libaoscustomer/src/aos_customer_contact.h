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

#ifndef __AFDAL_CUSTOMER_CONTACT_H__
#define __AFDAL_CUSTOMER_CONTACT_H__

#include "afdal_customer.h"

typedef struct __AfDalCustomerContact {
	gint                id;
	gint                customer_id;
	gchar              *name;
	gchar              *position;
} AfDalCustomerContact;

gboolean            afdal_customer_contact_new (gchar * name,
						gchar * position,
						gint customer_id,
						AfDalSimpleFunc usr_function,
						gpointer usr_data);

gboolean            afdal_customer_contact_remove (gint contact_id,
						   AfDalNulFunc usr_function,
						   gpointer usr_data);

gboolean            afdal_customer_contact_edit (gint contact_id,
						 gchar * new_name,
						 gchar * new_position,
						 gint new_customer_id,
						 AfDalNulFunc usr_function,
						 gpointer usr_data);

gboolean            afdal_customer_contact_list (gint max_row_number,
						 gint initial_contact,
						 AfDalDataFunc usr_function,
						 gpointer usr_data);

#endif
