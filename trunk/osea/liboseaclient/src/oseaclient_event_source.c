//
//  LibAfdal: common functions to liboseaclient* level and architectural functions.
//  Copyright (C) 2003  Advanced Software Production Line, S.L.
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

#include "oseaclient_event_source.h"
#include <string.h>

#define LOG_DOMAIN "oseaclient_event_source"

typedef struct __oseaclient_event_source_item { 
	GSource *source;
	gchar *name;
	gboolean active;
	GSourceFunc callback;
	gpointer userdata;
} AfdalEventSourceItem;

static GList        * oseaclient_event_source_list = NULL;
static GStaticMutex   oseaclient_event_source_mutex = G_STATIC_MUTEX_INIT;
static gint           oseaclient_event_source_transaction_number = 0;

gboolean              __oseaclient_event_source_thread_is_active = FALSE;
GMainContext        * __oseaclient_event_source_main_context  = NULL;
static GStaticMutex   __oseaclient_event_source_thread_launch_mutex = G_STATIC_MUTEX_INIT;

static gboolean __oseaclient_event_prepare (GSource *source, gint *timeout)
{
	GList           * ptr = NULL;
	AfdalEventSourceItem * src_item;

	*timeout = -1;

	g_static_mutex_lock (&oseaclient_event_source_mutex);
	for ( ptr = g_list_first (oseaclient_event_source_list); ptr; ptr = g_list_next (ptr)) {

		src_item = (AfdalEventSourceItem *) ptr->data;

		if (src_item->source == source) {
			g_static_mutex_unlock (&oseaclient_event_source_mutex);
			return src_item->active;
		}
	}
	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	return FALSE;
}

static gboolean __oseaclient_event_check (GSource *source)
{
	GList           * ptr = NULL;
	AfdalEventSourceItem * src_item;

	g_static_mutex_lock (&oseaclient_event_source_mutex);
	for ( ptr = g_list_first (oseaclient_event_source_list); ptr; ptr = g_list_next (ptr)) {
		src_item = (AfdalEventSourceItem *) ptr->data;
		if (src_item->source == source) {
			g_static_mutex_unlock (&oseaclient_event_source_mutex);
			return src_item->active;
		}
	}

	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	return FALSE;
}

static gboolean __oseaclient_event_dispatch (GSource *source, GSourceFunc callback, gpointer data)
{
	GList           * ptr = NULL;
	AfdalEventSourceItem * src_item;

	g_static_mutex_lock (&oseaclient_event_source_mutex);

	for ( ptr = g_list_first (oseaclient_event_source_list); ptr; ptr = g_list_next (ptr)) {
		src_item = (AfdalEventSourceItem *) ptr->data;

		if (src_item->source == source) {
			oseaclient_event_source_list = g_list_remove (afdal_event_source_list, src_item);

			g_static_mutex_unlock (&oseaclient_event_source_mutex);
			
//			g_print ("Executing user callback\n");

			((* (src_item->callback)) (src_item->userdata));

//			g_print ("Removing gsource from our poll\n");

			
 			g_source_destroy (src_item->source);
			g_free (src_item->name);
			g_free (src_item);
			return TRUE;
		}
	}

	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	return FALSE;
	
}

static void __oseaclient_event_finalize (GSource *source)
{
	return;
}


GSourceFuncs oseaclient_event_source_funcs = {
	__oseaclient_event_prepare,
	__oseaclient_event_check,
	__oseaclient_event_dispatch,
	__oseaclient_event_finalize
};



gpointer __oseaclient_event_source_thread (gpointer main_context)
{
	GMainContext * mc = (GMainContext *) main_context;
	GMainLoop    * ml = g_main_loop_new (mc, TRUE);

	__oseaclient_event_source_thread_is_active = TRUE;

	g_static_mutex_unlock (&__oseaclient_event_source_thread_launch_mutex);

	g_main_loop_run (ml);

	return NULL;
}


