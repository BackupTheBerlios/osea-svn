//  af_customer: Daemon for the server-side
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


#ifndef __AF_CUSTOMER_ENUM_VALUES_H__
#define __AF_CUSTOMER_ENUM_VALUES_H__

#include <glib.h>
#include <coyote/coyote.h>

gboolean            af_customer_enum_values_address_set (CoyoteXmlServiceData * data,
							 gpointer user_data,
							 RRChannel * channel,
							 gint msg_no);
gboolean            af_customer_enum_values_address_update_all (CoyoteXmlServiceData * data,
								gpointer user_data,
								RRChannel * channel,
								gint msg_no);
gboolean            af_customer_enum_values_address_list (CoyoteXmlServiceData * data,
							  gpointer user_data,
							  RRChannel * channel,
							  gint msg_no);
gboolean            af_customer_enum_values_address_remove (CoyoteXmlServiceData * data,
							    gpointer user_data,
							    RRChannel * channel,
							    gint msg_no);
gboolean            af_customer_enum_values_address_add (CoyoteXmlServiceData * data,
							 gpointer user_data,
							 RRChannel * channel,
							 gint msg_no);
gboolean            af_customer_enum_values_telephone_set (CoyoteXmlServiceData * data,
							   gpointer user_data,
							   RRChannel * channel,
							   gint msg_no);
gboolean            af_customer_enum_values_telephone_update_all (CoyoteXmlServiceData * data,
								  gpointer user_data,
								  RRChannel * channel,
								  gint msg_no);
gboolean            af_customer_enum_values_telephone_list (CoyoteXmlServiceData * data,
							    gpointer user_data,
							    RRChannel * channel,
							    gint msg_no);
gboolean            af_customer_enum_values_telephone_remove (CoyoteXmlServiceData * data,
							      gpointer user_data,
							      RRChannel * channel,
							      gint msg_no);
gboolean            af_customer_enum_values_telephone_add (CoyoteXmlServiceData * data,
							   gpointer user_data,
							   RRChannel * channel,
							   gint msg_no);
gboolean            af_customer_enum_values_inet_data_set (CoyoteXmlServiceData * data,
							   gpointer user_data,
							   RRChannel * channel,
							   gint msg_no);
gboolean            af_customer_enum_values_inet_data_update_all (CoyoteXmlServiceData * data,
								  gpointer user_data,
								  RRChannel * channel,
								  gint msg_no);
gboolean            af_customer_enum_values_inet_data_list (CoyoteXmlServiceData * data,
							    gpointer user_data,
							    RRChannel * channel,
							    gint msg_no);
gboolean            af_customer_enum_values_inet_data_remove (CoyoteXmlServiceData * data,
							      gpointer user_data,
							      RRChannel * channel,
							      gint msg_no);
gboolean            af_customer_enum_values_inet_data_add (CoyoteXmlServiceData * data,
							   gpointer user_data,
							   RRChannel * channel,
							   gint msg_no);
gboolean            af_customer_enum_values_customer_set (CoyoteXmlServiceData * data,
							  gpointer user_data,
							  RRChannel * channel,
							  gint msg_no);
gboolean            af_customer_enum_values_customer_update_all (CoyoteXmlServiceData * data,
								 gpointer user_data,
								 RRChannel * channel,
								 gint msg_no);
gboolean            af_customer_enum_values_customer_list (CoyoteXmlServiceData * data,
							   gpointer user_data,
							   RRChannel * channel,
							   gint msg_no);
gboolean            af_customer_enum_values_customer_remove (CoyoteXmlServiceData * data,
							     gpointer user_data,
							     RRChannel * channel,
							     gint msg_no);
gboolean            af_customer_enum_values_customer_add (CoyoteXmlServiceData * data,
							  gpointer user_data,
							  RRChannel * channel,
							  gint msg_no);
gboolean            af_customer_enum_values_new (CoyoteXmlServiceData * data,
						 gpointer user_data,
						 RRChannel * channel,
						 gint msg_no);
gboolean            af_customer_enum_values_remove (CoyoteXmlServiceData * data,
						    gpointer user_data,
						    RRChannel * channel,
						    gint msg_no);
gboolean            af_customer_enum_values_edit (CoyoteXmlServiceData * data,
						  gpointer user_data,
						  RRChannel * channel,
						  gint msg_no);
gboolean            af_customer_enum_values_list (CoyoteXmlServiceData * data,
						  gpointer user_data,
						  RRChannel * channel,
						  gint msg_no);
#endif
