//  ASPL Fact: invoicing client program for ASPL Fact System
//  Copyright (C) 2002, 2003  Advanced Software Production Line, S.L.
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


#include "event_source.h"
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

typedef struct __event_source_item { 
	GSource     * source;
	gchar       * name;
	gboolean      active;
	GSourceFunc   callback;
	gpointer      userdata;
} EventSourceItem;

static GStaticMutex   event_source_mutex = G_STATIC_MUTEX_INIT;
static GList * event_source_list = NULL;
static gint  transaction_number = 0;

static gboolean __event_prepare (GSource *source, gint *timeout)
{
	GList           * ptr = NULL;
	EventSourceItem * src_item;

	*timeout = -1;

	g_static_mutex_lock (&event_source_mutex);
	for ( ptr = g_list_first (event_source_list); ptr; ptr = g_list_next (ptr)) {

		src_item = (EventSourceItem *) ptr->data;

		if (src_item->source == source) {
			g_static_mutex_unlock (&event_source_mutex);
			return src_item->active;
		}
	}
	g_static_mutex_unlock (&event_source_mutex);

	return FALSE;
}

static gboolean __event_check (GSource *source)
{
	GList           * ptr = NULL;
	EventSourceItem * src_item;

	g_static_mutex_lock (&event_source_mutex);
	for ( ptr = g_list_first (event_source_list); ptr; ptr = g_list_next (ptr)) {
		src_item = (EventSourceItem *) ptr->data;
		if (src_item->source == source) {
			g_static_mutex_unlock (&event_source_mutex);
			return src_item->active;
		}
	}

	g_static_mutex_unlock (&event_source_mutex);

	return FALSE;
}

static gboolean __event_dispatch (GSource *source, GSourceFunc callback, gpointer data)
{
	GList           * ptr = NULL;
	EventSourceItem * src_item;

	g_static_mutex_lock (&event_source_mutex);

	for ( ptr = g_list_first (event_source_list); ptr; ptr = g_list_next (ptr)) {
		src_item = (EventSourceItem *) ptr->data;

		if (src_item->source == source) {
			event_source_list = g_list_remove (event_source_list, src_item);

			g_static_mutex_unlock (&event_source_mutex);
			
			((* (src_item->callback)) (src_item->userdata));
			
 			g_source_destroy (src_item->source);
			g_free (src_item->name);
			g_free (src_item);
			return TRUE;
		}
	}

	g_static_mutex_unlock (&event_source_mutex);

	return FALSE;
	
}

static void __event_finalize (GSource *source)
{
	return;
}


GSourceFuncs event_source_funcs = {
	__event_prepare,
	__event_check,
	__event_dispatch,
	__event_finalize
};

void event_source_add_signal (gchar *signal_name)
{
	EventSourceItem * source_item;
	GSource         * source;

	source = g_source_new (&event_source_funcs, sizeof(GSource));	

	source_item = g_new0 (EventSourceItem, 1);

	source_item -> source = source;
	source_item -> name = g_strdup(signal_name);
	source_item -> active = FALSE;

	g_static_mutex_lock (&event_source_mutex);
	event_source_list = g_list_append (event_source_list, source_item);
	g_static_mutex_unlock (&event_source_mutex);

	return;
}

guint event_source_set_callback (gchar *signal_name, GSourceFunc callback)
{
	GList           * ptr = NULL;
	EventSourceItem * src_item;

	g_static_mutex_lock (&event_source_mutex);
	for ( ptr = g_list_first (event_source_list); ptr; ptr = g_list_next (ptr)) {

		src_item = (EventSourceItem *) ptr->data;

		if (!strcmp (src_item->name, signal_name)) {

			src_item -> callback = callback;

			g_static_mutex_unlock (&event_source_mutex);

			if (g_source_get_context (src_item->source)) 
				return (g_source_get_id (src_item->source));
			
			return g_source_attach (src_item->source, NULL);		
			    
		}
	}
	g_static_mutex_unlock (&event_source_mutex);
	g_printerr( _("EVENT_SOURCE: there's no signal called %s\n"), signal_name);

	return 0;      
}

void event_source_emit_signal (gchar *signal_name, gpointer data)
{
	GList           * ptr = NULL;
	EventSourceItem * src_item;

	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, _("Emmiting signal '%s'"), signal_name);

	g_static_mutex_lock (&event_source_mutex);
	for ( ptr = g_list_first (event_source_list); ptr; ptr = g_list_next (ptr)) {

		src_item = (EventSourceItem *) ptr->data;

		if (!strcmp (src_item->name, signal_name)) {
			g_source_set_callback (src_item->source, src_item->callback, data, NULL);
			src_item->userdata = data;
			src_item->active = TRUE;
			g_static_mutex_unlock (&event_source_mutex);
			return;
		}
	}	
	g_static_mutex_unlock (&event_source_mutex);
	g_printerr( _("EVENT_SOURCE: there's no signal called %s\n"), signal_name);

	return;
}

void event_source_remove_signal (gchar *signal_name)
{
	// Skel function left to get compile compatibility.
	return;
}


gboolean event_source_exist (gchar *signal_name)
{
	GList           * ptr = NULL;
	EventSourceItem * src_item;

	g_static_mutex_lock (&event_source_mutex);
	for ( ptr = g_list_first (event_source_list); ptr; ptr = g_list_next (ptr)) {
		src_item = (EventSourceItem *) ptr->data;
		if (!strcmp (src_item->name, signal_name)) {
			g_static_mutex_unlock (&event_source_mutex);
			return TRUE;
		}
	}
	g_static_mutex_unlock (&event_source_mutex);

	return FALSE;

}

gchar  * event_source_arm_signal    (gchar * signal_basename, GSourceFunc func)
{
	gchar           * signal_name = g_strdup_printf ("group_transaction %d",  ++transaction_number);
	EventSourceItem * source_item;
	GSource         * source;

	source = g_source_new (&event_source_funcs, sizeof(GSource));	

	source_item = g_new0 (EventSourceItem, 1);

	source_item -> source = source;
	source_item -> name = g_strdup(signal_name);
	source_item -> active = FALSE;
	source_item -> callback = func;

	g_static_mutex_lock (&event_source_mutex);
	event_source_list = g_list_append (event_source_list, source_item);
	g_static_mutex_unlock (&event_source_mutex);

	if (g_source_get_context (source_item->source)) 
		return signal_name;
	g_source_attach (source_item->source, NULL);		
	return signal_name;
}

void    event_source_print_pool ()
{
#ifdef DEBUG
	GList           * ptr = NULL;
	EventSourceItem * src_item;

	g_print ("\n\nPrinting signal pool\n");
	g_static_mutex_lock (&event_source_mutex);
	for ( ptr = g_list_first (event_source_list); ptr; ptr = g_list_next (ptr)) {
		src_item = (EventSourceItem *) ptr->data;
		g_print ("%s\n", src_item->name);
	}
	g_static_mutex_unlock (&event_source_mutex);
#endif
}
