//  LibAfdalTax: interface library to the tax daemonstdin: is not a tty

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

#include "aos_tax_vat_value.h"
#include <glib.h>
#include <stdlib.h>
#include <librr/rr.h>
#include <liboseacomm/oseacomm.h>

#define LOG_DOMAIN "AOS_TAX_VALUE"


gint __aos_tax_vat_value_compare_int (gconstpointer a, gconstpointer b)
{
	if (GPOINTER_TO_INT(a) > GPOINTER_TO_INT(b))
		return 1;
	else if (GPOINTER_TO_INT(a) < GPOINTER_TO_INT(b))
		return -1;
	else
		return 0;
}

gint __aos_tax_vat_value_compare_keys (gconstpointer a, gconstpointer b)
{
	AosTaxVatValueKey *ka = (AosTaxVatValueKey *) a;
	AosTaxVatValueKey *kb = (AosTaxVatValueKey *) b;

	if (ka->id_item > kb->id_item)
		return 1;
	else if (ka->id_item < kb->id_item)
		return -1;
	else if (ka->id_customer > kb->id_customer)
		return 1;
	else if (ka->id_customer < kb->id_customer)
		return -1;
	else
		return 0;
}

void __vat_value_header_destroy (gpointer data)
{
	AosTaxVatValueHeader *header = (AosTaxVatValueHeader *) data;
	
	g_free (header->reference);
	g_free (header);

	return;
}

OseaClientList * __vat_value_create_customer_index_tree (OseaCommDataSet * dataset)
{
	AosTaxVatValueHeader * node;
	OseaClientList * result = NULL;
	gint    i = 0;

	result = oseaclient_list_new_full (__aos_tax_vat_value_compare_int, NULL, __vat_value_header_destroy);
	
	for (i = 0; i < oseacomm_dataset_get_height (dataset); i++) {
		node = g_new0 (AosTaxVatValueHeader, 1);

		node->id = atoi (oseacomm_dataset_get (dataset, i, 0));
		node->reference = g_strdup (oseacomm_dataset_get (dataset, i , 1));

		oseaclient_list_insert (result, GINT_TO_POINTER (node->id), node);
	}

	return result;
}

OseaClientList * __vat_value_create_item_index_tree (OseaCommDataSet * dataset)
{
	AosTaxVatValueHeader * node;
	OseaClientList * result = NULL;
	gint    i = 0;

	result = oseaclient_list_new_full (__aos_tax_vat_value_compare_int, NULL, __vat_value_header_destroy);
	
	for (i = 0; i < oseacomm_dataset_get_height (dataset); i++) {
		node = g_new0 (AosTaxVatValueHeader, 1);

		node->id = atoi (oseacomm_dataset_get (dataset, i, 0));
		node->reference = g_strdup (oseacomm_dataset_get (dataset, i , 1));

		oseaclient_list_insert (result, GINT_TO_POINTER (node->id), node);
	}

	return result;
}


void __vat_value_key_destroy (gpointer data)
{
	AosTaxVatValueKey *key = (AosTaxVatValueKey *) data;	
	g_free (key);

	return;
}

void __vat_value_data_destroy (gpointer data)
{
	AosTaxVatValueData *value = (AosTaxVatValueData *) data;

	// We mustn't free "value->key", as it's freed with __vat_value_key_destroy
	g_free (value);

	return;
}


OseaClientList * __vat_value_create_value_index_tree (OseaCommDataSet * value, 
					     OseaCommDataSet * customer_index, 
					     OseaCommDataSet * item_index)
{
	AosTaxVatValueData * node;
	AosTaxVatValueKey  * key;
	OseaClientList * result = NULL;
	gint i,j;


	result = oseaclient_list_new_full (__aos_tax_vat_value_compare_keys,  
				      __vat_value_key_destroy, __vat_value_data_destroy);

	for (i = 0; i < oseacomm_dataset_get_height (value); i++) {
		for (j = 0; j < oseacomm_dataset_get_width (value); j++) {

			key = g_new0 (AosTaxVatValueKey, 1);
			key->id_item = atoi ( oseacomm_dataset_get (item_index, i, 0) );
			key->id_customer = atoi ( oseacomm_dataset_get (customer_index, j, 0) );

			node = g_new0 (AosTaxVatValueData, 1);

			node->key = key;
			node->value = atol ( oseacomm_dataset_get (value, i, j) );
			g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Value %d, %d: %ld", i, j, node->value);
			oseaclient_list_insert (result, node->key, node);
		}
	}
	
	return result;
}

