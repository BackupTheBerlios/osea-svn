//  Aspl-rule server: Daemon for server side for rule report system
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <librr/rr.h>
#include <liboseacomm/oseacomm.h>
#include <liboseaserver/oseaserver.h>
#include <config.h>
#include <popt.h>
#include <unistd.h>
#include "os_kernel_login_request.h"
#include "os_kernel_logout_request.h"
#include "os_kernel_register_request.h"
#include "os_kernel_user_request.h"
#include "os_kernel_group_request.h"
#include "os_kernel_permission_request.h"
#include "os_kernel_update_services_request.h"
#include "os_kernel_server_request.h"
#include "os_kernel_refresh_session_request.h"
#include "os_kernel_keys.h"
#include "os_kernel_crypto.h"
#include "os_kernel_registry.h"

#define LOG_DOMAIN "af-kernel"

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) gettext (String)
#else
#define _(String) (String)
#endif

gchar * os_kernel_superuser = NULL;
struct poptOption   os_kernel_additional_options[] = { 
	{ "add-superuser", '\0', POPT_ARG_STRING, &os_kernel_superuser, 0, "Add a superuser to the system", "name"},
	POPT_TABLEEND
};

// Services 
OseaServerServicesProvided services_provided []  = {
/* Basic services for AF-Architecture */
	{"login" ,                        "", os_kernel_login_request                 , FALSE, NULL, NULL, NULL}, 
	{"logout",                        "", os_kernel_logout_request                , FALSE, NULL, NULL, NULL},
	{"register",                      "", os_kernel_register_request              , FALSE, NULL, NULL, NULL},
	{"unregister",                    "", os_kernel_unregister_request            , FALSE, NULL, NULL, NULL},
	{"update_services",               "", os_kernel_update_services               , FALSE, NULL, NULL, NULL},
	{"refresh_key",                   "", os_kernel_keys_request                  , FALSE, NULL, NULL, NULL},
//	{"refresh_session",               "", os_kernel_refresh_session_request       , FALSE, NULL, NULL, NULL},
/* 	{"get_servers",                   "Service for listing all the servers registered in af-kernel",  */
/* 	 os_kernel_register_list_request, TRUE,  NULL, NULL, NULL}, */

/* Services for user/group/permission managing */
	{"new_user",                      "Service for creating a new user", 
	 os_kernel_user_new                      , TRUE,  NULL, NULL, NULL},
        {"remove_user",                   "Service for removing existing users", 
	 os_kernel_user_remove                   , TRUE, NULL,  NULL, NULL},
	{"edit_user",                     "Service for change properties of existing users", 
	 os_kernel_user_edit                     , TRUE, NULL,  NULL, NULL},
	{"list_user",                     "Service for listing users in the system with their properties", 
	 os_kernel_user_list                     , TRUE, NULL,  NULL, NULL},	
	{"list_group_user",               "", 
	 os_kernel_user_list_group               , TRUE, NULL,  "list_user", NULL},
	{"new_group",                     "Service for creating a new group of users", 
	 os_kernel_group_new                     , TRUE, NULL,  NULL, NULL},
	{"remove_group",                  "Service for removing an existing group of users", 
	 os_kernel_group_remove                  , TRUE, NULL,  NULL, NULL},
	{"edit_group",                    "Service for change properties (as name or description) of a group of users", 
	 os_kernel_group_edit                    , TRUE, NULL,  NULL, NULL},
	{"list_group",                    "Service for listings groups in system with their properties", 
	 os_kernel_group_list                    , TRUE, NULL,  NULL, NULL},
	{"add_user_group",                "Service for adding and removing users to/from groups", 
	 os_kernel_group_add_user                , TRUE, NULL,  NULL, NULL},
	{"remove_user_group",             "", 
	 os_kernel_group_remove_user             , TRUE, NULL,  "add_user_group", NULL},
	{"list_user_group",               "", 
	 os_kernel_group_list_user               , TRUE, NULL,  "list_group", NULL},
	{"permission_user_set",           "Service for adding or removing permissions to/from users", 
	 os_kernel_permission_user_set           , TRUE, NULL,  NULL, NULL},
	{"permission_user_unset",         "", 
	 os_kernel_permission_user_unset         , TRUE, NULL,  "permission_user_set", NULL},
	{"permission_group_set",          "Service for adding or removing permissions to/from groups of users", 
	 os_kernel_permission_group_set          , TRUE, NULL,  NULL, NULL},
	{"permission_group_unset",        "", 
	 os_kernel_permission_group_unset        , TRUE, NULL,  "permission_group_set", NULL},
	{"permission_list",               "Service for listing all available permissions in system", 
	 os_kernel_permission_list               , TRUE, NULL,  NULL, NULL},
	{"permission_list_by_user",       "", 
	 os_kernel_permission_list_by_user       , TRUE, NULL,  "permission_list", NULL},
	{"permission_from_group_list_by_user","", 
	 os_kernel_permission_from_group_list_by_user, TRUE, NULL,  "permission_list", NULL},
	{"permission_actual_list_by_user","",
	 os_kernel_permission_actual_list_by_user, TRUE, NULL,  "permission_list", NULL},
	{"permission_list_by_group",      "", 
	 os_kernel_permission_list_by_group      , TRUE, NULL,  "permission_list", NULL},
	{"server_list",                   "Service for obtain all the servers with permissions hosted in af-kernel", 
	 os_kernel_server_list                   , TRUE, NULL,  NULL, NULL},
	{"server_remove",                 "Service for removing all the permissions of a given server hosted in af-kernel", 
	 os_kernel_server_remove                 , TRUE, NULL,  NULL, NULL},
	OSEASERVER_SERVICES_TABLE_END
};


