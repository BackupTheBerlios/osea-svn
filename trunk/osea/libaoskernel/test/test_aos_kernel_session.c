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

#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <src/aos_kernel.h>
#include <coyote/coyote.h>

#define MODULE_TEST_NAME "aos_kernel_session"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;


gboolean wait = TRUE;

// Functions that will be called by afdalkernel library
// when a response arrived.
gboolean handler_kernel_login (gpointer data, gpointer usr_data)
{
	g_print ("Function executed\n");
	wait = FALSE;
	return TRUE;
}

gboolean test1 (void) 
{
	GError * error = NULL;
	RRConnection * connection = NULL;

	// Initialize coyote 
	if (!coyote_init (0, 0, &error)) {
		g_printerr ("Error: unable to initialize coyote: %s\n", error->message);
		return FALSE;
	}
	

	// Create a connection to our server
	connection = coyote_connection_new ("localhost", "55000", TYPE_COYOTE_SIMPLE);
	if (!connection) {
		g_printerr ("Error: there was a problem while connecting to server\n");
		return FALSE;
	}
	
	// Ask for login to our server
	if (!aos_kernel_session_login ("pepe", "tonto", connection , handler_kernel_login, NULL)) {
		g_printerr ("Error: there was an error while tryng to login\n");
		return FALSE;
	}
	
	// Waits until a response arrives.
	g_printerr ("Waiting.\n");
	while (wait) {
		g_printerr (".");
		sleep (1);
	}
	g_printerr (".\n");
	
	return TRUE;
}




TestNode test_suite []  = { 
	{"aos_kernel_login", test1}, 
	{NULL, NULL}};

/**
 * main:
 * @argc: 
 * @argv: 
 * 
 * This function will test all the functions defined at test_suite array. 
 * DO NOT TOUCH THIS FUNCTION
 * 
 * Return value: 0 if everything was right or -1 if not.
 **/
int main(int argc, char **argv) 
{

	gint i;

	for (i = 0; test_suite[i].test_description != NULL; i++) {
	
		if (test_suite[i].test_function ()) 
			g_print (" [  OK  ] [%s]: Testing: %s \n", MODULE_TEST_NAME, test_suite[i].test_description);
		else {
			g_print (" [ FAIL ] [%s]: Testing: %s \n", MODULE_TEST_NAME, test_suite[i].test_description);
			return FALSE;
		}
	}
	
	return TRUE;

}








