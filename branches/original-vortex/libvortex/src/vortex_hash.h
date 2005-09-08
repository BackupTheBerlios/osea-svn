//  LibVortex:  A BEEP (RFC3080/RFC3081) implementation.
//  Copyright (C) 2005 Advanced Software Production Line, S.L.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public License
//  as published by the Free Software Foundation; either version 2.1 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this program; if not, write to the Free
//  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
//  02111-1307 USA
//  
//  You may find a copy of the license under this software is released
//  at COPYING file. This is LGPL software: you are wellcome to
//  develop propietary applications using this library withtout any
//  royalty or fee but returning back any change, improvement or
//  addition in the form of source code, project image, documentation
//  patches, etc. 
//
//  You can also contact us to negociate other license term you may be
//  interested different from LGPL. Contact us at:
//          
//      Postal address:
//         Advanced Software Production Line, S.L.
//         C/ Dr. Michavila Nº 14
//         Coslada 28820 Madrid
//         Spain
//
//      Email address:
//         info@aspl.es - http://fact.aspl.es
//
#ifndef __VORTEX_HASH_H__
#define __VORTEX_HASH_H__

#include <glib.h>
#include <vortex.h>


VortexHash * vortex_hash_new_full (GHashFunc      hash_func,
				   GEqualFunc     key_equal_func,
				   GDestroyNotify key_destroy_func,
				   GDestroyNotify value_destroy_func);

VortexHash * vortex_hash_new      (GHashFunc  hash_func,
				   GEqualFunc key_equal_func);

void         vortex_hash_insert   (VortexHash *hash_table,
				   gpointer    key,
				   gpointer    value);

void         vortex_hash_replace  (VortexHash *hash_table,
				   gpointer    key,
				   gpointer    value);

gint         vortex_hash_size     (VortexHash   *hash_table);

gpointer     vortex_hash_lookup   (VortexHash   *hash_table,
				   gconstpointer key);

gboolean     vortex_hash_remove   (VortexHash   *hash_table,
				   gconstpointer key);

void         vortex_hash_destroy  (VortexHash *hash_table);


void         vortex_hash_foreach  (VortexHash *hash_table,
				   GHFunc func,
				   gpointer user_data);

#endif
