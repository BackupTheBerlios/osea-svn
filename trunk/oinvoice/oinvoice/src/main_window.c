//  ASPL Fact: invoicing client program for ASPL Fact System
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

#include "config.h"
#include "main_window.h"
#include "login_window.h"
#include "error.h"
#include "tax.h"
#include "usergroup.h"
#include "customer.h"
#include "about_window.h"
#include <gal/shortcut-bar/e-shortcut-bar.h>
#include <oseaclient/afdal.h>
#include "settings.h"
#include "event_source.h"
#include "event_process.h"
#include "dialog.h"
#include "osea_datalist.h"
#include "widgets.h"

#define LOG_DOMAIN "main_window"

typedef GtkWidget * (*MainWindowCreateWidgetFunc)  (void);
typedef void        (*MainWindowDestroyWidgetFunc) (GtkWidget *widget);

enum {MAIN_WINDOW_ERROR_COMPONENT, 
      MAIN_WINDOW_VAT_COMPONENT, 
      MAIN_WINDOW_USER_COMPONENT,
      MAIN_WINDOW_CUSTOMER_COMPONENT,
      MAIN_WINDOW_COMPONENT_NUMBER};

typedef struct __MainWindowComponent {
	gchar *name;
	gchar *servername;
	MainWindowCreateWidgetFunc create_component;
	MainWindowCreateWidgetFunc create_menubar;
	MainWindowCreateWidgetFunc create_toolbar;
	MainWindowDestroyWidgetFunc destroy_component;	
	MainWindowDestroyWidgetFunc destroy_menubar;	
	MainWindowDestroyWidgetFunc destroy_toolbar;
	gchar *shortcutbar_name;
	gchar *pixmap;
	gint   group;
	gint   order;
} MainWindowComponent;

enum {MAIN_WINDOW_GROUP_INVOICING, MAIN_WINDOW_GROUP_MANAGING, MAIN_WINDOW_COMPONENT_GROUP_NUMBER};

typedef struct __MainWindowComponentGroup {
	gint id;
	gchar *shortcutbar_name;
} MainWindowComponentGroup;

MainWindowComponentGroup main_window_component_groups [] =
{{MAIN_WINDOW_GROUP_INVOICING, "Invoicing"},
 {MAIN_WINDOW_GROUP_MANAGING, "Managing"},
 {-1, NULL}};

