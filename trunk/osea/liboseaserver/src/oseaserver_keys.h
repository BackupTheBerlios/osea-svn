//  LibOseaServer:
//  Copyright (C) 2002, 2003 Advanced Software Production Line, S.L.
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

#ifndef __OSEASERVER_AFKEYS_H__
#define __OSEASERVER_AFKEYS_H__

#include <liboseacomm/oseacomm.h>
#include "oseaserver.h"


typedef struct __OseaServerAfKey {
	gchar * user;
	gchar * satellite_server;
	glong   time_stamp;
	GList * permission_list;
}OseaServerAfKey;


OseaServerAfKey  * oseaserver_afkeys_parse_and_check           (gchar     * af_key);

void         oseaserver_afkeys_destroy                   (OseaServerAfKey * af_key, 
						    gboolean     free_permission_list);

void         oseaserver_afkeys_set_connection_key_simple (RRConnection *connection,
						    OseaServerAfKey    *af_key);

gboolean     oseaserver_afkeys_set_connection_key        (OseaCommXmlServiceData * data,
						    gpointer               user_data,
						    RRChannel            * channel,
						    gint                   msg_no);

OseaServerAfKey  * oseaserver_afkeys_get_connection_key        (RRConnection * connection);

gboolean     oseaserver_afkeys_is_current                (gchar * af_key);

gboolean     oseaserver_afkeys_check_service_permission  (RRChannel * channel, gchar * service_name, gint msg_no);

#endif
