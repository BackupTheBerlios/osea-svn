## Process this file with automake to produce Makefile.in

INCLUDES = $(ASPLCLM_CFLAGS)  -DCOMPILATION_DATE=`date +%s` -I$(top_srcdir) -I$(top_srcdir)/libafdalkernel/src -g -Wall 


bin_PROGRAMS = aspl-clm

aspl_clm_SOURCES = \
	main.c \
	auto_completion.c auto_completion.h \
        help.c help.h \
	commands.c commands.h \
	connection.c connection.h \
	user.c user.h \
	group.c group.h \
	permission.c permission.h \
	servers.c servers.h


aspl_clm_LDADD = $(ASPLCLM_LIBS) $(READLINE_DEPLIBS) -lpopt \
	$(top_builddir)/libafdalkernel/src/libafdalkernel.la \
	$(top_builddir)/libafdal/src/libafdal.la \
	$(top_builddir)/libcoyote/src/libcoyote.la 