MainWindowComponent main_window_components [] = 
{{"error", "os-kernel", error_component_create, error_menu_create, error_toolbar_create, 
  error_component_destroy, error_menu_destroy, error_toolbar_destroy, NULL, NULL, -1, 0},
 
 {"vat", "os-tax", tax_vat_component_create, tax_vat_menu_create, tax_vat_toolbar_create, 
  tax_vat_component_destroy, tax_vat_menu_destroy, tax_vat_toolbar_destroy, 
  "VAT Managing", "vat-48x48-normal.png", MAIN_WINDOW_GROUP_MANAGING, 0},

 {"user", "os-kernel", usergroup_component_create, usergroup_menu_create, usergroup_toolbar_create, 
  usergroup_component_destroy, usergroup_menu_destroy, usergroup_toolbar_destroy,
  "Users & Permissions", "users-48x48-normal.png", MAIN_WINDOW_GROUP_MANAGING, 0},

 {"customer", "os-customer", customer_component_create, customer_menu_create, customer_toolbar_create, 
  customer_component_destroy, customer_menu_destroy, customer_toolbar_destroy,
  "Customers", "malehead.png", MAIN_WINDOW_GROUP_INVOICING, 0},

 {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};

guint      main_window_active_component;

GladeXML * main_window_xml = NULL;
gint       progress_timer = 0;
gint       status_message_timer = 0;

gint       main_window_transaction_number = 0;

GList    * main_window_active_window = NULL;

void __translate_component_names (void) {
	gchar *s;
	s = _("Managing");
	s = _("VAT Managing");
	s = _("Users & Permissions");
	s = _("Invoicing");
	s = _("Customers");
}

gint __component_lookup (gchar *name) {
	gint i;

	if (! name)
		return MAIN_WINDOW_VAT_COMPONENT;
		
	for (i = 0; i < MAIN_WINDOW_COMPONENT_NUMBER; i++)
		if (! g_strcasecmp (main_window_components[i].name, name))
			return i;
	
	return MAIN_WINDOW_VAT_COMPONENT;
}


void main_window_show()
{
	gchar     * xml_file = NULL;
	GtkWidget * widget = NULL, * hpaned;
	gchar     * active_component_name, * setting;
	gint        shortcutbar_width;

	if (! main_window_xml) {		
		xml_file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", "main", NULL );
		
		main_window_xml = widgets_load (xml_file, "main_window", NULL);
		
	}

	widget = glade_xml_get_widget (main_window_xml, "main_window");

	if (!widget) {
		g_error (_("Couldn't find 'main_window' widget in main.glade"));
		exit (1);
	}

	active_component_name = settings_get ("active_component", SETTINGS_STRING);
	main_window_active_component = __component_lookup (active_component_name);	

	main_window_menubar_create ();
	main_window_toolbar_create ();
	main_window_component_create ();

	setting = settings_get ("gui/shortcutbar_width", SETTINGS_INT);

	if (setting) {
		shortcutbar_width = atoi (setting);
		hpaned = glade_xml_get_widget (main_window_xml, "hpaned1");
		gtk_paned_set_position (GTK_PANED (hpaned), shortcutbar_width);
	}

	gtk_widget_show (widget);

	return;
}


void main_window_destroy ()
{
	GtkWidget * main_window = NULL, * hpaned = NULL;
	gchar     * hpaned_width;

	g_return_if_fail (main_window_xml);

	main_window = glade_xml_get_widget (main_window_xml, "main_window");

	if (main_window_xml) {

		if (main_window_active_component != MAIN_WINDOW_ERROR_COMPONENT)
			settings_set ("active_component", 
				      main_window_components[main_window_active_component].name, 
				      SETTINGS_STRING);

		hpaned = glade_xml_get_widget (main_window_xml, "hpaned1");
		hpaned_width = g_strdup_printf ("%d", gtk_paned_get_position (GTK_PANED (hpaned)));
		settings_set ("gui/shortcutbar_width", hpaned_width, SETTINGS_INT);
		g_free (hpaned_width);

		main_window_component_destroy ();
		main_window_menubar_destroy ();
		main_window_toolbar_destroy ();
		
 		g_object_unref (G_OBJECT (main_window_xml));
		main_window_xml = NULL;

		gtk_object_destroy (GTK_OBJECT(main_window));
		main_window = NULL;		
	}
	return;
}


GtkWindow * main_window_get () 
{
	return GTK_WINDOW(glade_xml_get_widget (main_window_xml, "main_window"));
}


GdkPixbuf * __main_window_load_image (gchar *filename)
{
	gchar     * file = NULL;
	GdkPixbuf * pixbuf = NULL;
	GError    * error  = NULL;

	if (! main_window_xml) {		
		file = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", filename, NULL );
		
		pixbuf = gdk_pixbuf_new_from_file (file, &error);
	}

	if (! pixbuf) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, _("Error when loading image '%s': %s"), filename, error->message);
		exit (1);
	}

	return pixbuf;
}




