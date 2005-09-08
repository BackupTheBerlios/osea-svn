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

#define LOG_DOMAIN "vortex-thread-pool"

GThreadPool * __vortex_thread_pool;

// vortex thread pool struct used by vortex library to notify to tasks
// to be performed to vortex thread pool
typedef struct _VortexThreadPoolTask {
	GThreadFunc   func;
	gpointer      data;
}VortexThreadPoolTask;

/**
 * __vortex_thread_pool_dispatcher:
 * @data: 
 * @user_data: 
 * 
 * This helper function dispatch the work to the right handler
 **/
void __vortex_thread_pool_dispatcher (gpointer data,
				      gpointer user_data)
{
	VortexThreadPoolTask * task        = data;

	// at this point we already are executing inside a thread
	task->func (task->data);
	g_free (task);
		
	// That's all!
}

/**
 * \defgroup vortex_thread_pool Vortex Thread Pool: Pool of threads which runns user defined async notifications.
 */

/// \addtogroup vortex_thread_pool
//@{


/**
 * @brief Init the Vortex Thread Pool subsystem.
 * 
 * Initializes the vortex thread pool. This pool is mainly used to
 * invoke frame receive handler at any level. Because we have allways
 * running threads to send messages (the vortex writer and sequencer)
 * this pool is not needed. 
 *
 * Among the vortex features are listed to create connection, channels
 * and also close channel in a asynchronous way. This means the those
 * process are run inside a separate thread. All those process are
 * also run inside the thread pool.
 *
 * @param max_threads how many threads to start.
 *
 **/
void vortex_thread_pool_init     (gint max_threads)
{
	GError * error = NULL;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "creating thread pool threads=%d", max_threads);

	__vortex_thread_pool =   g_thread_pool_new  (__vortex_thread_pool_dispatcher,
						     NULL,
						     max_threads,
						     TRUE,
						     &error);
	if (error != NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to create thread pool, error was: %s",
		       error->message);
		exit (-1);
	}
	
	return;
}

/**
 * @brief Queue a new task inside the VortexThreadPool.
 *
 * 
 * Queue a new task to be performed. This function is used by vortex
 * for internal purpose so it should not be useful for vortex library
 * consumers.
 *
 * @param func the function to execute.
 * @param data the data to be passed in to the function.
 *
 * 
 **/
void vortex_thread_pool_new_task (GThreadFunc func, gpointer data)
{
	GError * error = NULL;
	VortexThreadPoolTask * task;


	g_return_if_fail (func);
	g_return_if_fail (data);
	g_return_if_fail (__vortex_thread_pool);

	// create the task data
	task       = g_new0 (VortexThreadPoolTask, 1);
	task->func = func;
	task->data = data;

	g_thread_pool_push (__vortex_thread_pool, 
			    task,
			    &error);

	if (error != NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to create new task, error was: %s", 
		       error->message);
	}
}


/**
 * @brief Returns the running threads the given pool have.
 * 
 * Returns the actual running threads the vortex thread pool have.
 * 
 * @return the thread number or -1 if fails
 **/
gint vortex_thread_pool_get_running_threads ()
{

	g_return_val_if_fail (__vortex_thread_pool, -1);

	return g_thread_pool_get_num_threads (__vortex_thread_pool);
}

/**
 * @brief Returns how many threads have the given VortexThreadPool
 * 
 * This function is for internal vortex library purpose. This function
 * returns how many thread must be created inside the vortex thread
 * pool which actually is the one which dispatch all frame received
 * callback and other async notifications. 
 *
 * This function pay attention to the environment var <b>"VORTEX_THREADS"</b>
 * so user can control how many thread are created. In case this var
 * is not defined the function will return 5. In case the var is
 * defined but using a wrong value, that is, an non positive integer
 * the function will abort the vortex execution and consecuently the
 * application over it.
 * 
 * @return the number of threads to be created.
 **/
gint vortex_thread_pool_get_num             ()
{
	gint value;

	if (!g_getenv ("VORTEX_THREADS"))
		return 5;

	value = atoi (g_getenv ("VORTEX_THREADS"));
	if (value <= 0) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "VORTEX_THREAD environment var have a wrong value='%s', exiting", 
		       g_getenv ("VORTEX_THREADS"));
		exit (-1);
	}
	
	return value;
}

//@}
