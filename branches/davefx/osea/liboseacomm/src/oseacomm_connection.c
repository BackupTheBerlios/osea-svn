//  LibCoyote:  Support library with xml and communication functions.
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
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
#include "oseacomm_connection.h"
#include <glib.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#define LOG_DOMAIN "COYOTE_CONNECTION"


/**
 * oseacomm_connection_new:
 * @server: server we are going to connect to.
 * @port: tcp port.
 * @connection_profile: beep profile identifier. Try TYPE_COYOTE_SIMPLE.
 * 
 * This function create a new Road Runner connection to @server:@port using given
 * profile @connection_profile.
 * 
 * Return value: A Road Runner connection or NULL.
 **/
RRConnection * oseacomm_connection_new (gchar * server, gchar * port, gint connection_profile)
{
	GError * error = NULL;
	RRConnection * connection = NULL;
	RRProfileRegistry * beep_profiles = NULL;
	
	g_return_val_if_fail (server, NULL);
	g_return_val_if_fail (port, NULL);
	
	beep_profiles = rr_profile_registry_new ();

	rr_profile_registry_add_profile (beep_profiles, connection_profile, NULL);

	connection = rr_tcp_connection_new (beep_profiles, server, atoi(port), &error);
	if (!connection) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Error while connecting to %s:%s.\n", server, port);
		return NULL;
	}
	return connection;
}



/**
 * oseacomm_connection_get_client_name:
 * @channel: 
 * 
 * Gets the host name from the channel. This host is the origin of the
 * connection which holds the @channel. 
 * 
 * Return value: the host name or NULL if fails. The result must be freed.
 **/
gchar        * oseacomm_connection_get_client_name (RRChannel * channel)
{
	gint                 socket;
	struct sockaddr_in   sin;
	gint                 sin_size = sizeof (sin);



	g_return_val_if_fail (channel, NULL);

	socket = rr_tcp_connection_get_fd (RR_TCP_CONNECTION (channel->connection));

	if (getpeername (socket, (struct sockaddr *) &sin, &sin_size) < -1) 
		return NULL;

	return g_strdup (inet_ntoa (sin.sin_addr));
}
