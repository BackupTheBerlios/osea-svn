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

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <glib.h>

typedef gboolean (* CommandCallback) (gchar * line, gpointer data);

typedef struct _AllowedCommands {
	gchar           * short_name;
	gchar           * bind_name;
	gboolean          executable;
	gchar           * command_help;
	CommandCallback   callback;
} AllowedCommands;


AllowedCommands * commands_get ();

gboolean          commands_is_allowed   (AllowedCommands * commands, gchar * command);

CommandCallback   commands_get_callback (AllowedCommands * commands, gchar * command);

#endif
