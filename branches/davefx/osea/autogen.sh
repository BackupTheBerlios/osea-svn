#!/bin/sh

PACKAGE="Af-Arch: N-tier modular architecture for business application development"

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have automake installed to compile $PACKAGE";
	echo;
	exit;
}

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have autoconf installed to compile $PACKAGE";
	echo;
	exit;
}

echo "Generating configuration files for $PACKAGE, please wait...."
echo;

aclocal $ACLOCAL_FLAGS;
autoheader;
automake --add-missing;
autoconf;
libtoolize;

./configure $@ --enable-maintainer-mode --enable-compile-warnings
