//  AfdalKernel: test suite
//  Copyright (C) 2002  Advanced Software Production Line, S.L.
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

#include <src/aos_kernel.h>
#include <coyote/coyote.h>
#include <glib.h>
#include <unistd.h>


gboolean wait = TRUE;

// Functions that will be called by afdalkernel library
// when a response arrived.
gboolean handler_kernel_login (gpointer data, gpointer usr_data)
{
	g_print ("Function executed\n");
	wait = FALSE;
	return TRUE;
}

int main (int argc, gchar ** argv) 
{
	GError * error = NULL;
	RRConnection * connection = NULL;


	// Initialize coyote 
	if (!coyote_init (&argc, &argv, &error)) 
		g_print ("Error: unable to initialize coyote: %s\n", error->message);

	// Create a connection to our server
	connection = coyote_connection_new ("localhost", "55000", TYPE_COYOTE_SIMPLE);
	if (!connection) {
		g_print ("Error: there was a problem while connecting to server\n");
		return -1;
	}
	
	// Ask for login to our server
	if (!aos_kernel_login ("pepe", "tonto", connection , handler_kernel_login, NULL)) {
		g_print ("Error: there was an error while tryng to login\n");
	}
	
	// Waits until a response arrives.
	g_print ("Waiting.\n");
	while (wait) {
		g_print (".");
		sleep (1);
	}
	g_print (".\n");
	
	return 0;
}



