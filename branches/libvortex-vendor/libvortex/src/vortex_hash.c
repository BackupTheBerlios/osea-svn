//  LibVortex:  A BEEP (RFC3080/RFC3081) implementation.
//  Copyright (C) 2005 Advanced Software Production Line, S.L.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation; either version 2.1 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free
//  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
//  02111-1307 USA
//  
//  You may find a copy of the license under this software is released
//  at COPYING file. This is LGPL software: you are wellcome to
//  develop propietary applications using this library withtout any
//  royalty or fee but returning back any change, improvement or
//  addition in the form of source code, project image, documentation
//  patches, etc. 
//
//  You can also contact us to negociate other license term you may be
//  interested different from LGPL. Contact us at:
//          
//      Postal address:
//         Advanced Software Production Line, S.L.
//         C/ Dr. Michavila Nº 14
//         Coslada 28820 Madrid
//         Spain
//
//      Email address:
//         info@aspl.es - http://fact.aspl.es
//
#include <vortex.h>


struct _VortexHash {
	GHashTable * table;
	GMutex     * mutex;
};

#define LOG_DOMAIN "vortex-hash"

/**
 * \defgroup vortex_hash VortexHash: Thread Safe Hash table used inside Vortex Library.
 */

/// \addtogroup vortex_hash
//@{


/**
 * @brief Creates a new VortexHash setting all functions.
 * 
 * Creates a new Vortex Hash Table. This types is really a GHashTable
 * wrapper but with some mutex to make this hash table
 * thread-safe. All vortex library is programed making heavy use of hash
 * tables so things can go pretty much faster.
 *
 * But this makes race condition to appear anywhere so, this type
 * allow vortex library to create critical section to all operation
 * that are applied to a hash table.
 *
 * @param hash_func
 * @param key_equal_func
 * @param key_destroy_func
 * @param value_destroy_func
 *
 * @return a new VortexHash table or NULL if fail
 **/
VortexHash * vortex_hash_new_full (GHashFunc hash_func,
				   GEqualFunc key_equal_func,
				   GDestroyNotify key_destroy_func,
				   GDestroyNotify value_destroy_func)
{
	VortexHash * result;

	result        = g_new0 (VortexHash, 1);
	result->table = g_hash_table_new_full (hash_func, 
					       key_equal_func,
					       key_destroy_func,
					       value_destroy_func);
	result->mutex = g_mutex_new ();
	
	return result;
	
}

/**
 * @brief Creates a new VortexHash without providing destroy function.
 * 
 * A vortex_hash_new_full version passing in as NULL key_destroy_func and 
 * value_destroy_func. 
 *
 * @param hash_func
 * @param key_equal_func
 * 
 * @return a newly created VortexHash or NULL if fails.
 **/
VortexHash * vortex_hash_new      (GHashFunc hash_func,
				   GEqualFunc key_equal_func)
{
	return vortex_hash_new_full (hash_func, key_equal_func, NULL, NULL);
}

/**
 * @brief Inserts a pair key/value inside the given VortexHash
 *
 * 
 * Insert a key/value pair into hash_table.
 *
 * @param hash_table: the hash table
 * @param key: the key to insert
 * @param value: the value to insert
 **/
void         vortex_hash_insert   (VortexHash *hash_table,
				   gpointer key,
				   gpointer value)
{
	g_return_if_fail (hash_table);
	g_mutex_lock   (hash_table->mutex);

	g_hash_table_insert (hash_table->table, key, value);

	g_mutex_unlock (hash_table->mutex);

	return;
}

/**
 * @brief Replace using the given pair key/value into the given hash.
 * 
 * Replace the key/value pair into hash_table. If previous value key/value
 * is not found then the pair is simply added.
 *
 * @param hash_table the hash table to operate on
 * @param key the key value
 * @param value the value to insert
 **/
void         vortex_hash_replace  (VortexHash *hash_table,
				   gpointer key,
				   gpointer value)
{
	g_return_if_fail (hash_table);
	g_mutex_lock   (hash_table->mutex);

	g_hash_table_replace (hash_table->table, key, value);

	g_mutex_unlock (hash_table->mutex);

	return;
}

/**
 * @brief Returns the current hash size.
 *
 * 
 * Returns the hash table size
 * 
 * @param hash_table the hash table to operate on.
 *
 * @return the hash table size or -1 if fails
 **/
gint     vortex_hash_size     (VortexHash *hash_table)
{
	gint result;

	g_return_val_if_fail (hash_table, -1);
	g_mutex_lock     (hash_table->mutex);

	result = g_hash_table_size (hash_table->table);

	g_mutex_unlock   (hash_table->mutex);	
	
	return result;
}

/**
 * @brief Perform a lookup using the given key inside the given hash.
 *
 * 
 * Return the value, if found, associated with the key.
 * 
 * @param hash_table the hash table
 * @param key the key value
 *
 * @return the value found or NULL if fails
 **/
gpointer     vortex_hash_lookup   (VortexHash *hash_table,
				   gconstpointer key)
{
	gpointer data;

	g_return_val_if_fail (hash_table, NULL);
	g_mutex_lock   (hash_table->mutex);
	
	data = g_hash_table_lookup (hash_table->table, key);

	g_mutex_unlock (hash_table->mutex);	

	return data;
}

/**
 * @brief Removes the value index by the given key inside the given hash.
 *
 * 
 * Remove a key/pair value from the hash
 * 
 * @param hash_table the hash table
 * @param key the key value to lookup and destroy
 *
 * @return TRUE if found and removed and FALSE if not removed
 **/
gboolean     vortex_hash_remove   (VortexHash *hash_table,
				   gconstpointer key)
{
	gboolean result;

	g_return_val_if_fail (hash_table, FALSE);

	g_mutex_lock   (hash_table->mutex);

	result = g_hash_table_remove (hash_table->table, key);
	
	g_mutex_unlock (hash_table->mutex);

	return result;
}

/**
 * @brief Destroy the given hash freeing all resources.
 * 
 * Destroy the hash table.
 *
 * @param hash_table the hash table to operate on.
 **/
void         vortex_hash_destroy  (VortexHash *hash_table)
{
	g_return_if_fail (hash_table);

	g_hash_table_destroy (hash_table->table);
	g_mutex_free         (hash_table->mutex);
	g_free               (hash_table);
	return;
}

/**
 * @brief Perform a foreach over all elements inside the VortexHash.
 * 
 * A foreach version of g_hash_table_foreach but thread safe.
 *
 * @param hash_table
 * @param func
 * @param user_data
 **/
void         vortex_hash_foreach  (VortexHash *hash_table,
				   GHFunc func,
				   gpointer user_data)
{
	g_return_if_fail (hash_table);
	g_mutex_lock   (hash_table->mutex);

	g_hash_table_foreach (hash_table->table, func, user_data);

	g_mutex_unlock (hash_table->mutex);

	return;
}

// @}
