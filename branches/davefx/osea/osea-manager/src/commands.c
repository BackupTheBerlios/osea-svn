//  ASPL Clm: Command Line Manager for ASPL Fact system
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

#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "help.h"
#include "connection.h"
#include "user.h"
#include "permission.h"
#include "group.h"
#include "servers.h"

typedef struct _CommandNode {
	CommandCallback  handler;
	gint             score;
}CommandNode;

gboolean commands_quit (gchar * line, gpointer data);

// Adde here all allowed command an which function is executed
AllowedCommands commands_allowed [] = {{"[General options]", NULL, FALSE, NULL, NULL}, 
	                               {"help",            "?",  TRUE, "This help ...", 
					help_print},
				       {"open",            "?",  TRUE, "open 'server name': opens a connection to a kernel server", 
					connection_open},
				       {"quit",            "q",  TRUE, "exits from aspl-clm", 
					commands_quit},
				       {"[Users and Groups managing]", NULL, FALSE, NULL, NULL}, 
				       {"add user",        NULL, TRUE, "add user ['name']: creates a new user", 
					user_new},
				       {"del user",        NULL, TRUE, "del user ['user_id']: removes an already created user", 
					user_remove},
				       {"list user",       NULL, TRUE, "list user: list all created users", 
					user_list},
				       {"add group",        NULL, TRUE, "add group ['name']: creates a new group", 
					group_new},
				       {"add group user",       NULL, TRUE, "add group user ['user_id']: Add an user to a group", 
					group_add_user},
				       {"del group",        NULL, TRUE, "del group ['group_id']: removes an already created group", 
					group_remove},
				       {"del group user",       NULL, TRUE, "add group user ['user_id']: Add an user to a group", 
					group_del_user},
				       {"list group",       NULL, TRUE, "list group: list all created groups", 
					group_list},
				       {"list group user",       NULL, TRUE, "list group user ['group_id']: list all created user in group_id", 
					group_list_by_user},
				       {"[Permission managing]", NULL, FALSE, NULL, NULL}, 
				       {"list perm user",  NULL, TRUE, "list user permission ['user_id']: gets all permissions for user_id", 
					permission_list_by_user},
				       {"list perm group", NULL, TRUE, "list group permission ['group_id']: gets all permissions for group_id", 
					permission_list_by_group},
				       {"list perm",       NULL, TRUE, "list permission: gets all permissions available", 
					permission_list},
				       {"set perm user",       NULL, TRUE, "set perm user ['user_id']: sets a perm to user_id", 
					permission_set_user},
				       {"unset perm user",       NULL, TRUE, "unset perm user ['user_id']: unsets a perm to user_id", 
					permission_unset_user},
				       {"set perm group",       NULL, TRUE, "set perm group ['group_id']: sets a perm to group_id", 
					permission_set_group},
				       {"unset perm group",       NULL, TRUE, "unset perm group ['group_id']: unsets a perm to group_id", 
					permission_unset_group},
				       {"[Servers managing]", NULL, FALSE, NULL, NULL}, 
				       {"list servers",       NULL, TRUE, "list servers: list all available servers at af-kernel", 
					servers_list},
				       {"del server",       NULL, TRUE, "del server ['server_id']: removes the given server.", 
					servers_remove},
				       {NULL, NULL, TRUE, NULL, NULL}};

gboolean commands_quit (gchar * line, gpointer data)
{
	g_print ("Exiting..\n");
	exit (0);
}

AllowedCommands * commands_get ()
{
	return commands_allowed;
}

gboolean          commands_is_allowed (AllowedCommands * commands, gchar * command)
{
	gint i = 0;

	g_return_val_if_fail (commands, FALSE);
	g_return_val_if_fail (command, FALSE);

	while (commands[i].short_name != NULL) {
		// Check for complete command name
		if (!g_strncasecmp (commands[i].short_name, command, strlen (commands[i].short_name))) 
			return TRUE;
		
		// Check for bind command name
		if (commands[i].bind_name)
			if (!g_strncasecmp (commands[i].bind_name, command, strlen (commands[i].bind_name))) 
				return TRUE;
		i++;
	}
	return FALSE;
	
} 


gint commands_compare_func (gconstpointer value_a, gconstpointer value_b) 
{
	CommandNode * node_a = (CommandNode *) value_a;
	CommandNode * node_b = (CommandNode *) value_b;
	
	if (node_a->score > node_b->score) 
		return -1;
	if (node_a->score < node_b->score) 
		return 1;
	return 0;
}

CommandCallback   commands_get_callback (AllowedCommands * commands, gchar * command)
{
	gint i = 0;
	GList           * commands_to_be_executed = NULL, * aux_list = NULL;
	CommandNode     * node;	
	CommandCallback   callback;
	

	g_return_val_if_fail (commands, FALSE);
	g_return_val_if_fail (command, FALSE);


	while (commands[i].short_name != NULL) {
		// Check for complete command name
		if (!g_strncasecmp (commands[i].short_name, command, strlen (commands[i].short_name))) {
			node = g_new0 (CommandNode, 1);
			node->handler = commands[i].callback;
			node->score = strlen (commands[i].short_name);
			commands_to_be_executed = g_list_append (commands_to_be_executed, node);
		}

		// Check for bind command name
		if (commands[i].bind_name)
			if (!g_strncasecmp (commands[i].bind_name, command, strlen (commands[i].bind_name))) {
				node = g_new0 (CommandNode, 1);
				node->handler = commands[i].callback;
				node->score = strlen (commands[i].short_name);
				commands_to_be_executed = g_list_append (commands_to_be_executed, node);
			}
			
		i++;
	}

	commands_to_be_executed = g_list_sort (commands_to_be_executed, commands_compare_func);
	


	node = (CommandNode *) commands_to_be_executed->data;
	callback = node->handler;

	for (aux_list = g_list_first (commands_to_be_executed); aux_list; aux_list = g_list_next (aux_list)) {
		node = (CommandNode *)aux_list->data;
		g_free (node);
	}

	g_list_free (commands_to_be_executed);

	return callback; 
}
