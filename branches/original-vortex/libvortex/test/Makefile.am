EXTRA_DIST = Makefile.win

bin_PROGRAMS  = vortex-client 

noinst_PROGRAMS = vortex-listener vortex-simple-listener vortex-simple-client

INCLUDES = -I$(top_srcdir)/libvortex/src $(LIBRARIES_CFLAGS) -I$(READLINE_PATH)/include -Wall -g -DVERSION=\""$(VORTEX_VERSION)"\" -I../src

vortex_listener_SOURCES = vortex-listener.c
vortex_listener_LDADD   = $(LIBRARIES_LIBS) ../src/libvortex.la

vortex_client_SOURCES = vortex-client.c 
vortex_client_LDADD   = $(LIBRARIES_LIBS) ../src/libvortex.la -L$(READLINE_PATH)/lib -lreadline -ltermcap

vortex_simple_listener_SOURCES = vortex-simple-listener.c
vortex_simple_listener_LDADD   = $(LIBRARIES_LIBS) ../src/libvortex.la

vortex_simple_client_SOURCES = vortex-simple-client.c
vortex_simple_client_LDADD   = $(LIBRARIES_LIBS) ../src/libvortex.la
