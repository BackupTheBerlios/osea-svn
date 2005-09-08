#include <vortex.h>

#define PLAIN_PROFILE "http://fact.aspl.es/profiles/plain_profile"

void frame_received (VortexChannel    * channel,
		     VortexConnection * connection,
		     VortexFrame      * frame,
		     gpointer           user_data)
{
	g_print ("A frame received on channl: %d\n",     vortex_channel_get_number (channel));
	g_print ("Data received: '%s'\n",                vortex_frame_get_payload (frame));

	// reply the peer client with the same content
	vortex_channel_send_rpyv (channel,
				  vortex_frame_get_msgno (frame),
				  "Received Ok: %s",
				  vortex_frame_get_payload (frame));
				
	g_print ("VORTEX_LISTENER: end task (pid: %d)\n", getpid ());


	return;
}

gboolean start_channel (gint               channel_num, 
			VortexConnection * connection, 
			gpointer           user_data)
{
	// implement profile requirement for allowing starting a new channel

	// to return FALSE denies channel creation
	// to return TRUE allows create the channel
	return TRUE;
}

gboolean close_channel (gint               channel_num, 
			VortexConnection * connection, 
			gpointer           user_data)
{
	// implement profile requirement for allowing to closeing a the channel
	// to return FALSE denies channel closing
	// to return TRUE allows to close the channel
	return TRUE;
}

gint main (gint argc, gchar ** argv) 
{

	// init vortex library
	vortex_init ();

	// register a profile
	vortex_profiles_register (PLAIN_PROFILE, 
				  start_channel, NULL, 
				  close_channel, NULL,
				  frame_received, NULL);

	// create a vortex server
	vortex_listener_new ("0.0.0.0", "44000", NULL, NULL);

	// wait for listeners (until vortex_exit is called)
	vortex_listener_wait ();
	
	// end vortex function
	vortex_exit ();

	return 0;
}

