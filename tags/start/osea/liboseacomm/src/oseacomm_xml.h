//  LibCoyote:  Support library with xml and communication functions.
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
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

#ifndef __COYOTE_XML_H__
#define __COYOTE_XML_H__

#include <glib.h>
#include "oseacomm_code.h"

typedef struct _CoyoteXmlObject CoyoteXmlObject;

typedef struct _CoyoteXmlMessage CoyoteXmlMessage;

typedef struct _CoyoteXmlServiceData CoyoteXmlServiceData;

typedef struct _CoyoteXmlServiceNode CoyoteXmlServiceNode;

typedef enum {COYOTE_XML_SERVICE_REQUEST, COYOTE_XML_SERVICE_RESPONSE} CoyoteXmlServiceType;

typedef enum {COYOTE_XML_ARG_STRING, COYOTE_XML_ARG_DATASET} CoyoteXmlArgType;

struct _CoyoteXmlMessage {
	gchar *content;
	gint len;
};

struct _CoyoteXmlServiceNode {
	gchar * attrib;
	CoyoteXmlArgType type;
	gpointer value;
};

struct _CoyoteXmlServiceData {
	CoyoteXmlServiceType type;
	gchar * name;
	gchar * protocol_version;
	gchar * status;
	gchar * status_message;
	GList * item_list;
};


CoyoteXmlObject *      oseacomm_xml_new_object           (void);

void                   oseacomm_xml_destroy_object       (CoyoteXmlObject * object);

void                   oseacomm_xml_add_request_service  (CoyoteXmlObject * object,
							gchar * name_of_service, 
							...);

void                   oseacomm_xml_add_vrequest_service  (CoyoteXmlObject * object,
							 gchar * name_of_service, 
							 va_list args);

void                   oseacomm_xml_add_response_service (CoyoteXmlObject * object,
							CoyoteCodeType status_code,
							gchar * explanation,
							...);

void                   oseacomm_xml_add_vresponse_service (CoyoteXmlObject * object,
							 CoyoteCodeType status_code,
							 gchar * explanation,
							 va_list args);

gboolean               oseacomm_xml_validate_message     (CoyoteXmlMessage * message);
				      
CoyoteXmlServiceData * oseacomm_xml_parse_message        (CoyoteXmlMessage * message);

void                   oseacomm_xml_parse_destroy        (CoyoteXmlServiceData * data);

CoyoteXmlMessage *     oseacomm_xml_build_message        (CoyoteXmlObject * object);

void                   oseacomm_xml_destroy_message      (CoyoteXmlMessage * message);

#endif 














