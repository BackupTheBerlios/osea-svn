//  os_customer: Daemon for the server-side
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
#include <liboseacomm/oseacomm.h>
#include <liboseaserver/oseaserver.h>
#include <popt.h>

// Modules exported by this server
#include "os_customer_enum_values.h"
#include "os_customer_address.h"
#include "os_customer_telephone.h"
#include "os_customer_inet_data.h"
#include "os_customer_contact.h"
#include "os_customer_customer.h"
#define LOG_DOMAIN "os_customer"
#define OS_CUSTOMER_SERVICES_VERSION 1

//Services
OseaServerServicesProvided services_provided[] = {
	{"enum_values_address_set", "",
	 os_customer_enum_values_address_set, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_address_update_all", "",
	 os_customer_enum_values_address_update_all, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_address_list", "",
	 os_customer_enum_values_address_list, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_address_remove", "",
	 os_customer_enum_values_address_remove, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_address_add", "",
	 os_customer_enum_values_address_add, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_telephone_set", "",
	 os_customer_enum_values_telephone_set, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_telephone_update_all", "",
	 os_customer_enum_values_telephone_update_all, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_telephone_list", "",
	 os_customer_enum_values_telephone_list, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_telephone_remove", "",
	 os_customer_enum_values_telephone_remove, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_telephone_add", "",
	 os_customer_enum_values_telephone_add, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_inet_data_set", "",
	 os_customer_enum_values_inet_data_set, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_inet_data_update_all", "",
	 os_customer_enum_values_inet_data_update_all, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_inet_data_list", "",
	 os_customer_enum_values_inet_data_list, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_inet_data_remove", "",
	 os_customer_enum_values_inet_data_remove, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_inet_data_add", "",
	 os_customer_enum_values_inet_data_add, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_customer_set", "",
	 os_customer_enum_values_customer_set, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_customer_update_all", "",
	 os_customer_enum_values_customer_update_all, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_customer_list", "",
	 os_customer_enum_values_customer_list, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_customer_remove", "",
	 os_customer_enum_values_customer_remove, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_customer_add", "",
	 os_customer_enum_values_customer_add, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_new", "",
	 os_customer_enum_values_new, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_remove", "",
	 os_customer_enum_values_remove, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_edit", "",
	 os_customer_enum_values_edit, TRUE, NULL, NULL, NULL}
	,
	{"enum_values_list", "",
	 os_customer_enum_values_list, TRUE, NULL, NULL, NULL}
	,
	{"address_new", "",
	 os_customer_address_new, TRUE, NULL, NULL, NULL}
	,
	{"address_remove", "",
	 os_customer_address_remove, TRUE, NULL, NULL, NULL}
	,
	{"address_edit", "",
	 os_customer_address_edit, TRUE, NULL, NULL, NULL}
	,
	{"address_list", "",
	 os_customer_address_list, TRUE, NULL, NULL, NULL}
	,
	{"telephone_new", "",
	 os_customer_telephone_new, TRUE, NULL, NULL, NULL}
	,
	{"telephone_remove", "",
	 os_customer_telephone_remove, TRUE, NULL, NULL, NULL}
	,
	{"telephone_edit", "",
	 os_customer_telephone_edit, TRUE, NULL, NULL, NULL}
	,
	{"telephone_list", "",
	 os_customer_telephone_list, TRUE, NULL, NULL, NULL}
	,
	{"inet_data_new", "",
	 os_customer_inet_data_new, TRUE, NULL, NULL, NULL}
	,
	{"inet_data_remove", "",
	 os_customer_inet_data_remove, TRUE, NULL, NULL, NULL}
	,
	{"inet_data_edit", "",
	 os_customer_inet_data_edit, TRUE, NULL, NULL, NULL}
	,
	{"inet_data_list", "",
	 os_customer_inet_data_list, TRUE, NULL, NULL, NULL}
	,
	{"contact_new", "",
	 os_customer_contact_new, TRUE, NULL, NULL, NULL}
	,
	{"contact_remove", "",
	 os_customer_contact_remove, TRUE, NULL, NULL, NULL}
	,
	{"contact_edit", "",
	 os_customer_contact_edit, TRUE, NULL, NULL, NULL}
	,
	{"contact_list", "",
	 os_customer_contact_list, TRUE, NULL, NULL, NULL}
	,
	{"customer_address_set", "",
	 os_customer_customer_address_set, TRUE, NULL, NULL, NULL}
	,
	{"customer_address_update_all", "",
	 os_customer_customer_address_update_all, TRUE, NULL, NULL, NULL}
	,
	{"customer_address_list", "",
	 os_customer_customer_address_list, TRUE, NULL, NULL, NULL}
	,
	{"customer_address_remove", "",
	 os_customer_customer_address_remove, TRUE, NULL, NULL, NULL}
	,
	{"customer_address_add", "",
	 os_customer_customer_address_add, TRUE, NULL, NULL, NULL}
	,
	{"customer_telephone_set", "",
	 os_customer_customer_telephone_set, TRUE, NULL, NULL, NULL}
	,
	{"customer_telephone_update_all", "",
	 os_customer_customer_telephone_update_all, TRUE, NULL, NULL, NULL}
	,
	{"customer_telephone_list", "",
	 os_customer_customer_telephone_list, TRUE, NULL, NULL, NULL}
	,
	{"customer_telephone_remove", "",
	 os_customer_customer_telephone_remove, TRUE, NULL, NULL, NULL}
	,
	{"customer_telephone_add", "",
	 os_customer_customer_telephone_add, TRUE, NULL, NULL, NULL}
	,
	{"customer_inet_data_set", "",
	 os_customer_customer_inet_data_set, TRUE, NULL, NULL, NULL}
	,
	{"customer_inet_data_update_all", "",
	 os_customer_customer_inet_data_update_all, TRUE, NULL, NULL, NULL}
	,
	{"customer_inet_data_list", "",
	 os_customer_customer_inet_data_list, TRUE, NULL, NULL, NULL}
	,
	{"customer_inet_data_remove", "",
	 os_customer_customer_inet_data_remove, TRUE, NULL, NULL, NULL}
	,
	{"customer_inet_data_add", "",
	 os_customer_customer_inet_data_add, TRUE, NULL, NULL, NULL}
	,
	{"customer_contact_set", "",
	 os_customer_customer_contact_set, TRUE, NULL, NULL, NULL}
	,
	{"customer_contact_update_all", "",
	 os_customer_customer_contact_update_all, TRUE, NULL, NULL, NULL}
	,
	{"customer_contact_list", "",
	 os_customer_customer_contact_list, TRUE, NULL, NULL, NULL}
	,
	{"customer_contact_remove", "",
	 os_customer_customer_contact_remove, TRUE, NULL, NULL, NULL}
	,
	{"customer_contact_add", "",
	 os_customer_customer_contact_add, TRUE, NULL, NULL, NULL}
	,
	{"customer_new", "",
	 os_customer_customer_new, TRUE, NULL, NULL, NULL}
	,
	{"customer_remove", "",
	 os_customer_customer_remove, TRUE, NULL, NULL, NULL}
	,
	{"customer_edit", "",
	 os_customer_customer_edit, TRUE, NULL, NULL, NULL}
	,
	{"customer_list", "",
	 os_customer_customer_list, TRUE, NULL, NULL, NULL}
	,
	OSEASERVER_SERVICES_TABLE_END
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

	oseaserver_log_check_and_disable_glog ();

	oseaserver_main_run_server ("os-customer",
			      "os-customer server",
			      VERSION,
			      COMPILATION_DATE,
			      &argc, &argv,
			      g_build_filename (SYSCONFDIR, "oinvoice", "os-customer.cfg", NULL),
			      accepted_keys,
			      services_provided,
			      OS_CUSTOMER_SERVICES_VERSION, NULL, OSEASERVER_MAIN_NO_EXTRA_OPTION);

	return 0;
}
