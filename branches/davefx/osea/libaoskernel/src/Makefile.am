INCLUDES = -g -I$(top_srcdir) -I$(top_srcdir) $(LIBRARIES_CFLAGS)  -DCOMPILATION_DATE=`date +%s` -Wall

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

libafdalkernelincludedir = $(includedir)/afdalkernel

lib_LTLIBRARIES = libafdalkernel.la

libafdalkernel_la_SOURCES =	\
	afdal-kernel-user.c \
	afdal-kernel-group.c \
	afdal_kernel.c \
	afdal_kernel_register.c \
        afdal_kernel_user.c \
	afdal_kernel_group.c \
	afdal_kernel_permission.c \
	afdal_kernel_server.c

libafdalkernelinclude_HEADERS =	\
	afdal-kernel-user.h \
	afdal-kernel-group.h \
	afdal_kernel.h \
	afdal_kernel_register.h \
	afdal_kernel_user.h \
	afdal_kernel_group.h \
	afdal_kernel_permission.h \
	afdal_kernel_server.h

libafdalkernel_la_LIBADD = \
	$(LIBRARIES_LIBS) 

