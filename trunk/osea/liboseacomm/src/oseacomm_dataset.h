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

#ifndef __COYOTE_DATASET_H__
#define __COYOTE_DATASET_H__

#include<glib.h>

typedef struct __CoyoteDataSet CoyoteDataSet;

CoyoteDataSet * oseacomm_dataset_new        ();

const gchar *   oseacomm_dataset_get        (CoyoteDataSet * data, 
					   gint row, 
					   gint col);

gboolean        oseacomm_dataset_set        (CoyoteDataSet * dataset, 
					   gchar * data,
					   gint row, 
					   gint col);

void            oseacomm_dataset_add        (CoyoteDataSet * data, gchar * user_data);

void            oseacomm_dataset_add_nth    (CoyoteDataSet * data, ...);

void            oseacomm_dataset_new_row    (CoyoteDataSet * data);

gint            oseacomm_dataset_get_height (CoyoteDataSet * data);

gint            oseacomm_dataset_get_width  (CoyoteDataSet * data);

gint            oseacomm_dataset_lookup_row (CoyoteDataSet * data, gint column, const gchar *value);

void            oseacomm_dataset_free       (CoyoteDataSet * data);

void            oseacomm_dataset_print      (CoyoteDataSet * data);

#endif
