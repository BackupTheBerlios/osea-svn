//  Copyright (C) 2003  Advanced Software Production Line, S.L.
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

#ifndef __OSEASERVER_MAIN_H__
#define __OSEASERVER_MAIN_H__
#include <glib.h>
#include <string.h>
#include <popt.h>
#include "oseaserver.h"

#define OSEASERVER_MAIN_NO_EXTRA_OPTION oseaserver_main_no_extra_options ()

typedef void (* OseaServerMainCallback) (void);

void oseaserver_main_run_server (gchar                * server_name,
			   gchar                * server_description,
			   gchar                * server_version,
			   glong                  server_compilation_date,
			   gint                 * argc, 
			   gchar               ** argv[], 
			   gchar                * config_file,
			   gchar                * accepted_keys[], 
			   OseaServerServicesProvided   services_provided[],
			   gint                   services_version,
			   OseaServerMainCallback       check_function,
			   struct poptOption    * server_extra_popt_option);

void oseaserver_main_abort      (gchar * format, ...);

struct poptOption * oseaserver_main_no_extra_options ();

#endif
