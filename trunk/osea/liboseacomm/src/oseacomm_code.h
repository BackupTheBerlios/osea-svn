//  LibCoyote:  Support library for errors messages management.
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

#ifndef __COYOTE_CODE_H__
#define __COYOTE_CODE_H__

#include <glib.h>

typedef enum {COYOTE_CODE_OK, 
	      COYOTE_CODE_ERROR, 
	      COYOTE_CODE_UNKNOWN_SERVICE,
	      COYOTE_CODE_XML_VALIDATION_PROBLEM,
	      COYOTE_CODE_XML_PARSE_PROBLEM,
	      COYOTE_CODE_XML_INCORRECT_PARAMETER,
	      COYOTE_CODE_KEY_REQUIRED,
	      COYOTE_CODE_KEY_EXPIRED,
	      COYOTE_CODE_KEY_INVALID,
	      COYOTE_CODE_INSUFFICIENT_PERMISSIONS,
	      COYOTE_CODE_SERVICES_UPDATE_NEEDED,
	      COYOTE_CODE_SESSION_EXPIRED,
	      COYOTE_CODE_UNKNOWN_CODE              // This code must be the last one always
} CoyoteCodeType;

struct _CoyoteCode {
	gchar *status_code;
	gchar *status_text;
};

typedef struct _CoyoteCode CoyoteCode;

CoyoteCodeType  oseacomm_code_get_type   (gchar *status);

const gchar   * oseacomm_code_get_status (CoyoteCodeType type);

CoyoteCode    * oseacomm_code_new        (CoyoteCodeType type, const gchar *extended_message);

void            oseacomm_code_free       (CoyoteCode *code);

#endif 














