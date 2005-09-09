dnl  LibVortex:  A BEEP (RFC3080/RFC3081) implementation.
dnl  Copyright (C) 2005 Advanced Software Production Line, S.L.
dnl
dnl  This program is free software; you can redistribute it and/or
dnl  modify it under the terms of the GNU Lesser General Public License
dnl  as published by the Free Software Foundation; either version 2.1 of
dnl  the License, or (at your option) any later version.
dnl
dnl  This program is distributed in the hope that it will be useful,
dnl  but WITHOUT ANY WARRANTY; without even the implied warranty of 
dnl  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
dnl  GNU Lesser General Public License for more details.
dnl
dnl  You should have received a copy of the GNU Lesser General Public
dnl  License along with this program; if not, write to the Free
dnl  Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
dnl  02111-1307 USA
dnl  
dnl  You may find a copy of the license under this software is released
dnl  at COPYING file. This is LGPL software: you are wellcome to
dnl  develop propietary applications using this library withtout any
dnl  royalty or fee but returning back any change, improvement or
dnl  addition in the form of source code, project image, documentation
dnl  patches, etc. 
dnl
dnl  You can also contact us to negociate other license term you may be
dnl  interested different from LGPL. Contact us at:
dnl          
dnl      Postal address:
dnl         Advanced Software Production Line, S.L.
dnl         C/ Dr. Michavila Nº 14
dnl         Coslada 28820 Madrid
dnl         Spain
dnl
dnl      Email address:
dnl         info@aspl.es - http://fact.aspl.es
dnl

AC_INIT(src/vortex.h)

VORTEX_VERSION=`cat VERSION`
AC_SUBST(VORTEX_VERSION)

AM_INIT_AUTOMAKE(vortex, $VORTEX_VERSION)
AC_CONFIG_AUX_DIR(.)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(LIBRARIES, libxml-2.0 glib-2.0 >= 2.6 gthread-2.0) 

dnl check if vortex-client should be builded

AC_ARG_ENABLE(vortex-client, [  --enable-vortex-client     Enable vortex-client tool building [default=yes]], enable_vortex_client="$enableval", enable_vortex_client=yes)
if test x$enable_vortex_client = xyes ; then
AC_CHECK_HEADER(readline/readline.h, readline_found=true, readline_found=false)
     AC_TRY_LINK(,,[	READLINE_DEPLIBS="-lreadline" ],[ 
		LIBS="$LIBS -ltermcap" 
		AC_TRY_LINK(,,[ READLINE_DEPLIBS="-lreadline -ltermcap" ],[
			AC_MSG_ERROR([Cannot figure out how to link with the readline library; see config.log for more information])
		])
	])
fi
AM_CONDITIONAL(ENABLE_READLINE, test "x$readline_found" = "xtrue")

AC_ARG_VAR(READLINE_PATH, [Readline base include path. This can be helpful to set a different location for a readline installation.
This will be used to build a include path as READLINE_PATH/include and a READLINE_PATH/lib.
])

dnl check for doxygen documentation
AC_ARG_ENABLE(vortex-doc, [  --enable-vortex-doc     Enable building Vortex Library documentation (doxygen required) [default=yes]], enable_vortex_doc="$enableval", enable_vortex_doc=yes)
if test x$enable_vortex_doc = xyes ; then
        AC_CHECK_PROG(DOXYGEN, doxygen, "yes", "no")
fi
AM_CONDITIONAL(ENABLE_AF_ARCH_DOC, test "x$DOXYGEN" = "xyes")

AC_SUBST(LIBRARIES_CFLAGS)
AC_SUBST(LIBRARIES_LIBS)

AC_OUTPUT([
Makefile
src/Makefile
doc/Makefile
doc/vortex-doc.doxygen
test/Makefile
data/Makefile
logo/Makefile
vortex.pc])

echo "------------------------------------------"
echo "--       VORTEX LIBRARY SETTINGS        --"
echo "------------------------------------------"
echo
if test x$readline_found = xtrue ; then
   echo "   Build vortex-client tool:   enabled"
else
   echo "   Build vortex-client tool:   disabled"
   echo 
   echo "     NOTE: To enable vortex-client tool building";
   echo "     it is needed to have readline on this system. ";
   echo "     Once installed run again configure process."
fi
echo
if test x$DOXYGEN = xyes ; then
   echo "   Build Vortex Library doc:   enabled"
else	    
   echo "   Build Vortex Library doc:   disabled"
   echo  
   echo "     NOTE: To enable Vortex documentation you will";
   echo "     have to install doxygen tool. Then run again";
   echo "     configure process.";
fi
echo
echo "------------------------------------------"
echo "--       NOW TYPE: make; make install   --"
echo "------------------------------------------"
