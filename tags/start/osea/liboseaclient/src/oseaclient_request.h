//
//  LibAfdal: common functions to liboseaclient* level and architectural functions.
//  Copyright (C) 2003  Advanced Software Production Line, S.L.
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

#ifndef __AFDAL_REQUEST_H__
#define __AFDAL_REQUEST_H__
#include "oseaclient.h"

gboolean          oseaclient_request                               (RRConnection * connection, 
							       CoyoteSimpleCfgCompleteMessage return_function,
							       AfDalFunc usr_function, 
							       gpointer usr_data,
							       gchar * service_name,
							       ...);

void              oseaclient_request_process_data                  (RRChannel * channel, 
							       RRFrame * frame,
							       GString * message, 
							       gpointer data, 
							       gpointer custom_data);

void              oseaclient_request_process_simple_data           (RRChannel * channel, 
							       RRFrame * frame,
							       GString * message, 
							       gpointer data, 
							       gpointer custom_data);

void              oseaclient_request_process_nul_data              (RRChannel * channel, 
							       RRFrame * frame,
							       GString * message, 
							       gpointer data, 
							       gpointer custom_data);

gpointer          oseaclient_request_close_and_return_initial_data (AfDalRequestReturnData type,
							       RRChannel * channel,
							       RRFrame * frame,
							       GString * message, 
							       CoyoteDataSet ** returned_dataset,
							       GList ** returned_datasets,
							       gpointer * data,
							       gpointer * custom_data);

gboolean          oseaclient_request_start_afkey_refresh           (gpointer afdal_request_user_data);

gboolean          oseaclient_request_start_session_refresh         (gpointer afdal_request_user_data);

void              oseaclient_request_call_user_function            (AfDalRequestReturnData type, 
							       gpointer handler,
							       gpointer user_data, 
							       gpointer oseaclient_data);



#endif
