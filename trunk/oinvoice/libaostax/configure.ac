AC_INIT(src/aos_tax.h)

AOSTAX_VERSION=0.0.90
AC_SUBST(AOSTAX_VERSION)

AM_INIT_AUTOMAKE(aostax, $AOSTAX_VERSION)

AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(AOSTAX, oseaclient oseacomm glib-2.0 )

GOB2_CHECK(2.0.0)

AC_SUBST(AOSTAX_CFLAGS)
AC_SUBST(AOSTAX_LIBS)

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

    IFS="${IFS= 	}"; gtk_save_IFS="$IFS"; IFS="."
    set $gtk_doc_version
    for min in $gtk_doc_min_version ; do
        cur=$1; shift
        if test -z $min ; then break; fi
        if test -z $cur ; then GTKDOC=false; break; fi
        if test $cur -gt $min ; then break ; fi
        if test $cur -lt $min ; then GTKDOC=false; break ; fi
    done
    IFS="$gtk_save_IFS"

   if $GTKDOC ; then
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
test/Makefile
doc/Makefile
doc/reference/Makefile
aostax.pc])



