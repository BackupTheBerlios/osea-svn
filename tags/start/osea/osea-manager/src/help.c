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

#include "help.h"
#include "commands.h"
#include <glib.h>


gboolean help_print (gchar * line, gpointer data)
{
	AllowedCommands * commands_allowed = NULL;
	gint i = 0;

	commands_allowed = commands_get ();
	g_print ("\n");
	while (commands_allowed[i].short_name != NULL) {
		if (commands_allowed[i].executable)
			g_print ("  %-16s %s\n", commands_allowed[i].short_name, commands_allowed[i].command_help);
		else
			g_print ("\n  %-16s\n", commands_allowed[i].short_name);
		i++;
	}
	return TRUE;
}

void help_initial_help ()
{
	g_print ("ASPL Clm: Command Line Manager for ASPL Fact system\n");
	g_print ("Copyright (C) 2002, 2003  Advanced Software Production Line, S.L.\n\n");
	g_print ("Type 'help' or '?' for help\n");

	return;
}