GtkWidget * main_window_sidebar_create (gchar *string1, gchar *string2, gint int1, gint int2)
{
	GtkWidget      * widget;
	EShortcutModel * model;
	gint             i;

	model = e_shortcut_model_new ();
	
	for (i=0; i<MAIN_WINDOW_COMPONENT_GROUP_NUMBER; i++) {
		e_shortcut_model_add_group (model, -1, 
					    _(main_window_component_groups[i].shortcutbar_name));
	}

	for (i=1; i<MAIN_WINDOW_COMPONENT_NUMBER; i++) {
		main_window_components[i].order = 
			e_shortcut_model_add_item (model, main_window_components[i].group, -1,
						   main_window_components[i].name,
						   _(main_window_components[i].shortcutbar_name),
						   __main_window_load_image (main_window_components[i].pixmap));
	}
	

	widget = e_shortcut_bar_new ();
	e_shortcut_bar_set_model (E_SHORTCUT_BAR (widget), model);
	
	e_shortcut_bar_set_enable_drags (E_SHORTCUT_BAR (widget), FALSE);

	g_signal_connect (E_SHORTCUT_BAR (widget), "item_selected", 
			  (GCallback) main_window_sidebar_select, NULL);

	gtk_widget_show (widget);

	return widget;
}

void main_window_sidebar_destroy_event (GtkWidget *widget, gpointer data)
{
	
/* 	g_object_unref (G_OBJECT(widget)); */

	return;
}

void main_window_menubar_create () 
{
	GtkWidget          * menubar;
	GtkWidget          * menubar_container;

	if (! oseaclient_session_server_exists (main_window_components[main_window_active_component].servername))
	    main_window_active_component = MAIN_WINDOW_ERROR_COMPONENT;
	
	menubar =  main_window_components[main_window_active_component].create_menubar ();

	menubar_container = glade_xml_get_widget (main_window_xml, "main_window_menubar_handlebox");
	
	gtk_container_add (GTK_CONTAINER(menubar_container), menubar);

	return;
}

void main_window_menubar_destroy ()
{
	GtkContainer * parent;
	GList        * list = NULL;
	GtkWidget    * widget;
	GtkWidget    * main_window = NULL;

	g_return_if_fail (main_window_xml);

	main_window = glade_xml_get_widget (main_window_xml, "main_window");

	parent = GTK_CONTAINER(glade_xml_get_widget (main_window_xml, "main_window_menubar_handlebox"));
	list = gtk_container_get_children (parent);
	widget = GTK_WIDGET(list->data);
	g_list_free (list);
	gtk_container_remove (parent, widget);		
	main_window_components[main_window_active_component].destroy_menubar (main_window);

	return;

}

gint main_window_menubar_update (gint old_component, gint new_component) 
{
	GtkWidget          * menubar;
	GtkWidget          * menubar_container;
	GList              * list = NULL;
	GtkWidget          * widget;
	GtkWidget          * main_window = NULL;

	g_return_val_if_fail (main_window_xml, -1);
	
	menubar =  main_window_components[new_component].create_menubar ();

	menubar_container = glade_xml_get_widget (main_window_xml, "main_window_menubar_handlebox");
	
	main_window = glade_xml_get_widget (main_window_xml, "main_window");

	list = gtk_container_get_children (GTK_CONTAINER(menubar_container));
	widget = GTK_WIDGET(list->data);
	g_list_free (list);
	gtk_container_remove (GTK_CONTAINER(menubar_container), widget);		
	gtk_container_add (GTK_CONTAINER(menubar_container), menubar);
	main_window_components[old_component].destroy_menubar (main_window);

	return new_component;

}

void main_window_toolbar_create ()
{
	GtkWidget          * toolbar;
	GtkWidget          * toolbar_container;

	

	if (! oseaclient_session_server_exists (main_window_components[main_window_active_component].servername))
		main_window_active_component = MAIN_WINDOW_ERROR_COMPONENT;

	toolbar = main_window_components[main_window_active_component].create_toolbar ();

	toolbar_container = glade_xml_get_widget (main_window_xml, "main_window_toolbar_handlebox");
	
	gtk_container_add (GTK_CONTAINER(toolbar_container), toolbar);

	return;
}

