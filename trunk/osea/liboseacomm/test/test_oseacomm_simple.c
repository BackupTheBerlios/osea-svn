#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
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
	return TRUE;
}


gint launch_server (gchar *port, OseaCommSimpleCfg *config) {
	
	gint argc = 1;
	gchar *argv[] = {"./server"};

	GError * error = NULL;
	RRListener * server;
	RRProfileRegistry * beep_profile;
	
	// Initialize road runner
	if (!oseacomm_init (&argc, (gchar ***) &argv, &error)) 
		g_error ("Error: failed to call oseacomm_init: %s\n", error->message);
	
	// Create the profile registry that will hold our profiles.
	beep_profile = rr_profile_registry_new ();

	// Create a config object
	if (!config) {
		// If there is no config as a param, we create one
		config = oseacomm_simple_cfg_new ();
		oseacomm_simple_cfg_set_server_init (config, server_init, NULL);
	}
		
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
 * This test checks "oseacomm_simple_start"
 * 
 * This test will create a listening server and will try to establish
 * a connection and create a channel
 *
 **/
gboolean test_simple_start (void)
{
	gint argc = 1;
	gchar *argv[] = {"./test_client"};
	GError *error = NULL;
	RRConnection * connection = NULL;
	OseaCommSimple * oseacomm_simple = NULL;
	OseaCommSimpleCfg * oseacomm_simple_cfg = NULL;
	int status = 0;
	pid_t id;
	gchar *port = "55500";	

	id = fork();


	sleep (1);

	if (!oseacomm_init (&argc, (gchar ***) &argv, &error)) 
		return FALSE;
	
	connection = oseacomm_connection_new ("localhost", port, TYPE_OSEACOMM_SIMPLE);
	if (! connection) {
		kill (id, SIGKILL);
		waitpid (id, &status, 0);
		oseacomm_exit (&error);
		return FALSE;
	} else {

		oseacomm_simple_cfg = oseacomm_simple_cfg_new ();
		oseacomm_simple = oseacomm_simple_start (connection, 
						     oseacomm_simple_cfg,
						     &error);
		if (! oseacomm_simple) {
			kill (id, SIGKILL);
			waitpid (id, &status, 0);
			oseacomm_exit (&error);
			return FALSE;
		} else {			
			kill (id, SIGKILL);
			waitpid (id, &status, 0);
			oseacomm_exit (&error);
			return TRUE;
		}
	}
}


/**
 * test2:
 * This check tests "oseacomm_simple_stop"
 *
 * 
 * This test will create a listening server and will try to establish
 * a connection, creating a channel and then closing it
 * 
 **/
gboolean test_simple_stop (void)
{
	gint argc = 1;
	gchar *argv[] = {"./test_client"};
	GError *error = NULL;
	RRConnection * connection = NULL;
	OseaCommSimple * channel = NULL;
	OseaCommSimpleCfg * oseacomm_simple_cfg = NULL;
	int status = 0;
	pid_t id;
	gchar *port = "55501";	

	id = fork();

	if (!id) {
		launch_server(port, NULL);
		exit;
	}

	sleep (1);
	
	if (!oseacomm_init (&argc, (gchar ***) &argv, &error)) 
		return FALSE;
	
	connection = oseacomm_connection_new ("localhost", port, TYPE_OSEACOMM_SIMPLE);

	if (!connection) {
		kill (id, SIGKILL);
		waitpid (id, &status, 0);
		oseacomm_exit (&error);
		return FALSE;
	} else {
		
		oseacomm_simple_cfg = oseacomm_simple_cfg_new ();

		channel = oseacomm_simple_start (connection, 
						     oseacomm_simple_cfg,
						     &error);

		if (! channel) {
			kill (id, SIGKILL);
			waitpid (id, &status, 0);
			oseacomm_exit (&error);
			return FALSE;
		} else {
			if (oseacomm_simple_close (channel, &error)) {
				kill (id, SIGKILL);
				waitpid (id, &status, 0);
				oseacomm_exit (&error);
				return TRUE;
			} else {
				kill (id, SIGKILL);
				waitpid (id, &status, 0);
				oseacomm_exit (&error);
				return FALSE;
			}
		}
	}
}







gboolean test_simple_cfg_server_init_callback (RRChannel *channel,  const gchar *piggyback, GError **error)
{
	printf ("     In server init callback\n");

	return TRUE;
}

/**
 * test_simple_cfg_server_init:
 * This check tests "oseacomm_simple_cfg_set_server_init"
 *
 * 
 * This test will create a listening server and will try to establish
 * a connection, creating a channel and then closing it
 * 
 **/
gboolean test_simple_cfg_server_init (void)
{
	gint argc = 1;
	gchar *argv[] = {"./test_client"};
	GError *error = NULL;
	RRConnection * connection = NULL;
	OseaCommSimple * channel = NULL;
	OseaCommSimpleCfg * oseacomm_simple_cfg = NULL;
	pid_t id;
	gchar *port = "55503";	


	// Creating a configuration
	oseacomm_simple_cfg = oseacomm_simple_cfg_new ();
	oseacomm_simple_cfg_set_server_init (oseacomm_simple_cfg, test_simple_cfg_server_init_callback, NULL);


	id = fork();

	if (!id) {
		launch_server (port, oseacomm_simple_cfg);
		exit;
	}

	sleep (1);

	if (!oseacomm_init (&argc, (gchar ***) &argv, &error)) 
		return FALSE;
	
	connection = oseacomm_connection_new ("localhost", port, TYPE_OSEACOMM_SIMPLE);

	if (!connection) {
		kill (id, SIGKILL);
		return FALSE;
	} else {
		channel = oseacomm_simple_start (connection, 
					       oseacomm_simple_cfg,
					       &error);
		
		if (! channel) {
			kill (id, SIGKILL);
			return FALSE;
		} else {
			oseacomm_simple_close (channel, &error);
			kill (id, SIGKILL);
			return TRUE;
		}
	}
}


TestNode test_suite []  = {{"oseacomm_simple_start", test_simple_start},
                           {"oseacomm_simple_close", test_simple_stop}, {NULL, NULL}};

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

