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

#include "oseaclient_list.h"

struct __OseaClientList {
	GList            * list;	
	GList            * id_list;
	GList            * cursor;
	gint               cursor_position;
	GCompareFunc       compare_func;
	GDestroyNotify     destroy_func;
	GDestroyNotify     key_destroy_func;
};

/**
 * oseaclient_list_new:
 * @: 
 * 
 * Creates a new empty OseaClientList structure.
 * 
 * Return value: a new OseaClientList.
 **/
OseaClientList * oseaclient_list_new (GCompareFunc compare_func)
{
	OseaClientList *result = g_new0(OseaClientList, 1);
	
	result->compare_func = compare_func;
	
	return result;
}

/**
 * oseaclient_list_new_full: 
 * @value_destroy_func: a function to free the memory allocated for
 * the key used when removing the entry from the OseaClientList or NULL if
 * you don't want to supply such a function.
 * 
 * Creates a new OseaClientList like oseaclient_list_new() and allows to specify
 * a function to free the memory allocated for the key that get called
 * when removing the entry from the OseaClientList.
 * 
 * Return value: a new OseaClientList.
 **/
OseaClientList * oseaclient_list_new_full (GCompareFunc   compare_func,
				 GDestroyNotify key_destroy_func, 
				 GDestroyNotify value_destroy_func)
{
	OseaClientList *result = g_new0 (OseaClientList, 1);

	result->compare_func = compare_func;
	result->destroy_func = value_destroy_func;
	result->key_destroy_func = key_destroy_func;

	return result;
}

/**
 * oseaclient_list_insert:
 * @list: an OseaClientList.
 * @key: the key to insert.
 * @value: the value corresponding to the key.
 * 
 * Inserts a key/value pair into a OseaClientList.
 **/
void        oseaclient_list_insert (OseaClientList *list, gpointer key, gpointer value)
{
	g_return_if_fail (list);

	list->list = g_list_append (list->list, value);
	list->id_list = g_list_append (list->id_list, key);
	list->cursor = list->list;
	list->cursor_position = 0;

}


/**
 * oseaclient_list_index:
 * @list: an OseaClientList.
 * @key: the key to look up.
 * 
 * Gets the position of the element containing the given key (starting
 * from 0).
 * 
 * Return value: the index of the element containing the key, or -1 if
 * the key is not found.
 **/
gint        oseaclient_list_index  (OseaClientList *list, gpointer key)
{
	GList * cursor;
	gint    iterator = 0;

	g_return_val_if_fail (list, -1);
	cursor = g_list_first (list->id_list);

	while (cursor) {

		if (list->compare_func (cursor->data, key) == 0) {
			return iterator;			
		}

		cursor = cursor->next;
		iterator ++;
	}
	return -1;
}



/**
 * oseaclient_list_lookup:
 * @list: an OseaClientList.
 * @key: the key to look up.
 * 
 * Gets the value corresponding to the given key.
 * 
 * Return value: the value corresponding to the key.
 **/
gpointer    oseaclient_list_lookup (OseaClientList *list, gpointer key)
{
	gint    index;
	GList * cursor;  

	g_return_val_if_fail (list, NULL);
	g_return_val_if_fail (key, NULL);

	index  = oseaclient_list_index (list, key);
	cursor = g_list_first (list->list);

	if (index == -1)
		return NULL;
	
	return g_list_nth_data(cursor, index);

}

/**
 * oseaclient_list_nth_data:
 * @list: an OseaClientList.
 * @n: the position of the element, counting from 0. 
 * 
 * Gets the data of the element at the given position.
 * 
 * Return value: the element, or NULL if the position is off the end of the OseaClientList.
 **/
gpointer    oseaclient_list_nth_data (OseaClientList *list, gint n)
{
	g_return_val_if_fail (list, NULL);

	return g_list_nth_data (list->list, n);
}

/**
 * oseaclient_list_length:
 * @list: an OseaClientList.
 * 
 * Gets the number of elements in an OseaClientList.
 * 
 * Return value: the number of elements in the OseaClientList.
 **/
guint       oseaclient_list_length (OseaClientList *list)
{
	g_return_val_if_fail (list, -1);

	return g_list_length (list->list);
}

/**
 * oseaclient_list_remove:
 * @list: an OseaClientList.
 * @key: the key of the element to remove.
 * 
 * Removes an element from an OseaClientList. If two elements have the
 * same key, only the first is removed. If none of the elements
 * have the given key, the OseaClientList is unchanged.
 **/
