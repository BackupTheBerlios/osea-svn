//  ASPL Fact: invoicing client program for ASPL Fact System
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include <oseaclientkernel/aos_kernel.h>
#include "login_window.h"
#include "settings.h"
#include "connection_settings_window.h"
#include "connection_settings_wizard.h"
#include <popt.h>
#include <oseaclient/afdal.h>
#include <oseaclientkernel/aos_kernel.h>
#include <oseaclienttax/aos_tax.h>

void fake_log_handler (const gchar *log_domain,
		       GLogLevelFlags log_level,
		       const gchar *message,
		       gpointer user_data)
{
	return;
}

gint oinvoice_version_option = 0;
struct poptOption oinvoice_options[] = { 
	{ "version", 'v', POPT_ARG_NONE, &oinvoice_version_option, 0, "Show version info", NULL},
	POPT_TABLEEND
};

int main (int argc, char *argv[])
{

	GnomeProgram * asplfact_app = NULL;
	glong cd = COMPILATION_DATE;
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	if (! getenv ("AF_DEBUG")) {
		g_log_set_handler (NULL, G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("OSEACOMM_CONNECTION", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("OSEACLIENT_REQUEST", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("AOS_TAX_VALUE", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("oseaclient_session", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("OSEACOMM_SIMPLE", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("OSEACOMM_XML", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("Gtk", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("usergroup", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("group", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
		g_log_set_handler ("tax", G_LOG_LEVEL_DEBUG | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING 
				   | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_RECURSION, fake_log_handler, NULL);
	}

	asplfact_app = gnome_program_init ("ASPL Fact", VERSION, LIBGNOMEUI_MODULE, argc, argv, 
					   GNOME_PARAM_POPT_TABLE, oinvoice_options, NULL);

	oseaclient_init ();

	if (oinvoice_version_option) {
		g_print ("ASPL Fact version %s, compiled %s", VERSION, ctime(&cd));
		oseaclient_print_version_info ();
		aos_kernel_print_version_info ();
		aos_tax_print_version_info ();
		// aos_customer_print_version_info ();
		exit (0);
	}

	if (! settings_init () ) {
		// There are not settings: calling connection settings wizard
		connection_settings_wizard_show ();
	} else {
		login_window_show ();
	}

 	gtk_main ();

	return 0;
}

