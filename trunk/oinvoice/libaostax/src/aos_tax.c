//  LibAfdalTax: interface library to the tax daemonstdin: is not a tty

//  Copyright (C) 2002  Advanced Software Production Line, S.L.
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

#include "aos_tax.h"
#include <glib.h>
#include <librr/rr.h>
#include <config.h>
#include <time.h>

#define LOG_DOMAIN "AFDAL_TAX"

void afdal_tax_print_version_info ()
{
	glong cd = COMPILATION_DATE;

	g_print ("\tLibAfDalTax version %s, compiled %s", VERSION, ctime(&cd));
}


