//  LibOseaComm:  Support library with xml and communication functions.
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

#ifndef __OSEACOMM_XML_H__
#define __OSEACOMM_XML_H__

#include <glib.h>
#include "oseacomm_code.h"

typedef struct _OseaCommXmlObject OseaCommXmlObject;

typedef struct _OseaCommXmlMessage OseaCommXmlMessage;

typedef struct _OseaCommXmlServiceData OseaCommXmlServiceData;

typedef struct _OseaCommXmlServiceNode OseaCommXmlServiceNode;

typedef enum {OSEACOMM_XML_SERVICE_REQUEST, OSEACOMM_XML_SERVICE_RESPONSE} OseaCommXmlServiceType;

typedef enum {OSEACOMM_XML_ARG_STRING, OSEACOMM_XML_ARG_DATASET} OseaCommXmlArgType;

struct _OseaCommXmlMessage {
	gchar *content;
	gint len;
};

struct _OseaCommXmlServiceNode {
	gchar * attrib;
	OseaCommXmlArgType type;
	gpointer value;
};

struct _OseaCommXmlServiceData {
	OseaCommXmlServiceType type;
	gchar * name;
	gchar * protocol_version;
	gchar * status;
	gchar * status_message;
	GList * item_list;
};


OseaCommXmlObject *      oseacomm_xml_new_object           (void);

void                   oseacomm_xml_destroy_object       (OseaCommXmlObject * object);

void                   oseacomm_xml_add_request_service  (OseaCommXmlObject * object,
							gchar * name_of_service, 
							...);

void                   oseacomm_xml_add_vrequest_service  (OseaCommXmlObject * object,
							 gchar * name_of_service, 
							 va_list args);

void                   oseacomm_xml_add_response_service (OseaCommXmlObject * object,
							OseaCommCodeType status_code,
							gchar * explanation,
							...);

void                   oseacomm_xml_add_vresponse_service (OseaCommXmlObject * object,
							 OseaCommCodeType status_code,
							 gchar * explanation,
							 va_list args);

gboolean               oseacomm_xml_validate_message     (OseaCommXmlMessage * message);
				      
OseaCommXmlServiceData * oseacomm_xml_parse_message        (OseaCommXmlMessage * message);

void                   oseacomm_xml_parse_destroy        (OseaCommXmlServiceData * data);

OseaCommXmlMessage *     oseacomm_xml_build_message        (OseaCommXmlObject * object);

void                   oseacomm_xml_destroy_message      (OseaCommXmlMessage * message);

#endif 














