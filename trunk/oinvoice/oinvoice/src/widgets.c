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

#include "widgets.h"
#include <string.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <liboseaclient/oseaclient.h>

#define LOG_DOMAIN G_LOG_DOMAIN

#define INSUFFICIENT_PERM_MESSAGE _("You don't have sufficient permissions to do this action.")

gboolean __widgets_permission_disabling = FALSE;

typedef struct {
	gchar * name;
	gchar * server;
} WidgetsPermission;

typedef struct __WidgetsPermissionTreeNode {
	gchar             * name;
	GList             * permissions_list;
} WidgetsPermissionTreeNode;

typedef GTree WidgetsPermissionTree;


gint    __widgets_compare_string (gconstpointer a, gconstpointer b, gpointer data)
{
	return g_strcasecmp ((gchar *) a, (gchar *)b);
}

void    __widgets_permission_destroy (gpointer data)
{
	WidgetsPermission * perm = (WidgetsPermission *) data;
	
	g_free (perm->name);
	g_free (perm->server);
	g_free (perm);
}

void    __widgets_permission_tree_node_destroy (gpointer data)
{
	WidgetsPermissionTreeNode *node = (WidgetsPermissionTreeNode *) data;
	GList * cursor;

	g_free (node->name);

	cursor = g_list_first(node->permissions_list);

	while (cursor) {
		__widgets_permission_destroy (cursor->data);
		cursor = cursor->next;
	}
	
	g_list_free (node->permissions_list);
	
	g_free (node);

}



void       widgets_set_permission_disabling (gboolean active)
{
	__widgets_permission_disabling = active;

	return;
}



gboolean widgets_validate_permission_xml (const gchar *filename)
{

	xmlDtdPtr dtd;
	gchar *dtd_file_name = NULL;
	xmlValidCtxtPtr context;
	xmlDocPtr doc;

	g_return_val_if_fail (filename, FALSE);

	dtd_file_name = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "xpd.dtd", NULL );
	if (!dtd_file_name) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Cannot find the dtd file to validate permission_xmls");
		g_free (dtd_file_name);
		return FALSE;
	}

	dtd = xmlParseDTD (NULL, dtd_file_name);
	g_free (dtd_file_name);

	if (!dtd) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Cannot parse the dtd file");
		return FALSE;
	}

	// The DTD is correct

	doc = xmlParseFile (filename);
	if (!doc) {
		g_log (LOG_DOMAIN, G_LOG_LEVEL_CRITICAL, "Cannot parse xml permission_xml to validate it");
		xmlFreeDtd (dtd);
		return FALSE;
	}

	// doc correct

	context = xmlNewValidCtxt();

	if (xmlValidateDtd (context, doc, dtd)) {
		// Validation succesful
		xmlFreeDtd (dtd);
		xmlFreeDoc (doc);
		xmlFreeValidCtxt (context);
		return TRUE;
	} 
	
	// Validation failed
	xmlFreeDtd (dtd);
	xmlFreeDoc (doc);
	xmlFreeValidCtxt (context);
	return FALSE;
	
}



