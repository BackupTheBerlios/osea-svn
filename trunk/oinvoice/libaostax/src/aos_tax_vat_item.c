//  LibAosTax: interface library to the tax daemonstdin: is not a tty

//  Copyright (C) 2002  Advanced Software Production Line, S.L.
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

#include "aos_tax_vat_item.h"
#include <glib.h>
#include <stdlib.h>
#include <librr/rr.h>
#include <liboseacomm/oseacomm.h>

#define LOG_DOMAIN "AOS_TAX_ITEM"


gint __vat_item_compare_id (gconstpointer a, gconstpointer b) {
	if (GPOINTER_TO_INT(a) > GPOINTER_TO_INT (b))
		return 1;
	else if (GPOINTER_TO_INT(a) < GPOINTER_TO_INT (b))
		return -1;
	else
		return 0;
}

void __vat_item_destroy (gpointer data)
{
	AosTaxVatItem *item = (AosTaxVatItem *) data;
	
	g_object_unref ((GObject *) item);

	return;
}


OseaClientList *__vat_item_create_tree (OseaCommDataSet *data)
{
	AosTaxVatItem * item;
	OseaClientList       * result;
	gint i;
	
	result = oseaclient_list_new_full (__vat_item_compare_id, NULL, 
				      __vat_item_destroy);

	for (i=0; i < oseacomm_dataset_get_height (data); i++) {
		item = aos_tax_vatitem_new (atoi (oseacomm_dataset_get (data, i, 0)),
					      (gchar *) oseacomm_dataset_get (data, i, 1),
					      (gchar *) oseacomm_dataset_get (data, i, 2),
					      (gchar *) oseacomm_dataset_get (data, i, 3));

		oseaclient_list_insert (result, GINT_TO_POINTER (item->id), item);
	}

	return result;
}



static void __aos_tax_vat_item_list_process (RRChannel * channel, 
					       RRFrame   * frame,
					       GString   * message, 
					       gpointer    data, 
					       gpointer    custom_data)
{


	OseaClientData     * oseaclient_data = NULL;
	OseaCommDataSet * dataset = NULL;

	g_return_if_fail (channel);
	g_return_if_fail (message);
	g_return_if_fail (data);

	// Close the channel properly
	oseaclient_data = oseaclient_request_close_and_return_initial_data 
		(OSEACLIENT_REQUEST_DATA, channel, frame, message, &dataset, NULL, &data, &custom_data);

	if (! oseaclient_data)
		return;

	// Fill user data inside user_data
	oseaclient_data->data = __vat_item_create_tree (dataset);

	// Call to user defined callback.	
	oseaclient_request_call_user_function (OSEACLIENT_REQUEST_DATA, data, custom_data, oseaclient_data);

	return;
}


/**
 * aos_tax_vat_item_list:
 * @initial_item: identifier of the first item we want to get
 * @max_row_number: maximum number of rows we want to get in the table
 * @usr_function: function to be executed when the server response arrives.
 * @usr_data: user data passed to @usr_function
 * 
 * This function require os-tax server for a VAT-Item-Type list,
 * beginning with the item-type whose identifier is @id, and
 * with @max_row_number as maximum number of item-types.
 * 
 * Return value: TRUE if request is sent or FALSE if not.
 **/
gboolean aos_tax_vat_item_list (gint initial_item, gint max_row_number,
				  OseaClientDataFunc usr_function, gpointer usr_data)
{
	gboolean result = FALSE;
	GString *init, *rows;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-tax", NULL);
	if (! connection)
		return FALSE;
	
	init = g_string_new (NULL);
	rows = g_string_new (NULL);

	g_string_sprintf(init, "%d", initial_item);
	g_string_sprintf(rows, "%d", max_row_number);

	result = oseaclient_request (connection, 
				__aos_tax_vat_item_list_process, 
				(OseaClientFunc) usr_function, usr_data,
				"list_vat_item", 
				"initial", OSEACOMM_XML_ARG_STRING, init->str, 
				"row_number", OSEACOMM_XML_ARG_STRING, rows->str, 
				NULL);

	g_string_free (init, TRUE);
	g_string_free (rows, TRUE);
	
	return result;
}


/**
 * aos_tax_vat_item_new:
 * @reference: User identifier for the item type.
 * @name: Name of the item
 * @description: Item description
 * @usr_function: User space function to be executed when the server response arrives.
 * @usr_data: User space data passed to @usr_function.
 * 
 * Adds a new item to the database defined by @reference, @name
 * and @description.
 * 
 * Return value: TRUE if the item is added. FALSE in anyother case.
 **/
gboolean aos_tax_vat_item_new (gchar * reference, 
				 gchar * name, 
				 gchar * description,
				 OseaClientSimpleFunc usr_function, 
				 gpointer usr_data)
{
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-tax", NULL);
	if (! connection)
		return FALSE;

	return oseaclient_request (connection, 
			      oseaclient_request_process_simple_data, 
			      (OseaClientFunc) usr_function, usr_data,
			      "new_vat_item", 
			      "reference", OSEACOMM_XML_ARG_STRING, reference, 
			      "name", OSEACOMM_XML_ARG_STRING, name, 
			      "description", OSEACOMM_XML_ARG_STRING, description, 
			      NULL);
	
}


/**
 * aos_tax_vat_item_edit:
 * @id: unique item identifier
 * @new_reference: new reference to be set
 * @new_name: new name to be set.
 * @new_description: new description to be set.
 * @usr_function: User space function to be executed when the server response arrives.
 * @usr_data: User space data passed to @usr_function.
 * 
 * Edit an already created item identified by @id.
 * 
 * Return value: 
 **/
gboolean aos_tax_vat_item_edit (gint                       id,
				  gchar                    * new_reference, 
				  gchar                    * new_name, 
				  gchar                    * new_description,
				  OseaClientNulFunc               usr_function, 
				  gpointer                   usr_data) 
{
	GString * identifier = NULL;
	gboolean  result;
	RRConnection * connection = NULL;
	
	connection = oseaclient_session_get_connection ("os-tax", NULL);
	if (! connection)
		return FALSE;

	identifier = g_string_new (NULL);
	
	g_string_sprintf (identifier, "%d", id);
	
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"edit_vat_item", 
				"id", OSEACOMM_XML_ARG_STRING, identifier->str,
				"new_reference", OSEACOMM_XML_ARG_STRING, new_reference,
				"new_name", OSEACOMM_XML_ARG_STRING, new_name,
				"new_description", OSEACOMM_XML_ARG_STRING, new_description,
				NULL);
	
	g_string_free (identifier, TRUE);
	
	return result;
	
}

/**
 * aos_tax_vat_item_remove:
 * @id: unique item identifier
 * @usr_function: User space function to be executed when the server response arrives.
 * @usr_data: User space data passed to @usr_function.
 * 
 * Remove an already create item identified by @id.
 * 
 * Return value: 
 **/
gboolean aos_tax_vat_item_remove (gint id,
				    OseaClientNulFunc usr_function,
				    gpointer usr_data)
{
	GString * identifier = NULL;
	gboolean  result;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("os-tax", NULL);
	if (! connection)
		return FALSE;
	
	identifier = g_string_new (NULL);
	
	g_string_sprintf (identifier, "%d", id);
	
	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"remove_vat_item", 
				"id", OSEACOMM_XML_ARG_STRING, identifier->str,
				NULL);
	
	g_string_free (identifier, TRUE);
	
	return result;
}







