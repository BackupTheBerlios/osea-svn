#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <signal.h>
#include <src/oseacomm.h>

#define MODULE_TEST_NAME "oseacomm_connection"

typedef struct _TestNode {
	gchar * test_description;
	gboolean (*test_function) (void);
}TestNode;


// Fuctions defining the test-listening-server


gboolean server_init (RRChannel *channel,  const gchar *piggyback, GError **error)
{
	// initialize the config for the particular instance of the channel
	channel->instance_config = oseacomm_simple_cfg_new ();

	rr_channel_set_aggregate (channel, FALSE);

	return TRUE;
}


gint launch_server (gchar *port) {
	
	gint argc = 1;
	gchar *argv[] = {"./server"};

	GError * error = NULL;
	RRListener * server;
	RRProfileRegistry * beep_profile;
	OseaCommSimpleCfg * config;
	
	// Initialize road runner
	if (!oseacomm_init (&argc, (gchar ***) &argv, &error)) 
		g_error ("Error: failed to call oseacomm_init: %s\n", error->message);
	
	// Create the profile registry that will hold our profiles.
	beep_profile = rr_profile_registry_new ();

	// Create a config object
	config = oseacomm_simple_cfg_new ();
	
	oseacomm_simple_cfg_set_server_init (config, server_init, NULL);
		
	// Add to the supported profiles oseacomm_simple
	rr_profile_registry_add_profile (beep_profile, TYPE_OSEACOMM_SIMPLE, config);  
	
	// Start the server
	server = rr_tcp_listener_new (beep_profile, "localhost", atoi(port), &error);
	if (!server) {
		g_error ("Error while starting the server  %s\n", error->message);
	}
	
	if (!rr_wait_until_done (&error)) {
		g_error ("Error: unexpected error %s\n", error->message);
	}
	
	if (!rr_listener_shutdown (server, &error)) {
		g_error ("Error: shutting down error: %s\n", error->message);
	}
	
	if (!rr_exit (&error))
		g_error ("Error: unexpected error while exitting from rr_init: %s\n", error->message);
	
	exit (0);
}


/**
 * test1:
 * 
 * This test will create a listening server and will try to establish
 * a connection
 * 
 **/
gboolean test1 (void)
{
	gint argc = 1;
	gchar *argv[] = {"./test_client"};
	GError *error = NULL;
	RRConnection * connection = NULL;
	pid_t id;
	gchar *port = "55500";	

	id = fork();

	if (!id)
		launch_server(port);

	if (!oseacomm_init (&argc, (gchar ***) &argv, &error)) 
		return FALSE;
	
	connection = oseacomm_connection_new ("localhost", port, TYPE_OSEACOMM_SIMPLE);

	if (!connection) {
		kill (id, SIGKILL);
		return FALSE;
	} else {
		kill (id, SIGKILL);
		return TRUE;
	}
}


TestNode test_suite []  = {{"oseacomm_connection_new", test1},{NULL, NULL}};

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
			exit (-1);
		}
	}
	
	exit (0);

}