void __oseaclient_event_source_launch_thread ()
{
	if (! __oseaclient_event_source_thread_is_active) {
		__oseaclient_event_source_main_context = g_main_context_new ();
		
		g_static_mutex_lock (&__oseaclient_event_source_thread_launch_mutex);

		g_thread_create (__oseaclient_event_source_thread, __afdal_event_source_main_context, FALSE, NULL);


		/* We lock until the thread is created and completely running with its own main_context loop */

		g_static_mutex_lock (&__oseaclient_event_source_thread_launch_mutex);
		g_static_mutex_unlock (&__oseaclient_event_source_thread_launch_mutex);
		
		
	}

}



/**
 * oseaclient_event_source_add_signal:
 * @signal_name: 
 * 
 * Adds the signal @signal_name to the list of user generated signals
 **/
void oseaclient_event_source_add_signal (gchar *signal_name)
{
	AfdalEventSourceItem * source_item;
	GSource         * source;

	source = g_source_new (&oseaclient_event_source_funcs, sizeof(GSource));	

	source_item = g_new0 (AfdalEventSourceItem, 1);

	source_item -> source = source;
	source_item -> name = g_strdup(signal_name);
	source_item -> active = FALSE;

	g_static_mutex_lock (&oseaclient_event_source_mutex);
	oseaclient_event_source_list = g_list_append (afdal_event_source_list, source_item);
	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	return;
}


/**
 * oseaclient_event_source_set_callback:
 * @signal_name: 
 * @callback: 
 * 
 * Associate the given @callback to the user programmed event
 * @signal_name (which must be previously created), and put it into
 * the events checked by the main loop.
 * 
 * Return value: 
 **/
guint oseaclient_event_source_set_callback (gchar *signal_name, GSourceFunc callback)
{
	GList           * ptr = NULL;
	AfdalEventSourceItem * src_item;

	g_static_mutex_lock (&oseaclient_event_source_mutex);
	for ( ptr = g_list_first (oseaclient_event_source_list); ptr; ptr = g_list_next (ptr)) {

		src_item = (AfdalEventSourceItem *) ptr->data;

		if (!strcmp (src_item->name, signal_name)) {

			src_item -> callback = callback;

			g_static_mutex_unlock (&oseaclient_event_source_mutex);

			if (g_source_get_context (src_item->source)) {
				g_static_mutex_unlock (&oseaclient_event_source_mutex);
				return (g_source_get_id (src_item->source));
			}

			g_static_mutex_unlock (&oseaclient_event_source_mutex);

			__oseaclient_event_source_launch_thread ();

			return g_source_attach (src_item->source, __oseaclient_event_source_main_context);		
			    
		}
	}
	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	return 0;      
}

/**
 * oseaclient_event_source_emit_signal:
 * @signal_name: 
 * @data: 
 * 
 * This function "launch" the user programmed event called
 * @signal_name, passing the pointer @data to its callback
 **/
void oseaclient_event_source_emit_signal (gchar *signal_name, gpointer data)
{
	GList           * ptr = NULL;
	AfdalEventSourceItem * src_item;

	g_static_mutex_lock (&oseaclient_event_source_mutex);
	for ( ptr = g_list_first (oseaclient_event_source_list); ptr; ptr = g_list_next (ptr)) {

		src_item = (AfdalEventSourceItem *) ptr->data;

		if (!strcmp (src_item->name, signal_name)) {
			g_source_set_callback (src_item->source, src_item->callback, data, NULL);
			src_item->userdata = data;
			src_item->active = TRUE;
			g_static_mutex_unlock (&oseaclient_event_source_mutex);
			return;
		}
	}	
	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	return;
}

/**
 * oseaclient_event_source_remove_signal:
 * @signal_name: 
 * 
 * Deprecated. Don't use in new implementations.
 **/
void oseaclient_event_source_remove_signal (gchar *signal_name)
{
	// Skel function left to get compile compatibility.
	return;
}