void main_window_toolbar_destroy ()
{
	GtkContainer * parent;
	GList        * list = NULL;
	GtkWidget    * widget;
	GtkWidget    * main_window = NULL;

	g_return_if_fail (main_window_xml);

	main_window = glade_xml_get_widget (main_window_xml, "main_window");

	main_window_set_progressbar (FALSE);

	parent = GTK_CONTAINER(glade_xml_get_widget (main_window_xml, "main_window_toolbar_handlebox"));
	list = gtk_container_get_children (parent);
	widget = GTK_WIDGET(list->data);
	g_list_free (list);
	gtk_container_remove (parent, widget);
	main_window_components[main_window_active_component].destroy_toolbar (main_window);	

	return;
}

gint main_window_toolbar_update (gint old_component, gint new_component) 
{
	GtkWidget          * toolbar;
	GtkWidget          * toolbar_container;
	GList              * list = NULL;
	GtkWidget          * widget;
	GtkWidget          * main_window = NULL;

	g_return_val_if_fail (main_window_xml, -1);

	toolbar =  main_window_components[new_component].create_toolbar ();

	toolbar_container = glade_xml_get_widget (main_window_xml, "main_window_toolbar_handlebox");
	
	main_window = glade_xml_get_widget (main_window_xml, "main_window");

	list = gtk_container_get_children (GTK_CONTAINER(toolbar_container));
	widget = GTK_WIDGET(list->data);
	g_list_free (list);
	gtk_container_remove (GTK_CONTAINER(toolbar_container), widget);		
	gtk_container_add (GTK_CONTAINER(toolbar_container), toolbar);
	main_window_components[old_component].destroy_toolbar (main_window);

	return new_component;

}

void main_window_component_create ()
{
	GtkWidget          * component;
	GtkWidget          * component_container;

	if (! oseaclient_session_server_exists (main_window_components[main_window_active_component].servername))
		main_window_active_component = MAIN_WINDOW_ERROR_COMPONENT;

	component = main_window_components[main_window_active_component].create_component ();
	component_container = glade_xml_get_widget (main_window_xml, "hpaned1");

	gtk_container_add (GTK_CONTAINER(component_container), component);

	return;
}

void main_window_component_destroy ()
{
	GtkContainer * parent;
	GList        * list = NULL;
	GtkWidget    * widget;
	GtkWidget    * main_window = NULL;

	g_return_if_fail (main_window_xml);

	main_window = glade_xml_get_widget (main_window_xml, "main_window");

	parent = GTK_CONTAINER(glade_xml_get_widget (main_window_xml, "hpaned1"));
	list = gtk_container_get_children (parent);
	widget = GTK_WIDGET( g_list_nth_data (list, 1));
	g_list_free (list);
	gtk_container_remove (parent, widget);		
	main_window_components[main_window_active_component].destroy_component (main_window);

	return;
}

gint main_window_component_update (gint old_component, gint new_component) 
{
	GtkWidget          * component;
	GtkWidget          * component_container;
	GList              * list = NULL;
	GtkWidget          * widget;
	GtkWidget          * main_window = NULL;

	g_return_val_if_fail (main_window_xml, -1);

	component =  main_window_components[new_component].create_component ();

	component_container = glade_xml_get_widget (main_window_xml, "hpaned1");
	
	main_window = glade_xml_get_widget (main_window_xml, "main_window");

	list = gtk_container_get_children (GTK_CONTAINER(component_container));
	widget = GTK_WIDGET(list->next->data);
	g_list_free (list);
	gtk_container_remove (GTK_CONTAINER(component_container), widget);		
	gtk_container_add (GTK_CONTAINER(component_container), component);
	main_window_components[old_component].destroy_component (main_window);

	return new_component;

}


GtkWindow        * main_window_get_active_window        ()
{
	if (! main_window_active_window)
		main_window_active_window = g_list_append (main_window_active_window, main_window_get ());

	return GTK_WINDOW (main_window_active_window->data);
}


void               main_window_set_active_window        (GtkWindow *window)
{
	if (! main_window_active_window)
		main_window_active_window = g_list_append (main_window_active_window, main_window_get ());
	
	main_window_active_window = g_list_prepend (main_window_active_window, window);

	return;
}