void __os_kernel_process_check ()
{
	OseaCommDataSet        * dataset;
	GList                * services_provided = oseaserver_services_get ();
	GString              * passwd = NULL;
	gboolean               result;
	GList                * iterator = NULL;
	gchar                * hostname;
	gchar                * port;
	gchar                * clear_passwd;
	gchar                * clear_passwd2;


	// Check if af-kernel has installed its services
	dataset = oseaserver_command_execute_single_query ("SELECT version FROM kernel_server WHERE name = 'af-kernel'");

	if (oseacomm_dataset_get_height (dataset) == 0 ) {
		oseacomm_dataset_free (dataset);
		
		oseaserver_command_execute_non_query ("INSERT INTO kernel_server (name,version,description) VALUES ('af-kernel',1,'Kernel server')");
		
		dataset = oseaserver_command_execute_single_query ("SELECT id FROM kernel_server WHERE name = 'af-kernel'");
		
		// Af-kernel hasn't installed anything yet

		iterator = g_list_first (services_provided);

		while (iterator) {
			// Don't register the following services
			if ((((OseaServerServicesProvided *)(iterator->data))->need_key)
			    && (! ((OseaServerServicesProvided *) (iterator->data))->actual_permission)) {
								
				result = oseaserver_command_execute_non_query ("INSERT INTO kernel_permission (name, description, server_id) \
                                                                          VALUES ('%s', '%s', %s)",
									 ((OseaServerServicesProvided *) (iterator->data))->service_name, 
									 ((OseaServerServicesProvided *) (iterator->data))->description, 
									 oseacomm_dataset_get (dataset, 0,0));

				if (!result) {
					g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Couldn't insert available services");
				}
			}
			
			// Increase iterator
			iterator = iterator->next;
			
		}
	}		
	oseacomm_dataset_free (dataset);

	// Check if there is no, so insert aspl user.
	if (os_kernel_superuser) {

		do {
			clear_passwd = g_strdup (getpass (_("Enter password for the new superuser: ")));
			clear_passwd2 = g_strdup (getpass (_("Retype the password, please: ")));
			if (!strcmp (clear_passwd, clear_passwd2)) {
				passwd = os_kernel_crypto_md5_sum (clear_passwd);
			}else
				g_printerr ("Password mismatch.\n");
		}while (! passwd);


		// There is no user into database, so create it
		result = oseaserver_command_execute_non_query ("INSERT INTO kernel_user (nick, passwd, description) VALUES ('%s', '%s', '%s')",
							 os_kernel_superuser,
							 passwd->str,
							 "Superuser");
		if (!result) 
			oseaserver_main_abort ("unable to create superuser.");
		
		// Now, give to aspl user all capabilities
		result = oseaserver_command_execute_non_query ("INSERT INTO kernel_user_have (user_id, permission_id) \
                                                          SELECT u.id, p.id FROM kernel_permission p, kernel_user u \
                                                          WHERE u.nick = '%s'", os_kernel_superuser);
		if (!result) 
			oseaserver_main_abort ("unable to assign all permissions to user.");
		
		g_print ("User %s created successfully. Exiting..\n", os_kernel_superuser);
		oseaserver_log_write ("User %s created successfully. Exiting..\n", os_kernel_superuser);
		exit (0);
	}
		
	port = oseaserver_config_get (NULL, "listening port");
	if (! port)
		g_error ("Error: couldn't find 'listening port' key in config file");

	hostname = oseaserver_config_get (NULL, "listening hostname");
	
	if (!hostname)
		g_error ("Error: couldn't find 'listening hostname' key in config file");
	
	os_kernel_registry_insert ("af-kernel", hostname, port);

	return;
}


gint main (gint argc, gchar * argv[]) {

	gchar             * accepted_keys[] = {
					       "session expiration",
					       NULL
	};

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	oseaserver_log_check_and_disable_glog ();

	oseaserver_main_run_server ("af-kernel",
			      "AF Architecture Main server",
			      VERSION,
			      COMPILATION_DATE,
			      &argc,
			      &argv,
			      g_build_filename (SYSCONFDIR, "aspl-fact", "af-kernel.cfg", NULL ), 
			      accepted_keys,
			      services_provided,
			      0,
			      __os_kernel_process_check,
			      os_kernel_additional_options);
	
	return 0;
}
