//  ASPL Fact Server Services Update: 
//  Copyright (C) 2002, 2003  Advanced Software Production Line, S.L.
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

#ifndef __AFGS_SERVICES_H__
#define __AFGS_SERVICES_H__

#include <glib.h>
#include <afdal/afdal.h>
#include "oseaserver_config.h"

typedef struct _AfgsServicesProvided {
	gchar     * service_name; /* Name for service */
	gchar     * description;  /* Description to show when assigning permissions */
	gboolean (* service_request_process_function) (CoyoteXmlServiceData *data, 
						       gpointer user_data, 
						       RRChannel * channel, 
						       gint msg_no);
	gboolean    need_key;     /* Tells if this service need an af_key to be used */
	gpointer    user_data;    /* User data to pass to the service request process function */
	gchar     * actual_permission; /* Name of the actual permission checked when receiving request for this service */
	gchar     * dependencies; /* Comma-separated list of permissons in which this service depends on */
} AfgsServicesProvided;

#define AFGS_SERVICES_TABLE_END {NULL, NULL, NULL,  FALSE, NULL, NULL}

void                   oseaserver_services_set         (AfgsServicesProvided * services);

GList                * oseaserver_services_get         ();

CoyoteDataSet        * oseaserver_services_get_dataset ();

gboolean               oseaserver_services_process     (CoyoteXmlServiceData * data, RRChannel * channel, gint msg_no);

gboolean               oseaserver_services_update   (const gchar *server_name, const gchar *server_description, gint version_number);


#endif