WidgetsPermissionTree * widgets_parse_message (const gchar *filename)
{
	xmlDocPtr document;
	xmlNodePtr cursor;
	xmlNodePtr perm_cursor;
	xmlChar * string;
	WidgetsPermissionTree * result = NULL;
	WidgetsPermissionTreeNode * node;
	WidgetsPermission * perm;
	
	
	g_return_val_if_fail (filename, NULL);

	// Load the xml document
	document = xmlParseFile (filename);
	if (! document) {
		g_printerr ("Unable to parse the given document\n");
		result = NULL;
		goto exit_widgets_parse_message;
	}

	// Get the root element ("xpd")
	cursor = xmlDocGetRootElement (document);
	
	cursor = cursor->children;
	if (! cursor) {
		result = NULL;
		goto exit_widgets_parse_message;
	}

	while ((cursor) && (cursor->type != XML_ELEMENT_NODE))
		cursor = cursor->next;

	if (! cursor) {
		result = NULL;
		goto exit_widgets_parse_message;
	}

	// We create the result tree

	result = g_tree_new_full (__widgets_compare_string, NULL, 
				  NULL, __widgets_permission_tree_node_destroy);


	// We traverse all the widgets

	while (cursor) {
		node = g_new0 (WidgetsPermissionTreeNode, 1);
		
		string = xmlGetProp (cursor, "name");
		node->name = g_strdup ((gchar *) string);
		xmlFree (string);
		g_tree_insert (result, node->name, node);


		// We enter the widget name
	
		perm_cursor = cursor->children;
		while ((perm_cursor) && (perm_cursor->type != XML_ELEMENT_NODE))
			perm_cursor = perm_cursor->next;

		while (perm_cursor) {
			perm = g_new0 (WidgetsPermission, 1);		
			string = xmlGetProp (perm_cursor, "name");
			perm->name = g_strdup ((gchar *) string);
			xmlFree (string);

			string = xmlGetProp (perm_cursor, "server");
			perm->server = g_strdup ((gchar *) string);
			xmlFree (string);
			
			node->permissions_list = g_list_append (node->permissions_list, perm);

			perm_cursor = perm_cursor->next;
			while ((perm_cursor) && (perm_cursor->type != XML_ELEMENT_NODE))
				perm_cursor = perm_cursor->next;
		}
		
		cursor = cursor->next;
		while ((cursor) && (cursor->type != XML_ELEMENT_NODE))
			cursor = cursor->next;
	}

 exit_widgets_parse_message:
	if (document)
		xmlFreeDoc (document);

	return result;

}



WidgetsPermissionTree * widgets_load_permission_file (const gchar *filename)
{
	WidgetsPermissionTree * result = NULL;


	if (! widgets_validate_permission_xml (filename)) {
		return NULL;
	}

	result = widgets_parse_message (filename);

	return result;
}


void __widgets_disable_widget (gpointer wid)
{
	GtkWidget *widget = GTK_WIDGET (wid);
	
	gtk_widget_set_sensitive (widget, FALSE);
}

void      widgets_manage_permission_tooltip (GtkWidget *widget, gboolean active)
{
	GtkTooltipsData           * tooltips_data = NULL;
	GtkTooltips               * tooltips = NULL;
	gchar                     * new_tooltip = NULL;
	gchar                    ** string_vector = NULL;


	tooltips_data = gtk_tooltips_data_get (widget);
	
	if (active) {

		if (! tooltips_data) {
			tooltips = gtk_tooltips_new ();
			gtk_tooltips_enable (tooltips);
			gtk_tooltips_set_tip (tooltips, widget, INSUFFICIENT_PERM_MESSAGE, NULL);
			return;
		} 
			
		if (! g_str_has_suffix (tooltips_data->tip_text, INSUFFICIENT_PERM_MESSAGE)){
			new_tooltip = g_strdup_printf ("%s\n%s", tooltips_data->tip_text,
						       INSUFFICIENT_PERM_MESSAGE);
			tooltips_data->tip_text = g_renew (gchar,
							   tooltips_data->tip_text,
							   strlen (new_tooltip) + 1);
			tooltips_data->tip_text = strcpy (tooltips_data->tip_text,
							  new_tooltip);
			
			g_free (new_tooltip);
			return;
		}
		
		return;
	} 

	/* active == FALSE */

	if (! tooltips_data)
		return;

	if (! g_str_has_suffix (tooltips_data->tip_text, INSUFFICIENT_PERM_MESSAGE))
		return;

	string_vector = g_strsplit (tooltips_data->tip_text, INSUFFICIENT_PERM_MESSAGE, -1);

	tooltips_data->tip_text = g_renew (gchar,
					   tooltips_data->tip_text,
					   strlen(string_vector[1]) + 1);

	tooltips_data->tip_text = strcpy (tooltips_data->tip_text,
					  string_vector[1]);

	g_strfreev (string_vector);

}