void        oseaclient_list_remove (OseaClientList *list, gpointer key)
{
	gint index;

	g_return_if_fail (list);
	g_return_if_fail (key);
	
	list->list = g_list_first (list->list);
	list->id_list = g_list_first (list->id_list);

	index = oseaclient_list_index (list, key);
	if (index == -1)
		return;

	if (list->destroy_func)
		(list->destroy_func) (g_list_nth_data (list->list, index));
	
	list->list = g_list_delete_link (list->list, g_list_nth(list->list, index));
						     
	if (list->key_destroy_func)
		(list->key_destroy_func) (g_list_nth_data (list->id_list, index));

	list->id_list = g_list_delete_link (list->id_list, g_list_nth(list->id_list, index));

	list->cursor = list->list;
	list->cursor_position = 0;

	return;
}


/**
 * oseaclient_list_foreach:
 * @list: 
 * @func: 
 * @user_data: 
 * 
 * 
 **/
void        oseaclient_list_foreach (OseaClientList *list, GTraverseFunc func, gpointer user_data)
{


	GList * key_cursor;
	GList * cursor;

	g_return_if_fail (list);

	key_cursor = g_list_first (list->id_list);
	cursor     = g_list_first (list->list);
	
	while (key_cursor) {

		if (func (key_cursor->data, cursor->data, user_data)) {
			return;			
		}

		cursor = cursor->next;
		key_cursor = key_cursor->next;
	}

}


/**
 * oseaclient_list_replace:
 * @list: an OseaClientList.
 * @key: the key to insert.
 * @new_value: the value corresponding to the key.
 * 
 * Inserts a new key and value into a OseaClientList similar to
 * oseaclient_list_insert(). The difference is that if the key already
 * exists in the OseaClientList, its value gets replaced by the new
 * value. If you supplied a value_destroy_func when creating the
 * OseaClientList, the old value is freed using that function. If you supplied
 * a key_destroy_func when creating the OseaClientList, the old key is freed
 * using that function.
 **/
void        oseaclient_list_replace (OseaClientList *list, gpointer key, gpointer new_value)
{


	gint    index;
	GList * cursor;
	
	g_return_if_fail (list);
	index = oseaclient_list_index (list, key);

	if (index == -1) {
		oseaclient_list_insert (list, key, new_value);
		return;
	}

	cursor = g_list_nth (list->list, index);
	if (list->destroy_func)
		(list->destroy_func) (cursor->data);
	cursor->data = new_value;

	cursor = g_list_nth (list->id_list, index);
	if (list->key_destroy_func)
		(list->key_destroy_func) (cursor->data);
	cursor->data = key;

	return;
	
}



/**
 * oseaclient_list_destroy:
 * @list: an OseaClientList.
 * 
 * Destroys the OseaClientList. If values are dynamically allocated, you
 * should either free them first or create the OseaClientList using
 * oseaclient_list_new_full(). In the latter case the destroy functions you
 * supplied will be called on all keys and values before destroying
 * the OseaClientList.
 **/
void        oseaclient_list_destroy (OseaClientList *list)
{

	gint i;
	g_return_if_fail (list);

	if (list->destroy_func)
		for (i = 0; i < g_list_length(list->list); i++)
			(list->destroy_func) (g_list_nth_data(list->list, i));
	if (list->key_destroy_func)
		for (i = 0; i < g_list_length(list->id_list); i++)
			(list->key_destroy_func) (g_list_nth_data(list->id_list, i));


	g_list_free (list->list);
	g_list_free (list->id_list);
	
	g_free (list);
	
}

void        oseaclient_list_first   (OseaClientList *list)
{
	g_return_if_fail (list);

	list->cursor = g_list_first (list->list);
	list->cursor_position = 0;
}

gpointer    oseaclient_list_data    (OseaClientList *list)
{
	g_return_val_if_fail (list, NULL);
	g_return_val_if_fail (list->cursor, NULL);

	return list->cursor->data;
}

gboolean    oseaclient_list_next    (OseaClientList *list) 
{
	g_return_val_if_fail (list, FALSE);

	if (g_list_next (list->cursor)) {
		list->cursor = g_list_next (list->cursor);
		list->cursor_position++;
		return TRUE;
	} else
		return FALSE;	
}

gboolean    oseaclient_list_set_cursor_position (OseaClientList *list, gint cursor_position)
{
	g_return_val_if_fail (list, FALSE);
	g_return_val_if_fail (cursor_position, FALSE);

	g_return_val_if_fail (cursor_position < g_list_length (list->list), FALSE);
	list->cursor = g_list_nth (list->list, cursor_position);
	list->cursor_position = cursor_position;
	return TRUE;
}


gint        oseaclient_list_get_cursor_position (OseaClientList *list)
{
	g_return_val_if_fail (list, -1);

	return list->cursor_position;
}
