AC_INIT(src/os_tax.c)

OSTAX_VERSION=0.0.90
AC_SUBST(OSTAX_VERSION)

AM_INIT_AUTOMAKE(ostax, $OSTAX_VERSION)

AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC

PKG_CHECK_MODULES(OSTAX, oseacomm oseaclient oseaserver glib-2.0 libgda >= 0.11)

AC_SUBST(OSTAX_CFLAGS)
AC_SUBST(OSTAX_LIBS)

AC_CHECK_HEADER(popt.h, , AC_MSG_ERROR(*** cannot find popt.h header); exit)
AC_CHECK_LIB(popt, poptGetNextOpt, , AC_MSG_ERROR(*** Cannot find popt library))

##################################################
# Check for gtk-doc.
##################################################

AC_ARG_WITH(html-dir, [  --with-html-dir=PATH path to installed docs ])

if test "x$with_html_dir" = "x" ; then
  HTML_DIR='${datadir}/gtk-doc/html'
else
  HTML_DIR=$with_html_dir
fi

AC_SUBST(HTML_DIR)

AC_CHECK_PROG(GTKDOC, gtkdoc-mkdb, true, false)

gtk_doc_min_version=0.6
if $GTKDOC ; then 
    gtk_doc_version=`gtkdoc-mkdb --version`
    AC_MSG_CHECKING([gtk-doc version ($gtk_doc_version) >= $gtk_doc_min_version])
    if perl <<EOF ; then
      exit (("$gtk_doc_version" =~ /^[[0-9]]+\.[[0-9]]+$/) &&
            ("$gtk_doc_version" >= "$gtk_doc_min_version") ? 0 : 1);
EOF
      AC_MSG_RESULT(yes)
   else
      AC_MSG_RESULT(no)
      GTKDOC=false
   fi
fi

dnl Let people disable the gtk-doc stuff.
AC_ARG_ENABLE(gtk-doc, [  --enable-gtk-doc  Use gtk-doc to build documentation [default=auto]], enable_gtk_doc="$enableval", enable_gtk_doc=auto)

if test x$enable_gtk_doc = xauto ; then
  if test x$GTKDOC = xtrue ; then
    enable_gtk_doc=yes
  else
    enable_gtk_doc=no 
  fi
fi

AM_CONDITIONAL(ENABLE_GTK_DOC, test x$enable_gtk_doc = xyes)


AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
])



