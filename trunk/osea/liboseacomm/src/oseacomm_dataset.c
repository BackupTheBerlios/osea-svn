//  LibOseaComm:  Support library with xml and communication functions.
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
#include "oseacomm_dataset.h"

#define LOG_DOMAIN "OSEACOMM_DATASET"

struct __OseaCommDataSet {
	gchar *** table;       
	gint      width;    
	gint      height;  
	gboolean  first_row;
	gboolean  fixated;
	gint      index;

};


/**
 * oseacomm_dataset_new:
 * 
 * Creates a new OseaCommDataSet object. This type represents a set of
 * data sorted as a table. This type is thought to be efficient access
 * and easy to fill. 
 *
 * To access data to an already created OseaCommDateSet, which will be
 * the most common case, you have to use oseacomm_dataset_get function.  
 * 
 * But, if you want to create a OseaCommDataSet and fill it with data
 * you have to use oseacomm_dataset_add and oseacomm_dataset_new_row both
 * functions. 
 *
 * Complete examples about how to use oseacomm_dataset are placed in the
 * "test" directory.
 * 
 * Return value: a new OseaCommDataSet object or NULL if it fails.
 **/
OseaCommDataSet *  oseacomm_dataset_new    ()
{
	OseaCommDataSet * result;

	result = g_new0 (OseaCommDataSet, 1);

	result->first_row = TRUE;
	result->fixated = FALSE;

	return result;
}

/**
 * oseacomm_dataset_get:
 * @data: A OseaCommDataSet object.
 * @row:  the row where resides the asked element
 * @col: the column where resied the asked element
 * 
 * Gets the element located at [@row,@col].
 * 
 * Return value: returns the asked element or NULL if it not where
 * filled or the asked element falls outside @data boundaries.
 **/
const gchar *   oseacomm_dataset_get    (OseaCommDataSet * data, gint row, gint col)
{

	g_return_val_if_fail (data, NULL);
	g_return_val_if_fail (row >= 0, NULL);
	g_return_val_if_fail (col >= 0, NULL);

	if ((row > data->height) || (col > data->width)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Trying to get an element outside dataset structure boundaries");
		return NULL;
	}
	return data->table[row][col];
}


/**
 * oseacomm_dataset_set:
 * @dataset: A OseaCommDataSet object.
 * @data: new value for the replaced cell. This data is copied so you must free it.
 * @row: the row where resides the element to be replaced
 * @col: the column where resides the element to be replaced
 * 
 * Sets the value of the cell located at [@row,@col].
 * 
 * Return value: TRUE if the cell exists, FALSE otherwise
 **/
gboolean        oseacomm_dataset_set        (OseaCommDataSet * dataset, 
					   gchar * data,
					   gint row, 
					   gint col)
{

	g_return_val_if_fail (data, FALSE);
	g_return_val_if_fail (dataset, FALSE);
	g_return_val_if_fail (row >= 0, FALSE);
	g_return_val_if_fail (col >= 0, FALSE);

	if ((row > dataset->height) || (col > dataset->width)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
		       "Trying to get an element outside dataset structure boundaries");
		return FALSE;
	}

	g_free (dataset->table[row][col]);
	dataset->table[row][col] = g_strdup (data);

	return TRUE;

}

void            oseacomm_dataset_add     (OseaCommDataSet * data, gchar * user_data)
{
	g_return_if_fail (data);

	if (!data->table) {
		// Empty table case
		data->table = g_new0 (gchar **, 2);

		data->table[0] = g_new0 (gchar *, 2);

		data->table[0][0] = g_strdup (user_data);

		data->height = 1;
		data->width  = 1;
		return;
	}
	
	if (data->first_row) {
		
		// First row flags means we are setting on the fly table width, so in this
		// case we have to use realloc functions

		data->table[data->height - 1] = g_realloc (data->table[data->height - 1], 
							   (data->width + 2) * sizeof (gchar *));
		
		data->table[data->height - 1][data->width] = g_strdup (user_data);

		data->table[data->height - 1][data->width + 1] = NULL;

		data->width = data->width + 1;
	}else {

		// Table width 
		if (data->index < data->width) {

			data->table[data->height - 1][data->index] = g_strdup (user_data);
			
			data->index = data->index + 1;
		}else {
			g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, 
			       "Trying to add an element outside dataset structure boundaries");
		}
	}

	return;
}

