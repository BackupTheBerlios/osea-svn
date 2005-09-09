/* LibShieldVortex: TLS and SASL profiles for Vortex Library
   Copyright (c) 2005 David Marín Carreño <davefx@davefx.is-a-geek.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "shieldvortex.h"
#define TLS_PROFILE "http://iana.org/beep/TLS"

void __shieldvortex_tls_frame_received (VortexChannel    * channel,
					VortexConnection * connection,
					VortexFrame      * frame,
					gpointer           user_data)
{
        g_print ("A frame received on channel: %d\n",     
		 vortex_channel_get_number (channel));
        g_print ("Data received: '%s'\n",                
		 vortex_frame_get_payload (frame));

        // reply the peer client with the same content
        vortex_channel_send_rpyv (channel,
                                  vortex_frame_get_msgno (frame),
                                  "Received Ok: %s",
                                  vortex_frame_get_payload (frame));
                                
        g_print ("VORTEX_LISTENER: end task (pid: %d)\n", getpid ());


        return;
}

gboolean __shieldvortex_tls_start_channel (gint               channel_num, 
					   VortexConnection * connection, 
					   gpointer           user_data)
{
        // implement profile requirement for allowing starting a new channel

        // to return FALSE denies channel creation
        // to return TRUE allows create the channel
        return TRUE;
}

gboolean __shieldvortex_tls_close_channel (gint               channel_num, 
                        VortexConnection * connection, 
                        gpointer           user_data)
{
        // implement profile requirement for allowing to closeing a the channel
        // to return FALSE denies channel closing
        // to return TRUE allows to close the channel
        return TRUE;
}


void shieldvortex_tls_init ()
{
	vortex_profiles_register (TLS_PROFILE, 
				  __shieldvortex_tls_start_channel, NULL, 
                                  __shieldvortex_tls_close_channel, NULL,
                                  __shieldvortex_tls_frame_received, NULL);

}
