//  LibOseaComm:  Support library for errors messages management.
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

#ifndef __OSEACOMM_CODE_H__
#define __OSEACOMM_CODE_H__

#include <glib.h>

typedef enum {OSEACOMM_CODE_OK, 
	      OSEACOMM_CODE_ERROR, 
	      OSEACOMM_CODE_UNKNOWN_SERVICE,
	      OSEACOMM_CODE_XML_VALIDATION_PROBLEM,
	      OSEACOMM_CODE_XML_PARSE_PROBLEM,
	      OSEACOMM_CODE_XML_INCORRECT_PARAMETER,
	      OSEACOMM_CODE_KEY_REQUIRED,
	      OSEACOMM_CODE_KEY_EXPIRED,
	      OSEACOMM_CODE_KEY_INVALID,
	      OSEACOMM_CODE_INSUFFICIENT_PERMISSIONS,
	      OSEACOMM_CODE_SERVICES_UPDATE_NEEDED,
	      OSEACOMM_CODE_SESSION_EXPIRED,
	      OSEACOMM_CODE_UNKNOWN_CODE              // This code must be the last one always
} OseaCommCodeType;

struct _OseaCommCode {
	gchar *status_code;
	gchar *status_text;
};

typedef struct _OseaCommCode OseaCommCode;

OseaCommCodeType  oseacomm_code_get_type   (gchar *status);

const gchar   * oseacomm_code_get_status (OseaCommCodeType type);

OseaCommCode    * oseacomm_code_new        (OseaCommCodeType type, const gchar *extended_message);

void            oseacomm_code_free       (OseaCommCode *code);

#endif 