void __aos_tax_vat_value_list_process (RRChannel *channel, RRFrame *frame,
					 GString *message, gpointer  data, 
					 gpointer custom_data )
{
	OseaClientMultiData   * oseaclient_data = NULL;
	GList            * dataset_list = NULL;
	GList            * gtree_list = NULL;
	OseaClientList        * gtree_data = NULL;
	OseaCommDataSet    * customer_index = NULL;
	OseaCommDataSet    * item_index = NULL;
	
	

	g_return_if_fail (channel);
	g_return_if_fail (data);

	oseaclient_data = oseaclient_request_close_and_return_initial_data (OSEACLIENT_REQUEST_MULTI_DATA,
								  channel,
								  frame,
								  message,
								  NULL,
								  &dataset_list, &data, &custom_data);

	if (! oseaclient_data)
		return;

	if (oseaclient_data->state == OSEACLIENT_ERROR)
		return oseaclient_request_call_user_function (OSEACLIENT_REQUEST_MULTI_DATA, data, custom_data, oseaclient_data);

	// Create initial customer index gtree 
	customer_index = (OseaCommDataSet *) dataset_list->data;
	gtree_data = __vat_value_create_customer_index_tree ( customer_index );
	
	gtree_list = g_list_append (gtree_list, gtree_data);
	dataset_list = g_list_next (dataset_list);

	// Create item index gtree
	item_index = (OseaCommDataSet *) dataset_list->data;
	gtree_data = __vat_value_create_item_index_tree ( item_index );
	
	gtree_list = g_list_append (gtree_list, gtree_data);
	dataset_list = g_list_next (dataset_list);

	// Create value gtree
	gtree_data = __vat_value_create_value_index_tree ( (OseaCommDataSet *) dataset_list->data, 
							   customer_index, item_index);

	gtree_list = g_list_append (gtree_list, gtree_data);

	// Set all recieved data and call user function.
	oseaclient_data->multi_data = gtree_list;

	return oseaclient_request_call_user_function (OSEACLIENT_REQUEST_MULTI_DATA, data, custom_data, oseaclient_data);
}

/**
 * aos_tax_vat_value_list:
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * List all available values at af-tax. 
 * 
 * Return value: 
 **/
gboolean           aos_tax_vat_value_list        (OseaClientMultiFunc  usr_function, 
						    gpointer       usr_data)
{
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("af-tax", NULL);
	if (! connection)
		return FALSE;

	return oseaclient_request (connection, __aos_tax_vat_value_list_process, (OseaClientFunc) usr_function, usr_data,
			      "list_vat_value", 
			      NULL);

}


/**
 * aos_tax_vat_value_set:
 * @id_customer: 
 * @id_item: 
 * @value: value to set
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * This function set the given @value to the relation between the
 * given customer type @id_customer and the given item type @id_item.
 * 
 * Return value: 
 **/
gboolean           aos_tax_vat_value_set         (gint            id_customer,
						    gint            id_item,
						    glong           value,
						    OseaClientNulFunc usr_function, 
						    gpointer        usr_data)
{
	GString      * identifier_customer = NULL;
	GString      * identifier_item = NULL;
	GString      * str_value = NULL;
	RRConnection * connection = NULL;

	gboolean  result;
	
	identifier_customer = g_string_new (NULL);
	identifier_item = g_string_new (NULL);
	str_value = g_string_new (NULL);
	
	g_string_sprintf (identifier_customer, "%d", id_customer);
	g_string_sprintf (identifier_item, "%d", id_item);
	g_string_sprintf (str_value, "%ld", value);
	
	connection = oseaclient_session_get_connection ("af-tax", NULL);
	if (! connection)
		return FALSE;

	result = oseaclient_request (connection, oseaclient_request_process_nul_data, (OseaClientFunc) usr_function, usr_data,
				"set_vat_value", 
				"customer_type_id", OSEACOMM_XML_ARG_STRING, identifier_customer->str,
				"item_type_id", OSEACOMM_XML_ARG_STRING, identifier_item->str,
				"value", OSEACOMM_XML_ARG_STRING, str_value->str,
				NULL);
	
	g_string_free (identifier_customer, TRUE);
	g_string_free (identifier_item, TRUE);
	g_string_free (str_value, TRUE);
	
	return result;

}

/**
 * aos_tax_vat_value_get:
 * @id_customer: 
 * @id_item: 
 * @usr_function: function to be executed on server response.
 * @usr_data: user data to be passed to @user_function
 * 
 * This function return the value from the relation between the given
 * customer type @id_customer and the given item type @id_item.
 * 
 * Return value: 
 **/
gboolean           aos_tax_vat_value_get         (gint           id_customer,
						    gint           id_item,
						    OseaClientSimpleFunc   usr_function,
						    gpointer       usr_data)
{
	GString * identifier_customer = NULL;
	GString * identifier_item = NULL;
	RRConnection * connection = NULL;

	connection = oseaclient_session_get_connection ("af-tax", NULL);
	if (! connection)
		return FALSE;

	
	identifier_customer = g_string_new (NULL);
	identifier_item = g_string_new (NULL);
	
	g_string_sprintf (identifier_customer, "%d", id_customer);
	g_string_sprintf (identifier_item, "%d", id_item);


	return oseaclient_request (connection, oseaclient_request_process_simple_data, (OseaClientFunc) usr_function, usr_data,
			      "get_vat_value", 
			      "customer_type_id", OSEACOMM_XML_ARG_STRING, identifier_customer->str,
			      "item_type_id", OSEACOMM_XML_ARG_STRING, identifier_item->str,
			      NULL);

}
