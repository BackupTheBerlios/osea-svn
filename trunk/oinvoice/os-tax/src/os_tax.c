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
#include "os_tax_vat_customer.h"
#include "os_tax_vat_item.h"
#include "os_tax_vat_value.h"

#define LOG_DOMAIN "os-tax"

#define OS_TAX_SERVICES_VERSION 1

OseaServerServicesProvided  services_provided []  = {
	{"list_vat_customer" ,   "Service for listing VAT-customer types", 
	 os_tax_vat_customer_list,       TRUE,  NULL, NULL, NULL},
	{"new_vat_customer" ,    "Service for adding a new VAT-customer type", 
	 os_tax_vat_customer_new,        TRUE,  NULL, NULL, NULL},
	{"edit_vat_customer" ,   "Service for changing properties of a VAT-customer type", 
	 os_tax_vat_customer_edit,       TRUE,  NULL, NULL, NULL},
	{"remove_vat_customer" , "Service for removing an existing VAT-customer type", 
	 os_tax_vat_customer_remove,     TRUE,  NULL, NULL, NULL},
	{"list_vat_item" ,       "Service for listing VAT-item types", 
	 os_tax_vat_item_list,           TRUE,  NULL, NULL, NULL},
	{"new_vat_item" ,        "Service for adding a new VAT-item type", 
	 os_tax_vat_item_new,            TRUE,  NULL, NULL, NULL},
	{"edit_vat_item" ,       "Service for changing properties of a VAT-item type", 
	 os_tax_vat_item_edit,           TRUE,  NULL, NULL, NULL},
	{"remove_vat_item" ,     "Service for removing an existing VAT-customer type", 
	 os_tax_vat_item_remove,         TRUE,  NULL, NULL, NULL},
	{"get_vat_value",        "Service for listing and getting VAT values", 
	 os_tax_vat_value_get,           TRUE,  NULL, NULL, NULL},
	{"set_vat_value",        "Service for setting VAT values", 
	 os_tax_vat_value_set,           TRUE,  NULL, NULL, NULL},
	{"list_vat_value",       "", 
	 os_tax_vat_value_list,          TRUE,  NULL, "get_vat_value", NULL},
	OSEASERVER_SERVICES_TABLE_END
};



gint main (gint argc, gchar * argv[]) {

	gchar             * accepted_keys[] = { NULL };

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	oseaserver_log_check_and_disable_glog ();


	oseaserver_main_run_server ("os-tax",
			      "AF Architecture Tax Server",
			      VERSION,
			      COMPILATION_DATE,
			      &argc, &argv, 
			      g_build_filename (SYSCONFDIR, "oinvoice", "os-tax.cfg", NULL ),
			      accepted_keys,
			      services_provided,
			      OS_TAX_SERVICES_VERSION,
			      NULL,
			      OSEASERVER_MAIN_NO_EXTRA_OPTION);



	return 0;
}