/**
 * oseacomm_dataset_add_nth:
 * @data: A OseaCommDataSet object.
 * Varargs@: A chain list ended by NULL.
 * 
 * Convenience function to those who prefer to create a full row
 * without multiple calls to oseacomm_dataset_add. So, if you want to
 * create a row with 3 elements you could make 3 calls to
 * oseacomm_dataset_add or make a single call to oseacomm_dataset_add_nth
 * as follow: 
 *
 * oseacomm_dataset_add_nth (data, "chain1", "chain2", "chain3", NULL);
 *
 * This function will do for you all necesary oseacomm_dataset_add
 * calls.
 **/
void            oseacomm_dataset_add_nth    (OseaCommDataSet * data, ...)
{
	gchar   * element;
	va_list   ap;

	g_return_if_fail (data);

	va_start (ap, data);

	while ((element = va_arg (ap, gchar *))) {
		oseacomm_dataset_add (data, element);
	}

	va_end (ap);

	return;
}

/**
 * oseacomm_dataset_new_row:
 * @data: A OseaCommDataSet object.
 * 
 * This function add a new object to @data, so next calls to
 * oseacomm_dataset_add will insert data elements at the next created
 * row. This function also sets the maximun @data width. So, when the
 * first row is been create, this will set the maximun width for
 * futher rows.
 * 
 **/
void            oseacomm_dataset_new_row    (OseaCommDataSet * data)
{
	
	gint      i;

	g_return_if_fail (data);
	g_return_if_fail (data->height);


	if (data->first_row) 
		data->first_row = FALSE;

	data->height = data->height + 1;

 	data->table = (gchar ***) g_realloc (data->table, (data->height + 1) * sizeof (gchar **));

	data->table[data->height] = NULL;

	data->table[data->height - 1] = g_new0 (gchar *, data->width + 1);
	
	data->table[data->height - 1][data->width] = NULL;

	for (i = 0; i < data->width; i++)
		data->table[data->height - 1][i] = NULL;

	data->index = 0;
	return;
}

/**
 * oseacomm_dataset_get_height:
 * @data: A OseaCommDataSet object.
 * 
 * Gets @data height
 * 
 * Return value: 
 **/
gint            oseacomm_dataset_get_height (OseaCommDataSet * data)
{
	g_return_val_if_fail (data, -1);
	return data->height;
}

/**
 * oseacomm_dataset_get_width:
 * @data: A OseaCommDataSet object.
 * 
 * Gets @data width.
 * 
 * Return value: 
 **/
gint            oseacomm_dataset_get_width  (OseaCommDataSet * data)
{
	g_return_val_if_fail (data, -1);
	return data->width;
}

/**
 * oseacomm_dataset_lookup_row:
 * @data: A OseaCommDataSet object.
 * @column: Column number to compare (starting by 0).
 * @value: String to compare.
 * 
 * Returns the index of the first row whose column @column has a
 * string equal to @value.
 * 
 * Return value: The index of the found row (starting by  0) or -1 if
 * there was an error.
 **/
gint            oseacomm_dataset_lookup_row (OseaCommDataSet * data, 
					   gint column, 
					   const gchar *value)
{
	gint i;

	if (column > oseacomm_dataset_get_width (data)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Trying to access element outside data set boundaries");
		return -1;
	}

	for (i = 0; i < oseacomm_dataset_get_height (data); i++) {

		if (oseacomm_dataset_get (data, i, column)) {
			if (!g_strcasecmp (oseacomm_dataset_get (data, i, column), value)) 
				return i;
		}
	}
	return -1;
}


/**
 * oseacomm_dataset_free:
 * @data: A OseaCommDataSet object.
 * 
 * Free all memory allocated by @data.
 *
 **/
void            oseacomm_dataset_free   (OseaCommDataSet * data)
{

	gint i;
	gint j;
	g_return_if_fail (data);

	for (i = 0; i < data->height; i++) {
		
		for (j = 0; j < data->width; j++) {
			
			if (data->table[i][j]) {
				g_free (data->table[i][j]);
			}
		}

 		g_free (data->table[i]);
	}

	g_free (data->table);
	
	return;
	
}




/**
 * oseacomm_dataset_print:
 * @data: dataset to be printed out
 * 
 * Prints dataset @data content. This function is for debug purpose.
 **/
void            oseacomm_dataset_print      (OseaCommDataSet * data)
{
	gint i, j;

	for  ( i = 0; i < oseacomm_dataset_get_height (data); i++ ) {
		for ( j = 0; j < oseacomm_dataset_get_width (data); j ++) {
			if (oseacomm_dataset_get (data, i, j))
				g_printerr (" %s ", oseacomm_dataset_get (data, i, j));
			else
				g_printerr (" null ");
					 
		}
		g_printerr ("\n");
	}
	return;
}