/**
 * oseaclient_event_source_exist:
 * @signal_name: 
 * 
 * 
 * 
 * Return value: TRUE if @signal_name is a valid user programmed event
 * name, FALSE otherwise.
 **/
gboolean oseaclient_event_source_exist (gchar *signal_name)
{
	GList           * ptr = NULL;
	AfdalEventSourceItem * src_item;

	g_static_mutex_lock (&oseaclient_event_source_mutex);
	for ( ptr = g_list_first (oseaclient_event_source_list); ptr; ptr = g_list_next (ptr)) {
		src_item = (AfdalEventSourceItem *) ptr->data;
		if (!strcmp (src_item->name, signal_name)) {
			g_static_mutex_unlock (&oseaclient_event_source_mutex);
			return TRUE;
		}
	}
	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	return FALSE;

}

/**
 * oseaclient_event_source_arm_signal:
 * @signal_basename: 
 * @func: 
 * 
 * This function simplifies the use of oseaclient_event, generating
 * automatically an unique event_name associated with callback @func
 * 
 * Return value: the name for this event (can be used 
 **/
gchar  * oseaclient_event_source_arm_signal    (gchar * signal_basename, GSourceFunc func)
{
	gchar           * signal_name = g_strdup_printf ("group_transaction %d",  ++oseaclient_event_source_transaction_number);
	AfdalEventSourceItem * source_item;
	GSource         * source;

	source = g_source_new (&oseaclient_event_source_funcs, sizeof(GSource));	

	source_item = g_new0 (AfdalEventSourceItem, 1);

	source_item -> source = source;
	source_item -> name = g_strdup(signal_name);
	source_item -> active = FALSE;
	source_item -> callback = func;

	g_static_mutex_lock (&oseaclient_event_source_mutex);
	oseaclient_event_source_list = g_list_append (afdal_event_source_list, source_item);
	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	if (g_source_get_context (source_item->source)) 
		return signal_name;

	__oseaclient_event_source_launch_thread ();

	g_source_attach (source_item->source,  __oseaclient_event_source_main_context);		
	return signal_name;
}


/**
 * oseaclient_event_source_launch:
 * @func: 
 * @data: 
 * 
 * Launch the given function with the given data after a while.
 **/
void    oseaclient_event_source_launch    (GSourceFunc func, gpointer data)
{
	gchar           * signal_name = g_strdup_printf ("group_transaction %d",  ++oseaclient_event_source_transaction_number);
	AfdalEventSourceItem * source_item;
	GSource         * source;

	source = g_source_new (&oseaclient_event_source_funcs, sizeof(GSource));	

	source_item = g_new0 (AfdalEventSourceItem, 1);

	source_item -> source = source;
	source_item -> name = g_strdup(signal_name);
	source_item -> active = FALSE;
	source_item -> callback = func;
	source_item -> userdata = data;
	source_item -> active = TRUE;

	g_static_mutex_lock (&oseaclient_event_source_mutex);
	oseaclient_event_source_list = g_list_append (afdal_event_source_list, source_item);
	g_static_mutex_unlock (&oseaclient_event_source_mutex);

	if (g_source_get_context (source_item->source)) 
		return;

	__oseaclient_event_source_launch_thread ();

	g_source_attach (source_item->source,  __oseaclient_event_source_main_context);		
	return;
}


/**
 * oseaclient_event_source_print_pool:
 * @: 
 * 
 * Print the current event source pool
 **/
void    oseaclient_event_source_print_pool ()
{
	GList           * ptr = NULL;
	AfdalEventSourceItem * src_item;

	g_print ("\n\nPrinting signal pool\n");
	g_static_mutex_lock (&oseaclient_event_source_mutex);
	for ( ptr = g_list_first (oseaclient_event_source_list); ptr; ptr = g_list_next (ptr)) {
		src_item = (AfdalEventSourceItem *) ptr->data;
		g_print ("%s\n", src_item->name);
	}
	g_static_mutex_unlock (&oseaclient_event_source_mutex);
}



