//
//  LibOseaClient: common functions to liboseaclient* level and architectural functions.
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

#ifndef __OSEACLIENT_TYPES_H__
#define __OSEACLIENT_TYPES_H__

#include <glib.h>
#include <librr/rr.h>
#include <liboseacomm/oseacomm.h>
#include "oseaclient_list.h"

typedef enum {OSEACLIENT_ERROR, 
	      OSEACLIENT_OK, 
	      OSEACLIENT_UPDATE_SERVICES_NEEDED,
	      OSEACLIENT_SESSION_EXPIRED
} OseaClientState;


struct __OseaClientMultiData {
	OseaClientState state;
	gchar     * text_response;
	GList     * multi_data;
};

struct __OseaClientData {
	OseaClientState  state;
	gchar     * text_response;
	OseaClientList     * data;  
};

struct __OseaClientSimpleData {
	OseaClientState  state;
	gchar     * text_response;
	gint        id;
};

struct __OseaClientNulData {
	OseaClientState  state;
	gchar     * text_response;
};

typedef enum {OSEACLIENT_REQUEST_MULTI_DATA,
	      OSEACLIENT_REQUEST_DATA, 
	      OSEACLIENT_REQUEST_SIMPLE_DATA, 
	      OSEACLIENT_REQUEST_NUL_DATA} OseaClientRequestReturnData;

typedef struct __OseaClientMultiData  OseaClientMultiData;
typedef struct __OseaClientData       OseaClientData;
typedef struct __OseaClientSimpleData OseaClientSimpleData;
typedef struct __OseaClientNulData    OseaClientNulData;


typedef gboolean (* OseaClientMultiFunc)   (OseaClientMultiData  * data, gpointer usr_data);
typedef gboolean (* OseaClientDataFunc)    (OseaClientData       * data, gpointer usr_data);
typedef gboolean (* OseaClientSimpleFunc)  (OseaClientSimpleData * data, gpointer usr_data);
typedef gboolean (* OseaClientNulFunc)     (OseaClientNulData    * data, gpointer usr_data);
typedef gboolean (* OseaClientFunc)        (gpointer          data, gpointer usr_data);


void    oseaclient_nul_free    (OseaClientNulData * data);

void    oseaclient_simple_free (OseaClientSimpleData * data);

void    oseaclient_data_free   (OseaClientData * data, gboolean destroy_list);

void    oseaclient_multi_free  (OseaClientMultiData * data, gboolean destroy_lists);

#endif