gboolean __widgets_tree_print (gpointer wname, gpointer tree_node, gpointer data)
{
	gchar                     * widget_name =  (gchar *) wname;
	WidgetsPermissionTreeNode * node = (WidgetsPermissionTreeNode *) tree_node;
	GList                     * cursor;
	WidgetsPermission         * permission;

	g_printerr ("\nDisabling widget: %s with permission:\n", widget_name);

	cursor = g_list_first(node->permissions_list);
	while (cursor) {
		permission = (WidgetsPermission *) cursor->data;
		g_printerr ("\t%s/%s\n", permission->server, permission->name);
		cursor = cursor->next;
	}
	
	return FALSE;
}


gboolean __widgets_disable (gpointer wname, gpointer tree_node, gpointer gladexml)
{
	gchar                     * widget_name =  (gchar *) wname;
	WidgetsPermissionTreeNode * node = (WidgetsPermissionTreeNode *) tree_node;
	GladeXML                  * widget_tree = (GladeXML *) gladexml;
	GtkWidget                 * widget;
	GList                     * cursor;
	WidgetsPermission         * permission;

	g_return_val_if_fail (widget_name, FALSE);


	cursor = g_list_first(node->permissions_list);
	while (cursor) {
		permission = (WidgetsPermission *) cursor->data;

		widget = glade_xml_get_widget (widget_tree, widget_name);
		if (! widget)
			return FALSE;

		if ((! oseaclient_session_get_permission (permission->server, permission->name)) &&
		    (GTK_WIDGET_IS_SENSITIVE (widget))) {

			gtk_widget_set_sensitive (widget, FALSE);			
		}

		cursor = cursor->next;
	}
	

	return FALSE;
}



GladeXML * widgets_load (const gchar *fname, const gchar *root, const gchar *domain)
{
	GladeXML               * result;
	WidgetsPermissionTree  * permission_tree;

	gchar    * filename = g_strdup_printf ("%s.glade", fname);

	result = glade_xml_new (filename, root, domain);

	g_free (filename);

	glade_xml_signal_autoconnect (result);

	if (__widgets_permission_disabling) {				

		filename = g_strdup_printf ("%s.xpd", fname);
		
		// Open XML file, check and parse it.

		permission_tree = widgets_load_permission_file (filename);

		g_free (filename);

		if (! permission_tree) {
			return result;
		}

		// Go through the tree widget, checking if there are
		// permissions for the widget
		
		g_tree_foreach (permission_tree, __widgets_disable, result);
		

		g_tree_destroy (permission_tree);
		
	}

	return result;
}



void       widgets_set_sensitive (const gchar *xpd_base_name, GtkWidget *widget, gboolean value)
{

	WidgetsPermissionTree     * permission_tree = NULL;
	gchar                     * basename = NULL;
	gchar                     * filename = NULL;
	WidgetsPermissionTreeNode * node = NULL;
	GList                     * cursor;
	WidgetsPermission         * permission;

	g_return_if_fail (widget);

	if (__widgets_permission_disabling && value) {

		basename = g_build_filename (PACKAGE_DATA_DIR, "oinvoice", "glade", xpd_base_name, NULL );
		filename = g_strdup_printf ("%s.xpd", basename);	
		g_free (basename);

		permission_tree = widgets_load_permission_file (filename);

		g_free (filename);

		if (! permission_tree)
			goto widget_set_sensitive_exit;		

		node = (WidgetsPermissionTreeNode *) g_tree_lookup (permission_tree, 
								    gtk_widget_get_name (widget));

		if (! node)
			goto widget_set_sensitive_exit;


		cursor = g_list_first(node->permissions_list);
		while (cursor) {
			permission = (WidgetsPermission *) cursor->data;

			if (! oseaclient_session_get_permission (permission->server, permission->name)) {
				widgets_manage_permission_tooltip (widget, TRUE);
				value = FALSE;
				goto widget_set_sensitive_exit;
			}
			cursor = cursor->next;
		}
	}
			
 widget_set_sensitive_exit:
	
	if (value)
		widgets_manage_permission_tooltip (widget, FALSE);

	gtk_widget_set_sensitive (widget, value);
	

	if (permission_tree)
		g_tree_destroy (permission_tree);

	return;
}
