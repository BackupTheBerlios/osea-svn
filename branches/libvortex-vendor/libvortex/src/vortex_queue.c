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

struct _VortexQueue {
	GMutex  * mutex;
	GQueue  * queue;
};

/**
 * \defgroup vortex_queue Vortex Queue: Thread safe Queue definition based on GLib Queue used across Vortex Library
 */

/// \addtogroup vortex_queue
//@{


/**
 * @brief Creates a new \ref VortexQueue object.
 * 
 * \ref VortexQueue is a thread-safe wrapper to GQueue GList type. All function that \ref vortex_queue
 * module provides can also be found and behaves the same way that GQueue but in a thread-safe fashion. 
 * 
 * Refer to GQueue documentation.
 *
 * @return a new \ref VortexQueue object.
 **/
VortexQueue * vortex_queue_new           ()
{
	VortexQueue * result;

	result        = g_new0 (VortexQueue, 1);
	result->mutex = g_mutex_new ();
	result->queue = g_queue_new ();

	return result;
}

/**
 * @brief Retuns the emptiness status for the given queue.
 *
 * 
 *
 * @param queue The queue to check.
 * 
 * @return TRUE if the \ref VortexQueue is empty, FALSE if not.
 **/
gboolean      vortex_queue_is_empty      (VortexQueue * queue)
{
	gboolean result;

	g_return_val_if_fail (queue, FALSE);

	g_mutex_lock (queue->mutex);

	result = g_queue_is_empty (queue->queue);

	g_mutex_unlock (queue->mutex);

	return result;
	
}

/**
 * @brief Returns how many items the given queue have.
 * 
 *
 * @param queue the queue to check.
 * @return Returns how many items the given queue have or -1 if it fails.
 **/
guint         vortex_queue_get_length    (VortexQueue * queue)
{
	guint result;

	g_return_val_if_fail (queue, -1);

	g_mutex_lock (queue->mutex);

	result = g_queue_get_length (queue->queue);

	g_mutex_unlock (queue->mutex);

	return result;
}

/**
 * @brief Queues new data inside the given queue. 
 * 
 * push data at the queue's tail.
 *
 * @param queue the queue to use.
 * @param data user defined data to queue.
 * 
 **/
void          vortex_queue_push          (VortexQueue * queue, gpointer data)
{
	g_return_if_fail (queue);
	g_return_if_fail (data);

	g_mutex_lock      (queue->mutex);

	g_queue_push_tail (queue->queue, data);

	g_mutex_unlock    (queue->mutex);

	return;
}

/**
 * @brief Extracts queue data from its header.
 * 
 * pop data at the queue's head.
 *
 * @param queue the queue to operate.
 * 
 * @return the next item on the Queue's header of NULL if fails.
 **/
gpointer      vortex_queue_pop           (VortexQueue * queue)
{
	gpointer result;

	g_return_val_if_fail (queue, NULL);

	g_mutex_lock   (queue->mutex);

	result = g_queue_pop_head (queue->queue);

	g_mutex_unlock (queue->mutex);

	return result;
}

/**
 * @brief Retuns a reference without poping the item from the queue.
 * 
 * Returns next value to be poped or NULL if no data is found.
 *
 * @param queue the queue to operate
 * 
 * @return a reference for the item on the Queue's header or NULL if fails.
 **/
gpointer      vortex_queue_peek          (VortexQueue * queue)
{
	gpointer result;

	g_return_val_if_fail (queue, NULL);

	g_mutex_lock   (queue->mutex);

	result = g_queue_peek_head (queue->queue);

	g_mutex_unlock (queue->mutex);

	return result;
}

/**
 * @brief Frees the \ref VortexQueue.
 *
 * @param queue the queue to free.
 * 
 **/
void          vortex_queue_free          (VortexQueue * queue)
{
	g_return_if_fail (queue);
	if (vortex_queue_get_length (queue) != 0) {
		g_critical ("freeing a vortex queue which already have content: elements %d",
			    vortex_queue_get_length (queue));
		return;
	}

	g_mutex_free (queue->mutex);
	g_queue_free (queue->queue);

	g_free (queue);
	return;
}

// @}
