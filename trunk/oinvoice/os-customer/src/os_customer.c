//  af_customer: Daemon for the server-side
//  Copyright (C) 2002,2003 Advanced Software Production Line, S.L.

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

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) gettext (String)
#else
#define _(String) (String)
#endif

#include <config.h>
#include <glib.h>
#include <locale.h>
#include <librr/rr.h>
#include <coyote/coyote.h>
#include <afgs/afgs.h>
#include <popt.h>

// Modules exported by this server
#include "af_customer_enum_values.h"
#include "af_customer_address.h"
#include "af_customer_telephone.h"
#include "af_customer_inet_data.h"
#include "af_customer_contact.h"
#include "af_customer_customer.h"
#define LOG_DOMAIN "af_customer"
#define AF_CUSTOMER_SERVICES_VERSION 1

//Services
AfgsServicesProvided services_provided[] = {
	{"af_customer_enum_values_address_set", "",
	 af_customer_enum_values_address_set, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_address_update_all", "",
	 af_customer_enum_values_address_update_all, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_address_list", "",
	 af_customer_enum_values_address_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_address_remove", "",
	 af_customer_enum_values_address_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_address_add", "",
	 af_customer_enum_values_address_add, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_telephone_set", "",
	 af_customer_enum_values_telephone_set, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_telephone_update_all", "",
	 af_customer_enum_values_telephone_update_all, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_telephone_list", "",
	 af_customer_enum_values_telephone_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_telephone_remove", "",
	 af_customer_enum_values_telephone_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_telephone_add", "",
	 af_customer_enum_values_telephone_add, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_inet_data_set", "",
	 af_customer_enum_values_inet_data_set, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_inet_data_update_all", "",
	 af_customer_enum_values_inet_data_update_all, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_inet_data_list", "",
	 af_customer_enum_values_inet_data_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_inet_data_remove", "",
	 af_customer_enum_values_inet_data_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_inet_data_add", "",
	 af_customer_enum_values_inet_data_add, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_customer_set", "",
	 af_customer_enum_values_customer_set, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_customer_update_all", "",
	 af_customer_enum_values_customer_update_all, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_customer_list", "",
	 af_customer_enum_values_customer_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_customer_remove", "",
	 af_customer_enum_values_customer_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_customer_add", "",
	 af_customer_enum_values_customer_add, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_new", "",
	 af_customer_enum_values_new, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_remove", "",
	 af_customer_enum_values_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_edit", "",
	 af_customer_enum_values_edit, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_enum_values_list", "",
	 af_customer_enum_values_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_address_new", "",
	 af_customer_address_new, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_address_remove", "",
	 af_customer_address_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_address_edit", "",
	 af_customer_address_edit, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_address_list", "",
	 af_customer_address_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_telephone_new", "",
	 af_customer_telephone_new, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_telephone_remove", "",
	 af_customer_telephone_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_telephone_edit", "",
	 af_customer_telephone_edit, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_telephone_list", "",
	 af_customer_telephone_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_inet_data_new", "",
	 af_customer_inet_data_new, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_inet_data_remove", "",
	 af_customer_inet_data_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_inet_data_edit", "",
	 af_customer_inet_data_edit, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_inet_data_list", "",
	 af_customer_inet_data_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_contact_new", "",
	 af_customer_contact_new, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_contact_remove", "",
	 af_customer_contact_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_contact_edit", "",
	 af_customer_contact_edit, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_contact_list", "",
	 af_customer_contact_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_address_set", "",
	 af_customer_customer_address_set, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_address_update_all", "",
	 af_customer_customer_address_update_all, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_address_list", "",
	 af_customer_customer_address_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_address_remove", "",
	 af_customer_customer_address_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_address_add", "",
	 af_customer_customer_address_add, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_telephone_set", "",
	 af_customer_customer_telephone_set, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_telephone_update_all", "",
	 af_customer_customer_telephone_update_all, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_telephone_list", "",
	 af_customer_customer_telephone_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_telephone_remove", "",
	 af_customer_customer_telephone_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_telephone_add", "",
	 af_customer_customer_telephone_add, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_inet_data_set", "",
	 af_customer_customer_inet_data_set, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_inet_data_update_all", "",
	 af_customer_customer_inet_data_update_all, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_inet_data_list", "",
	 af_customer_customer_inet_data_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_inet_data_remove", "",
	 af_customer_customer_inet_data_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_inet_data_add", "",
	 af_customer_customer_inet_data_add, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_contact_set", "",
	 af_customer_customer_contact_set, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_contact_update_all", "",
	 af_customer_customer_contact_update_all, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_contact_list", "",
	 af_customer_customer_contact_list, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_contact_remove", "",
	 af_customer_customer_contact_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_contact_add", "",
	 af_customer_customer_contact_add, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_new", "",
	 af_customer_customer_new, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_remove", "",
	 af_customer_customer_remove, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_edit", "",
	 af_customer_customer_edit, TRUE, NULL, NULL, NULL}
	,
	{"af_customer_customer_list", "",
	 af_customer_customer_list, TRUE, NULL, NULL, NULL}
	,
	AFGS_SERVICES_TABLE_END
};

gint main (gint argc,
	   gchar * argv[])
{

	gchar              *accepted_keys[] = { NULL };


#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	afgs_log_check_and_disable_glog ();

	afgs_main_run_server ("af-customer",
			      "af-customer server",
			      VERSION,
			      COMPILATION_DATE,
			      &argc, &argv,
			      g_build_filename (SYSCONFDIR, "aspl-fact", "af-customer.cfg", NULL),
			      accepted_keys,
			      services_provided,
			      AF_CUSTOMER_SERVICES_VERSION, NULL, AFGS_MAIN_NO_EXTRA_OPTION);

	return 0;
}
