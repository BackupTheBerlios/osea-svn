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
#include "oseaclient.h"

struct __OseaClientDecimal {
	gchar * number;
	glong   precision;
	glong   width;
};


gchar * oseaclient_decimal_stringfy (OseaClientDecimal * decimal) {
	g_return_val_if_fail (decimal, NULL);

	return g_strdup (decimal->number);
}

void    oseaclient_decimal_free (OseaClientDecimal * decimal) {
	g_return_if_fail (decimal);
	

	g_free (decimal->number);
	g_free (decimal);

	return;
}

OseaClientDecimal * oseaclient_decimal_new      (gchar * value)
{
	OseaClientDecimal  * result = NULL;
	gchar        ** aux_strings;
	
	
	
	g_return_val_if_fail (value, NULL);

#warning "Add support to better check if incoming value is a properly number"		
	aux_strings = g_strsplit (value, ",", 0);
	g_return_val_if_fail (aux_strings[0], NULL);
	g_return_val_if_fail (aux_strings[1], NULL);


	result = g_new0 (OseaClientDecimal, 1);
	result->number = g_strdup (value);
	result->width  = strlen (aux_strings[0]) + strlen (aux_strings[1]);
	result->precision = strlen (aux_strings[1]);
	
	g_strfreev (aux_strings);
	return result;
}

