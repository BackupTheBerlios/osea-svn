//  Af-Kernel server: 
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

#include "os_kernel_crypto.h"
#include <openssl/evp.h>
#include <string.h>

GString *  os_kernel_crypto_md5_sum        (gchar * string)
{
	GString      * result;
	gchar          buffer[EVP_MAX_MD_SIZE];
	EVP_MD_CTX     mdctx;
	const EVP_MD * md = EVP_md5 ();
	gint           i;
#ifdef __SSL_0_97__
	gint           md_len;
#else
	gint           md_len = EVP_MAX_MD_SIZE;
#endif

	g_return_val_if_fail (string, NULL);

	OpenSSL_add_all_digests();
	
#ifdef __SSL_0_97__
	EVP_MD_CTX_init(&mdctx);
	EVP_DigestInit_ex(&mdctx, md, NULL);
	EVP_DigestUpdate(&mdctx, string, strlen(string));
	EVP_DigestFinal_ex(&mdctx, buffer, &md_len);
	EVP_MD_CTX_cleanup(&mdctx);
#else
	EVP_DigestInit (&mdctx, md);
	EVP_DigestUpdate (&mdctx, string, strlen(string));
	EVP_DigestFinal (&mdctx, buffer, &md_len);
#endif

	result = g_string_new (NULL);
	
	for (i = 0; i < md_len; i++ ) {
		g_string_append_printf (result, "%02hhx", buffer[i]);
	}

	return result;
}