void               main_window_unset_active_window      ()
{
	if (! main_window_active_window)
		main_window_active_window = g_list_append (main_window_active_window, main_window_get ());

	main_window_active_window = g_list_remove (main_window_active_window,
						   main_window_active_window->data);

}



//****************************************************************************************************

gboolean main_window_on_delete_event (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	main_window_on_quit_activate (NULL, NULL);

	return TRUE;
}

//****************************************************************************************************

gint __component_group_item_lookup (gint group, gint order)
{
	gint i;
	
	for (i = 0; i < MAIN_WINDOW_COMPONENT_NUMBER; i++)
		if (group == main_window_components[i].group && order == main_window_components[i].order)
			return i;
	
	return -1;	
}

void main_window_sidebar_select (EShortcutBar * shortcut_bar,
				 GdkEvent     * event,
				 gint	        group_num,
				 gint		item_num)
{
	gint                 old_component = main_window_active_component;
	gint                 new_component = __component_group_item_lookup (group_num, item_num);

	if (new_component == -1)
		return;
	

	if (! oseaclient_session_server_exists (main_window_components[new_component].servername))
		new_component = MAIN_WINDOW_ERROR_COMPONENT;

	if (old_component == new_component)
		return;

	main_window_menubar_update (old_component, new_component);
	main_window_toolbar_update (old_component, new_component);
	main_window_component_update (old_component, new_component);

	main_window_active_component = new_component;

	return;
}


//****************************************************************************************************
// Callbacks for File menu



void __main_window_on_log_out_process (AsplDataList * datalist)
{

	StatusBarMessage * status_message = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");
	
	if (status_message)
		main_window_remove_statusbar_message (status_message);

	main_window_set_progressbar (FALSE);
	
	main_window_destroy();

	widgets_set_permission_disabling (FALSE);
	
	login_window_show();

	osea_datalist_free (datalist);
	return;
}

void main_window_on_log_out_activate (gpointer widget, gpointer user_data) 
{	
	gchar            * transaction_event = NULL;
	AsplDataList     * datalist = NULL;
	StatusBarMessage * status_message;

	transaction_event = g_strdup_printf ("logout_transaction %d", ++main_window_transaction_number);
		
	event_source_add_signal (transaction_event);
	event_source_set_callback (transaction_event, (GSourceFunc) __main_window_on_log_out_process);
	
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Logging out..."),0);

	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);
	
	if (! oseaclient_session_logout (event_process_afdal_nuldata, datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while logging out. Exiting locally"));
		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);
		main_window_destroy ();
		widgets_set_permission_disabling (FALSE);
		login_window_show ();
	}
	return;

}

void __main_window_on_quit_process (AsplDataList * datalist)
{
	

	StatusBarMessage * status_message = (StatusBarMessage *) osea_datalist_get (datalist, "MESSAGE");
	
	if (status_message)	
		main_window_remove_statusbar_message (status_message);
	main_window_set_progressbar (FALSE);
	
	osea_datalist_free (datalist);

	main_window_destroy();
	gtk_main_quit ();

}

void main_window_on_quit_activate (gpointer widget, gpointer user_data) 
{	
	gchar            * transaction_event = NULL;
	AsplDataList     * datalist = NULL;
	StatusBarMessage * status_message;

	transaction_event = g_strdup_printf ("quit_transaction %d", ++main_window_transaction_number);
		
	event_source_add_signal (transaction_event);
	event_source_set_callback (transaction_event, (GSourceFunc) __main_window_on_quit_process);
	
	main_window_set_progressbar (TRUE);	
	status_message = main_window_set_statusbar_message (_("Logging out..."),0);

	datalist = osea_datalist_new();
	
	osea_datalist_set_full (datalist, "SIGNAL", transaction_event, g_free);
	osea_datalist_set (datalist, "MESSAGE", status_message);
	
	if (! oseaclient_session_logout (event_process_afdal_nuldata, datalist)) {
		dialog_close_run 
			(main_window_get (), GTK_MESSAGE_ERROR,
			 _("There was a problem while logging out. Exiting locally"));
		main_window_remove_statusbar_message (status_message);
		main_window_set_progressbar (FALSE);
		main_window_destroy ();
		gtk_main_quit ();
	}
	return;
}


