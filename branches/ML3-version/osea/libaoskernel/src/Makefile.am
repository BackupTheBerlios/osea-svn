INCLUDES = -g -I$(top_srcdir)/liboseacomm -I$(top_srcdir)/liboseaclient \
	$(LIBRARIES_CFLAGS)  -DCOMPILATION_DATE=`date +%s` -Wall

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

libaoskernelincludedir = $(includedir)/aoskernel

lib_LTLIBRARIES = libaoskernel.la

libaoskernel_la_SOURCES =	\
	aos-kernel-user.c \
	aos-kernel-group.c \
	aos_kernel.c \
	aos_kernel_register.c \
        aos_kernel_user.c \
	aos_kernel_group.c \
	aos_kernel_permission.c \
	aos_kernel_server.c

libaoskernelinclude_HEADERS =	\
	aos-kernel-user.h \
	aos-kernel-group.h \
	aos_kernel.h \
	aos_kernel_register.h \
	aos_kernel_user.h \
	aos_kernel_group.h \
	aos_kernel_permission.h \
	aos_kernel_server.h

libaoskernel_la_LIBADD = \
	$(LIBRARIES_LIBS) 

