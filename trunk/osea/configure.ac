#
# öSea system. Released under the GPL (see README file).
#
# (c) 2003 Advanced Software Production Line, S.L.
# (c) 2004 David Marín Carreño
#

AC_INIT(liboseaclient/src/oseaclient.h)

OSEA_VERSION=0.0.90
AC_SUBST(OSEA_VERSION)

AM_INIT_AUTOMAKE(osea, $OSEA_VERSION)

AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL

GOB2_CHECK(2.0.0)

PKG_CHECK_MODULES(LIBRARIES, vortex >= 0.8  glib-2.0 gthread-2.0)
AC_SUBST(LIBRARIES_CFLAGS)
AC_SUBST(LIBRARIES_LIBS)

#PKG_CHECK_MODULES(OSKERNEL, roadrunner >= 0.9 glib-2.0 libgda >= 0.11)
#AC_SUBST(OSKERNEL_CFLAGS)
#AC_SUBST(OSKERNEL_LIBS)

#PKG_CHECK_MODULES(OSEAMANAGER, roadrunner >= 0.9 glib-2.0)
#AC_SUBST(OSEAMANAGER_CFLAGS)
#AC_SUBST(OSEAMANAGER_LIBS)

#######################
# Check for readline.
########################
AC_CHECK_HEADER(readline/readline.h, , AC_MSG_ERROR(*** Cannot find libreadline headers))
#AC_CHECK_LIB(readline, readline, , AC_SEARCH_LIBS(readline, [readline libs], ,AC_MSG_ERROR(*** Cannot find readline library)))
old_LIBS=$LIBS
LIBS="-lreadline"

AC_TRY_LINK(,,[	READLINE_DEPLIBS="-lreadline" ],[ 
		LIBS="$LIBS -ltermcap" 
		AC_TRY_LINK(,,[ READLINE_DEPLIBS="-lreadline -ltermcap" ],[
			AC_MSG_ERROR([Cannot figure out how to link with the readline library; see config.log for more information])
		])
	])

LIBS=$old_LIBS

AC_SUBST(READLINE_DEPLIBS)

#######################
# Check for libssl
########################
AC_MSG_CHECKING(for LibSSL development files)
AC_CHECK_HEADER(openssl/ssl.h, , AC_MSG_ERROR(*** cannot find ssl.h header); exit)
AC_CHECK_LIB(ssl, SSL_library_init, , AC_MSG_ERROR(*** Cannot find ssl library))
AC_CHECK_HEADER(openssl/rsa.h, , AC_MSG_ERROR(*** cannot find ssl crypto header); exit)

dnl Check commented due to get support for 0.96 openssl library.
dnl AC_CHECK_LIB(crypto, EVP_MD_CTX_init, , AC_MSG_ERROR(*** Cannot find ssl 0.9.7 crypto library))


##################################################
# Check for gtk-doc.
##################################################
GTK_DOC_CHECK

#################################################################
# Links to compile the whole proyect before installing anything
#################################################################
test -h liboseacomm/liboseacomm        || ln -s src        liboseacomm/liboseacomm
test -h liboseaclient/liboseaclient    || ln -s src      liboseaclient/liboseaclient
test -h liboseaserver/liboseaserver    || ln -s src      liboseaserver/liboseaserver
#test -h libaoskernel/libaoskernel      || ln -s src       libaoskernel/libaoskernel

#################################################################
# Links to generate gtk-doc documentation properly
#################################################################
test -h doc/reference/src/liboseacomm        || ln -s ../../../liboseacomm/src      doc/reference/src/liboseacomm
test -h doc/reference/src/liboseaclient         || ln -s ../../../liboseaclient/src       doc/reference/src/liboseaclient
test -h doc/reference/src/liboseaserver          || ln -s ../../../liboseaserver/src        doc/reference/src/liboseaserver
#test -h doc/reference/src/libaoskernel   || ln -s ../../../libaoskernel/src doc/reference/src/libaoskernel
test -h include_dir || ln -s doc/reference/src include_dir

AC_OUTPUT([
Makefile
libshieldvortex/Makefile
libshieldvortex/src/Makefile
libshieldvortex/test/Makefile
liboseatype/Makefile
liboseatype/src/Makefile
liboseatype/test/Makefile
liboseacomm/Makefile
liboseacomm/src/Makefile
liboseacomm/test/Makefile
liboseacomm/data/Makefile
liboseaclient/Makefile
liboseaclient/src/Makefile
liboseaclient/test/Makefile
liboseaserver/Makefile
liboseaserver/src/Makefile
liboseaserver/test/Makefile
#libaoskernel/Makefile
#libaoskernel/src/Makefile
#libaoskernel/test/Makefile
os-kernel/Makefile
os-kernel/src/Makefile
os-kernel/data/Makefile
osea-manager/Makefile
osea-manager/src/Makefile
libshieldvortex/libshieldvortex.pc
liboseatype/liboseatype.pc
liboseacomm/liboseacomm.pc
liboseaclient/liboseaclient.pc
liboseaserver/liboseaserver.pc
#libaoskernel/libaoskernel.pc
doc/Makefile
doc/reference/Makefile
])



