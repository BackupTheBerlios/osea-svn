#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="GNOME Data Access"
AUTOCONF=autoconf
REQUIRED_AUTOCONF_VERSION=2.59
AUTOMAKE=automake-1.9
REQUIRED_AUTOMAKE_VERSION=1.9

(test -f $srcdir/configure.in \
  && test -d $srcdir/idl \
  && test -f $srcdir/idl/GNOME_Database.idl) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level GDA directory"
    exit 1
}

gnome_autogen=`which gnome-autogen.sh`
test -z "$gnome_autogen"

USE_GNOME2_MACROS=1 . $gnome_autogen

