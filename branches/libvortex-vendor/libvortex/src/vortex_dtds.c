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

GStaticMutex     __channel_dtd_mutex  = G_STATIC_MUTEX_INIT;
static xmlDtdPtr channel_dtd          = NULL;

#define LOG_DOMAIN "vortex-dtds"

/** 
 * @internal
 * @brief Clean up Vortex DTD module. 
 * 
 */
void vortex_dtd_cleanup ()
{
	if (channel_dtd != NULL) {
		xmlFreeDtd (channel_dtd);
		channel_dtd = NULL;
	}
	return;
}

gchar  * find_data_file (gchar * name)
{
	gchar * dtd_file_name;
	// first lookup
#ifdef G_OS_UNIX
	dtd_file_name = g_build_filename (PACKAGE_DTD_DIR, "libvortex", "data", name, NULL );
	if (g_file_test (dtd_file_name, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "file found at: %s", dtd_file_name);
		return dtd_file_name;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to find file %s on %s",  name, dtd_file_name);
	g_free (dtd_file_name);

	// second lookup
	dtd_file_name = g_build_filename (PACKAGE_DTD_DIR, "libvortex",  name, NULL );
	if (g_file_test (dtd_file_name, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "file found at: %s", dtd_file_name);
		return dtd_file_name;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to find file %s on %s",  name, dtd_file_name);
	g_free (dtd_file_name);

	// third lookup
	dtd_file_name = g_build_filename ("libvortex", name, NULL );
	if (g_file_test (dtd_file_name, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "file found at: %s", dtd_file_name);
		return dtd_file_name;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to find file %s on %s",  name, dtd_file_name);
	g_free (dtd_file_name);

	// fourth lookup
	dtd_file_name = g_build_filename (PACKAGE_TOP_DIR, "libvortex", "data", name, NULL);
	if (g_file_test (dtd_file_name, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "file found at: %s", dtd_file_name);
		return dtd_file_name;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to find file %s on %s",  name, dtd_file_name);
	g_free (dtd_file_name);
#endif
	// fifth lookup
	dtd_file_name = g_build_filename (name, NULL);
	if (g_file_test (dtd_file_name, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "file found at: %s", dtd_file_name);
		return dtd_file_name;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "unable to find file %s on %s",  name, dtd_file_name);

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "file not found at any place %s", name);
	g_free (dtd_file_name);
	return NULL;	
}

gboolean vortex_dtds_load_channel_dtd () 
{
	gchar * dtd_file_name;

	dtd_file_name = find_data_file ("channel.dtd");
	if (dtd_file_name == NULL)
		return FALSE;

	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "dtd file definition found at: %s", dtd_file_name);
	channel_dtd = xmlParseDTD (NULL, dtd_file_name);
	g_free (dtd_file_name);

	if (channel_dtd == NULL) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to parse dtd, this can be a problem");
		return FALSE;
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "dtd parsed..");
	return TRUE;
}

xmlDtdPtr vortex_dtds_get_channel_dtd ()
{
	// load DTD channel definition if not weren't yet
	// g_static_mutex_lock (& __channel_dtd_mutex);
	// if (channel_dtd == NULL) 
	// load_channel_dtd ();
	// g_static_mutex_unlock (& __channel_dtd_mutex);
	return channel_dtd;
}


