//  ASPL Fact: invoicing client program for ASPL Fact System
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

#include "event_source.h"
#include "event_process.h"
#include "osea_datalist.h"
#include <gnome.h>



gboolean event_process_oseaclient_multidata (OseaClientMultiData * data, gpointer datalist_aux)
{
	AsplDataList * datalist = (AsplDataList *) datalist_aux;
	gchar        * signal   = (gchar *) osea_datalist_get (datalist, "SIGNAL");
		
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       _("Entering event_process_oseaclient_multidata function with signal '%s'\n"), signal);

	osea_datalist_set (datalist, "DATA", data);

	event_source_print_pool ();

	if (event_source_exist (signal)) {
		event_source_emit_signal (signal, datalist);
		return TRUE;
	} 

	oseaclient_multi_free (data, TRUE);

	osea_datalist_free (datalist);

	event_source_remove_signal (signal);

	return FALSE;
	
}

gboolean event_process_oseaclient_data (OseaClientData * data, gpointer datalist_aux)
{
	AsplDataList * datalist = (AsplDataList *) datalist_aux;
	gchar        * signal   = (gchar *) osea_datalist_get (datalist, "SIGNAL");
		
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       _("Entering event_process_oseaclient_data function with signal '%s'\n"), signal);

	osea_datalist_set (datalist, "DATA", data);

	event_source_print_pool ();

	if (event_source_exist (signal)) {
		event_source_emit_signal (signal, datalist);
		return TRUE;
	} 

	oseaclient_data_free (data, TRUE);

	osea_datalist_free (datalist);

	event_source_remove_signal (signal);

	return FALSE;
	
}

gboolean event_process_oseaclient_simpledata (OseaClientSimpleData * data, gpointer datalist_aux)
{
	AsplDataList * datalist = (AsplDataList *) datalist_aux;
	gchar * signal   = (gchar *) osea_datalist_get (datalist, "SIGNAL");
		
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       _("Entering event_process_oseaclient_simpledata function with signal '%s'\n"), signal);

	osea_datalist_set (datalist, "DATA", data);

	event_source_print_pool ();

	if (event_source_exist (signal)) {
		event_source_emit_signal (signal, datalist);
		return TRUE;
	}else {
		g_critical ("Signal '%s' does not exit\n", signal);
	} 

	oseaclient_simple_free (data);

	osea_datalist_free (datalist);

	event_source_remove_signal (signal);

	return FALSE;
	
}

gboolean event_process_oseaclient_nuldata (OseaClientNulData * data, gpointer datalist_aux)
{
	AsplDataList * datalist = (AsplDataList *) datalist_aux;
	gchar * signal   = (gchar *) osea_datalist_get (datalist, "SIGNAL");
		
	g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, 
	       _("Entering event_process_oseaclient_nuldata function with signal '%s'\n"), signal);

	osea_datalist_set (datalist, "DATA", data);

	event_source_print_pool ();

	if (event_source_exist (signal)) {
		event_source_emit_signal (signal, datalist);
		return TRUE;
	}else {
		g_critical ("Signal '%s' does not exit\n", signal);
	}

	oseaclient_nul_free (data);

	osea_datalist_free (datalist);

	event_source_remove_signal (signal);

	return FALSE;
	
}
