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

#ifndef __OSEACOMM_DATASET_H__
#define __OSEACOMM_DATASET_H__

#include<glib.h>

typedef struct __OseaCommDataSet OseaCommDataSet;

OseaCommDataSet * oseacomm_dataset_new        ();

const gchar *   oseacomm_dataset_get        (OseaCommDataSet * data, 
					   gint row, 
					   gint col);

gboolean        oseacomm_dataset_set        (OseaCommDataSet * dataset, 
					   gchar * data,
					   gint row, 
					   gint col);

void            oseacomm_dataset_add        (OseaCommDataSet * data, gchar * user_data);

void            oseacomm_dataset_add_nth    (OseaCommDataSet * data, ...);

void            oseacomm_dataset_new_row    (OseaCommDataSet * data);

gint            oseacomm_dataset_get_height (OseaCommDataSet * data);

gint            oseacomm_dataset_get_width  (OseaCommDataSet * data);

gint            oseacomm_dataset_lookup_row (OseaCommDataSet * data, gint column, const gchar *value);

void            oseacomm_dataset_free       (OseaCommDataSet * data);

void            oseacomm_dataset_print      (OseaCommDataSet * data);

#endif