// Callbacks for View menu

void main_window_on_toolbar_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data)
{
	GtkWidget *widget;

	widget = glade_xml_get_widget (main_window_xml, "main_window_toolbar_handlebox");
	
	if (! gtk_check_menu_item_get_active (checkmenuitem)) 
		gtk_widget_hide (widget);
	else
		gtk_widget_show (widget);

	return;
}

void main_window_on_statusbar_activate (GtkCheckMenuItem *checkmenuitem, gpointer user_data) 
{ 
	GtkWidget *widget;

	widget = glade_xml_get_widget (main_window_xml, "main_window_statusbar_handlebox");
	
	if (! gtk_check_menu_item_get_active (checkmenuitem)) 
		gtk_widget_hide (widget);
	else
		gtk_widget_show (widget);

	return;
}


// Callbacks for About menu

void main_window_on_about_activate (GtkMenuItem *menuitem, gpointer user_data) 
{
	GtkWindow *main_window = NULL;

	main_window = GTK_WINDOW (glade_xml_get_widget (main_window_xml, "main_window"));

	about_window_show (main_window);

	return;
}


//****************************************************************************************************
// Statusbar stuff

gint __main_windows_statusbar_timeout (gpointer context)
{
	GtkStatusbar *statusbar = NULL;
	StatusBarMessage * message = (StatusBarMessage *) context;

	if (message->message_id) {
	
		statusbar = GTK_STATUSBAR (glade_xml_get_widget (main_window_xml, "main_window_statusbar"));
		
		gtk_statusbar_remove (statusbar, message->context_id, message->message_id);

	}
	g_free (message);

	return FALSE;	
}

void main_window_remove_statusbar_message (StatusBarMessage *message) {
	__main_windows_statusbar_timeout (message);	
	return;
}

StatusBarMessage * main_window_set_statusbar_message (const gchar *message, gint timeout)
{
	GtkStatusbar     * statusbar = NULL;
	StatusBarMessage * bar_message = NULL;
	guint              context_id;
	guint              message_id;	
	
	statusbar = GTK_STATUSBAR (glade_xml_get_widget (main_window_xml, "main_window_statusbar"));
	
	context_id = gtk_statusbar_get_context_id (statusbar, "main_window");
	message_id = gtk_statusbar_push (statusbar, context_id, message);
	
	bar_message = g_new (StatusBarMessage, 1);
	bar_message -> context_id = context_id;
	bar_message -> message_id = message_id;

	if (timeout)
		g_timeout_add (timeout * 1000, __main_windows_statusbar_timeout, bar_message);

	return bar_message;
}


gint __progress_timeout( gpointer data )
{
	GtkProgressBar *progressbar = NULL;
	
	progressbar = GTK_PROGRESS_BAR (glade_xml_get_widget (main_window_xml, "progressbar1"));

	if (progressbar) {
		gtk_progress_bar_pulse (progressbar);
	}
	return TRUE;
} 


void main_window_set_progressbar (gboolean active)
{
	GtkProgressBar *progressbar = NULL;
	
	progressbar = GTK_PROGRESS_BAR (glade_xml_get_widget (main_window_xml, "progressbar1"));
	
	
	if (active) {
		if (progress_timer == 0) {
			progress_timer = g_timeout_add (100, __progress_timeout, NULL);
			if (progressbar)
				gtk_progress_bar_set_pulse_step (progressbar, 0.1);
		}
	} else {
		if (progress_timer != 0) {
			gtk_timeout_remove (progress_timer);
			progress_timer = 0;
			if (progressbar)
				gtk_progress_bar_set_fraction (progressbar, 0);
		}
	}
	
	return;
	
}


