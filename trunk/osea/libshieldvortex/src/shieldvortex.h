/* LibShieldVortex: TLS and SASL profiles for Vortex Library
   Copyright (c) 2005 David Marín Carreño <davefx@davefx.is-a-geek.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __SHIELDVORTEX_H__
#define __SHIELDVORTEX_H__

#include <vortex.h>
#include "shieldvortex_tls.h"
#include "shieldvortex_sasl.h"

void shieldvortex_init ();

void shieldvortex_exit ();

#endif
