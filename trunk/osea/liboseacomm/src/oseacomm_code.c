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

#include <glib.h>
#include "oseacomm_code.h"

#define LOG_DOMAIN "oseacomm-code"

typedef struct __OseaCommCodeInternal {
	OseaCommCodeType    code;
	gchar           * status_code;
	gchar           * string_description;
}OseaCommCodeInternal;

OseaCommCodeInternal oseacomm_internal_codes [] = {
	{OSEACOMM_CODE_OK,                       "100", "Ok message"},
	{OSEACOMM_CODE_ERROR,                    "201", "ERROR message"},
	{OSEACOMM_CODE_UNKNOWN_SERVICE,          "202", "UNKNOWN SERVICE message"},
	{OSEACOMM_CODE_XML_VALIDATION_PROBLEM,   "203", "XML VALIDATION PROBLEM message"},
	{OSEACOMM_CODE_XML_PARSE_PROBLEM,        "204", "XML PARSE PROBLEM message"},
	{OSEACOMM_CODE_XML_INCORRECT_PARAMETER,  "205", "XML INCORRECT PARAMENTER message"},
	{OSEACOMM_CODE_KEY_REQUIRED,             "206", "KEY REQUIRED message"},
	{OSEACOMM_CODE_KEY_EXPIRED,              "207", "KEY EXPIRED message"},
	{OSEACOMM_CODE_KEY_INVALID,              "208", "KEY INVALID message"},
	{OSEACOMM_CODE_INSUFFICIENT_PERMISSIONS, "209", "INSUFFICIENT PERMISSIONS message"},
	{OSEACOMM_CODE_SERVICES_UPDATE_NEEDED,   "210", "SERVICES UPDATE NEEDED message"},
	{OSEACOMM_CODE_SESSION_EXPIRED,          "211", "SESSION EXPIRED message"},
	{OSEACOMM_CODE_UNKNOWN_CODE,              NULL, NULL}
};

/**
 * oseacomm_code_get_type:
 * @status: A string containing a numeric status
 * 
 *  Translation function. Returns the oseacomm code type corresponding to the
 *  given status.
 * 
 * Return value: The code type for the given numeric status. If the status
 * is unknown, the value 0 is returned.
 **/

OseaCommCodeType oseacomm_code_get_type (gchar *status)
{
	gint iterator = 0;

	while (oseacomm_internal_codes[iterator].status_code) {
		if (!(g_ascii_strcasecmp (status, oseacomm_internal_codes[iterator].status_code))) 
			return oseacomm_internal_codes[iterator].code;
		iterator++;
	}
	/* for error trapping: */
	return OSEACOMM_CODE_UNKNOWN_CODE;
}



/**
 * oseacomm_code_get_status:
 * @type: Type of code
 * 
 * Translation function. Returns the numeric code corresponding to the
 * given code type.
 * 
 * Return value: A string containing the numeric code for the given
 *type. If the code is not allowed, a NULL is returned.
 **/

const gchar *oseacomm_code_get_status (OseaCommCodeType type)
{
	gint iterator = 0;
	
	while (oseacomm_internal_codes[iterator].status_code) {
		if (oseacomm_internal_codes[iterator].code == type)
			return oseacomm_internal_codes[iterator].status_code;
		iterator++;
	}
	/* for error trapping: */
	g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Undefined oseacomm code");
	return NULL;
}


/**
 * oseacomm_code_new:
 * @type: Type of the code to be generated
 * @extended_message: Aditional message to be attached
 * 
 * Creates a new structure filled with the status code and the message
 * for that status. 
 * 
 * 
 * Return value: A new structure of the type OseaCommCode (public type)
 * or NULL if the code can not be generated due to wrong arguments.
 **/

OseaCommCode *oseacomm_code_new (OseaCommCodeType type, const gchar *extended_message)
{
	OseaCommCode * code = NULL;

	code = g_new0 (OseaCommCode, 1);

	code->status_code = g_strdup (oseacomm_code_get_status (type));

	if (!(code->status_code)) {
		/* A wrong type was specified */
		g_free (code->status_code);
		g_free (code);
		return NULL;
	}

	code->status_text = g_strdup_printf ("%s : %s", oseacomm_internal_codes[type].string_description, extended_message);
		
	return code;
}


/**
 * oseacomm_code_free:
 * @code: The variable to be freed
 * 
 *  Frees up the memory allocated by @code.
 **/

void oseacomm_code_free (OseaCommCode *code)
{
	if (code) {
		g_free (code->status_code);
		g_free (code->status_text);
		g_free (code);
	}
}

















