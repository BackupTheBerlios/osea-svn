## Process this file with automake to produce Makefile.in

INCLUDES = $(OSEAMANAGER_CFLAGS)  -DCOMPILATION_DATE=`date +%s` -I$(top_srcdir) -I$(top_srcdir)/include_dir -g -Wall 


bin_PROGRAMS = osea-manager

osea_manager_SOURCES = \
	main.c \
	auto_completion.c auto_completion.h \
        help.c help.h \
	commands.c commands.h \
	connection.c connection.h \
	user.c user.h \
	group.c group.h \
	permission.c permission.h \
	servers.c servers.h


osea_manager_LDADD = $(OSEAMANAGER_LIBS) $(READLINE_DEPLIBS) -lpopt \
	$(top_builddir)/libaoskernel/src/libaoskernel.la \
	$(top_builddir)/liboseaclient/src/liboseaclient.la \
	$(top_builddir)/liboseacomm/src/liboseacomm.la 

