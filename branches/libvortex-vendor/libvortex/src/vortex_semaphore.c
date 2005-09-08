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

struct _VortexSemaphore {
	gint     state;
	GMutex * mutex;
	GCond  * cond;
};

/**
 * \defgroup vortex_semaphore VortexSemaphore: Thread Safe Semaphore used inside Vortex Library.
 */

/// \addtogroup vortex_semaphore
//@{

/**
 * @brief Creates an new VortexSemaphore.
 * 
 * This structure is mainly used to synchronize the vortex sequencer
 * process and the vortex writer process. This behaves pretty much
 * like the posix semaphores but builded using glib mutex, conditional
 * plus an state value.
 * 
 * @return the new semaphore
 **/
VortexSemaphore * vortex_semaphore_new ()
{
	VortexSemaphore * sem;

	sem        = g_new0 (VortexSemaphore, 1);
	sem->mutex = g_mutex_new ();
	sem->cond  = g_cond_new ();

	return sem;
}

/**
 * @brief Gets the current semaphore status.
 * 
 * Returns actual semaphore status. 
 *
 * @param sem the semaphore to check the status.
 * 
 * @return semaphore status or -1 if fails
 **/
gint              vortex_semaphore_get_status (VortexSemaphore * sem)
{
	gint result;
	g_return_val_if_fail (sem, -1);

	g_mutex_lock (sem->mutex);
	result = sem->state;
	g_mutex_unlock (sem->mutex);

	return result;
}

/**
 * @brief Blocks the caller until the semaphore gets into a positive value.
 * 
 * Calling this function will block if semaphore state is equal to or
 * less than 0. Once the semaphore becomes to a positive state, that
 * is, over 0, the caller will be unblocked
 *
 * @param sem the semaphore to operate on
 **/
void              vortex_semaphore_wait    (VortexSemaphore * sem)
{
	g_return_if_fail (sem);

	g_mutex_lock   (sem->mutex);

	while (sem->state <= 0)
		g_cond_wait (sem->cond, sem->mutex);

	g_mutex_unlock (sem->mutex);

	return;
}

/**
 * @brief Reduces the internal semaphore state.
 * 
 * Decreate <i>times</i> the semaphore state. This function is somewhat
 * especial because it will not block the caller the sem goes down
 * 0. If you want to get blocked if semaphore goes under 0 you must
 * call \ref vortex_semaphore_wait after calling this function. 
 *
 * @param sem the semaphore to operate
 * @param times how many times to down the semaphore
 **/
void              vortex_semaphore_down_n (VortexSemaphore * sem, gint times)
{
	g_return_if_fail (sem);

	g_mutex_lock   (sem->mutex);

	sem->state -= times;
	
	g_mutex_unlock (sem->mutex);
	
	return;
}

/**
 * @brief Increases the internal semaphore state.
 * 
 * Increase the semaphore state one unit. This function never block
 * the caller.
 *
 * @param sem the semaphore to operate on.
 **/
void              vortex_semaphore_up   (VortexSemaphore * sem)
{
	g_return_if_fail (sem);

	g_mutex_lock   (sem->mutex);

	sem->state++;
	g_cond_broadcast (sem->cond);

	g_mutex_unlock (sem->mutex);

	return;
	
}

/**
 * @brief Destroy the given semaphore.
 *
 * Destroy the semaphore.
 *
 * @param sem the semaphore to destroy.
 **/
void              vortex_semaphore_destroy (VortexSemaphore * sem)
{
	g_return_if_fail (sem);

	g_mutex_free (sem->mutex);
	g_cond_free  (sem->cond);
	g_free       (sem);

	return;
}


// @}
