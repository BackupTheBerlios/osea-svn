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

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "oseaserver.h"

void oseaserver_signal_handler_exit (int value)
{

	oseaserver_command_close_connection ();

	oseaserver_main_abort ("\nExiting due to a recieved signal: %d\n", value);
	
	return;
}

void oseaserver_signal_handler_init ()
{
	
	signal (SIGTERM, oseaserver_signal_handler_exit);
	signal (SIGQUIT, oseaserver_signal_handler_exit);
	signal (SIGKILL, oseaserver_signal_handler_exit);
	signal (SIGSEGV, oseaserver_signal_handler_exit);
	return;
}






