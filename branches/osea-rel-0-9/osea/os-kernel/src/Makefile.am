INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	$(OSKERNEL_CFLAGS) -DSYSCONFDIR=\""$(sysconfdir)"\" \
	-I$(top_srcdir) -I$(top_srcdir)/include_dir \
	-DCOMPILATION_DATE=`date +%s` -Wall

bin_PROGRAMS = os-kernel

os_kernel_SOURCES = \
	os_kernel.c \
	os_kernel_crypto.c os_kernel_crypto.h \
	os_kernel_session.c os_kernel_session.h \
	os_kernel_registry.c os_kernel_registry.h \
	os_kernel_login_request.c os_kernel_login_request.h \
	os_kernel_logout_request.c os_kernel_logout_request.h \
	os_kernel_register_request.c os_kernel_register_request.h \
	os_kernel_user_request.c os_kernel_user_request.h \
	os_kernel_group_request.c  os_kernel_group_request.h \
	os_kernel_permission_request.c os_kernel_permission_request.h \
	os_kernel_update_services_request.c os_kernel_update_services_request.h \
	os_kernel_server_request.c os_kernel_server_request.h \
	os_kernel_keys.c os_kernel_keys.h

os_kernel_LDADD = \
	$(OSKERNEL_LIBS) \
	$(top_builddir)/liboseaserver/src/liboseaserver.la  \
	$(top_builddir)/liboseaclient/src/liboseaclient.la \
	$(top_builddir)/liboseacomm/src/liboseacomm.la -lpopt

