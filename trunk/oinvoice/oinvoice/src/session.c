//  ASPL Fact: invoicing client program for ASPL Fact System
//  Copyright (C) 2003 Advanced Software Production Line, S.L.
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

#include <glib.h>
#include <liboseacomm/oseacomm.h>
#include <aoskernel/aos_kernel.h>
#include "session.h"

#define LOG_DOMAIN "session"

RRConnection       * connection = NULL;
AosKernelSession * session = NULL;

AosKernelSession * session_get () 
{
	return session;
}

void session_set (AosKernelSession *new_session) 
{

	g_return_if_fail (new_session);

	session = new_session;

	return;
}

RRConnection * session_connection_get ()
{
	return connection;
}

void session_connection_set (RRConnection *new_connection)
{
	g_return_if_fail (new_connection);
	
	connection = new_connection;

	return;
}

void session_free ()
{
	GError * error = NULL;
	// Free the already opened connection to os-tax
	if (!rr_connection_disconnect (connection, &error)) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "failed: %s\n",  error->message);
		g_error_free (error);
	}
	connection = NULL;

	// Free AosKernelSession
	aos_kernel_session_destroy (session);
	
	session = NULL;
	
	return;
}
