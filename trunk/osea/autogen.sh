#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="öSea system"
AUTOCONF=autoconf
REQUIRED_AUTOCONF_VERSION=2.59
AUTOMAKE=automake-1.6
REQUIRED_AUTOMAKE_VERSION=1.6

gnome_autogen=`which gnome-autogen.sh`
test -z "$gnome_autogen"

USE_GNOME2_MACROS=1 . $gnome_autogen

