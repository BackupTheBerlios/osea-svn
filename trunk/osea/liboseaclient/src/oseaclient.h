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

#ifndef __AFDAL_H__
#define __AFDAL_H__

#include <glib.h>
#include <librr/rr.h>
#include <coyote/coyote.h>

typedef enum {AFDAL_ERROR, 
	      AFDAL_OK, 
	      AFDAL_UPDATE_SERVICES_NEEDED,
	      AFDAL_SESSION_EXPIRED
} AfDalState;

#include "oseaclient_list.h"

struct __AfDalMultiData {
	AfDalState state;
	gchar     * text_response;
	GList     * multi_data;
};

struct __AfDalData {
	AfDalState  state;
	gchar     * text_response;
	AfDalList     * data;  
};

struct __AfDalSimpleData {
	AfDalState  state;
	gchar     * text_response;
	gint        id;
};

struct __AfDalNulData {
	AfDalState  state;
	gchar     * text_response;
};

typedef enum {AFDAL_REQUEST_MULTI_DATA,
	      AFDAL_REQUEST_DATA, 
	      AFDAL_REQUEST_SIMPLE_DATA, 
	      AFDAL_REQUEST_NUL_DATA} AfDalRequestReturnData;

typedef struct __AfDalMultiData  AfDalMultiData;
typedef struct __AfDalData       AfDalData;
typedef struct __AfDalSimpleData AfDalSimpleData;
typedef struct __AfDalNulData    AfDalNulData;


typedef gboolean (* AfDalMultiFunc)   (AfDalMultiData  * data, gpointer usr_data);
typedef gboolean (* AfDalDataFunc)    (AfDalData       * data, gpointer usr_data);
typedef gboolean (* AfDalSimpleFunc)  (AfDalSimpleData * data, gpointer usr_data);
typedef gboolean (* AfDalNulFunc)     (AfDalNulData    * data, gpointer usr_data);
typedef gboolean (* AfDalFunc)        (gpointer          data, gpointer usr_data);


void    oseaclient_nul_free    (AfDalNulData * data);

void    oseaclient_simple_free (AfDalSimpleData * data);

void    oseaclient_data_free   (AfDalData * data, gboolean destroy_list);

void    oseaclient_multi_free  (AfDalMultiData * data, gboolean destroy_lists);

void    oseaclient_print_version_info ();

void    oseaclient_init ();

#include "oseaclient_request.h"
#include "oseaclient_session.h"
#include "oseaclient_event_source.h"
#include "oseaclient_password.h"
#include "oseaclient_support.h"
#include "oseaclient-datanode.h"
#include "oseaclient-detaileddatanode.h"
#include "oseaclient_decimal.h"
#include "oseaclient_afkeys.h"

#endif
