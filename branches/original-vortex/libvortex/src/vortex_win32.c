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
//         C/ Dr. Michavila N� 14
//         Coslada 28820 Madrid
//         Spain
//
//      Email address:
//         info@aspl.es - http://fact.aspl.es
//
#include <vortex.h>

#define LOG_DOMAIN "vortex-win32"

void vortex_win32_init ()
{
	WORD wVersionRequested; 
	WSADATA wsaData; 
	int error; 
	
	wVersionRequested = MAKEWORD( 2, 2 ); 
	
	error = WSAStartup( wVersionRequested, &wsaData ); 
	if (error != NO_ERROR) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "unable to init winsock api, exiting..");
		
		exit (0);
	}
	g_log (LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "winsock initialization ok");
	return;
}

BOOL APIENTRY DllMain (HINSTANCE hInst,
                       DWORD reason,
                       LPVOID reserved)
{
 
    // allways returns TRUE because vortex init is done throught vortex_init
    return TRUE;
}

gboolean vortex_win32_nonblocking_enable (gint socket)
{
	unsigned long enable=1;

	return (ioctlsocket (socket, FIONBIO, &enable) == 0);
}

gboolean vortex_win32_blocking_enable (gint socket)
{
    unsigned long active=1;

	return (ioctlsocket (socket, FIONBIO, &active) == 0);
}
