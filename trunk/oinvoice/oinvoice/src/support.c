//  oinvoice: support functions
//  Copyright (C) 2002, 2003  Francis Brosnan Blázquez
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
#include "support.h"

AsplDataList *  support_create_callback (SupportFunc source_func)
{
	gchar        * base_signal = "support";
	gchar        * signal;
	AsplDataList * datalist;
	
	signal = event_source_arm_signal (base_signal, (GSourceFunc) source_func);

	datalist = osea_datalist_new ();

	osea_datalist_set_full (datalist, "SIGNAL", signal, g_free);

	return datalist;
}

