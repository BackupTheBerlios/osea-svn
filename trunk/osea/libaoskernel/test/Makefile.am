noinst_PROGRAMS = test_aos_kernel_user \
		  test_aos_kernel_group \
		  test_aos_kernel_permission 

INCLUDES = -I.. -I $(top_srcdir)/libaoskernel -I $(top_srcdir)/include_dir $(LIBRARIES_CFLAGS) -Wall -g

test_aos_kernel_user_SOURCES = test_aos_kernel_user.c

test_aos_kernel_user_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libaoskernel/src/libaoskernel.la $(top_builddir)/liboseaclient/src/liboseaclient.la $(top_builddir)/liboseacomm/src/liboseacomm.la 

test_aos_kernel_group_SOURCES = test_aos_kernel_group.c

test_aos_kernel_group_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libaoskernel/src/libaoskernel.la $(top_builddir)/liboseaclient/src/liboseaclient.la $(top_builddir)/liboseacomm/src/liboseacomm.la 


test_aos_kernel_permission_SOURCES = test_aos_kernel_permission.c

test_aos_kernel_permission_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libaoskernel/src/libaoskernel.la $(top_builddir)/liboseaclient/src/liboseaclient.la $(top_builddir)/liboseacomm/src/liboseacomm.la 