//  LibOseaClient: common functions to liboseaclient* level.
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

#include "oseaclient_support.h"
#include <stdlib.h>

gint       oseaclient_support_compare_id (gconstpointer a, gconstpointer b)
{
	if (GPOINTER_TO_INT(a) > GPOINTER_TO_INT (b))
		return 1;
	else if (GPOINTER_TO_INT(a) < GPOINTER_TO_INT (b))
		return -1;
	else
		return 0;
}

gint       oseaclient_support_compare_string (gconstpointer a, gconstpointer b)
{
	return (strcmp ((const gchar *)a, (const gchar *)b));
}

glong      oseaclient_support_get_number (const gchar * from_string)
{
	gchar * aux_string = NULL;
	glong   value;

	if (!from_string || !(* from_string))
		return 0;
	
	value = strtol (from_string, &aux_string, 10);

	if (aux_string && (* aux_string))
		return 0;

	return value;
}
