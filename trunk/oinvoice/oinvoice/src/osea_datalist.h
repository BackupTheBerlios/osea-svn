//  Copyright (C) 2002, 2003  Advanced Software Production Line, S.L.
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
#ifndef __OSEA_DATALIST_H__
#define __OSEA_DATALIST_H__
#include <glib.h>

typedef GTree AsplDataList;

AsplDataList * osea_datalist_new      ();

void           osea_datalist_free     (AsplDataList * adl);

void           osea_datalist_set_full (AsplDataList * adl, const gchar * name, gpointer data, GDestroyNotify free_func);

void           osea_datalist_set      (AsplDataList * adl, const gchar * name, gpointer data);

gpointer       osea_datalist_get      (AsplDataList * adl, const gchar * name);

void           osea_datalist_print    (AsplDataList * adl);

#endif
