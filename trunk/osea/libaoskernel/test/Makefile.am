noinst_PROGRAMS = test_afdal_kernel_user \
		  test_afdal_kernel_group \
		  test_afdal_kernel_permission 

INCLUDES = -I.. -I $(top_srcdir)/libafdalkernel $(LIBRARIES_CFLAGS) -Wall -g

test_afdal_kernel_user_SOURCES = test_afdal_kernel_user.c

test_afdal_kernel_user_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libafdalkernel/src/libafdalkernel.la $(top_builddir)/libafdal/src/libafdal.la $(top_builddir)/libcoyote/src/libcoyote.la 

test_afdal_kernel_group_SOURCES = test_afdal_kernel_group.c

test_afdal_kernel_group_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libafdalkernel/src/libafdalkernel.la $(top_builddir)/libafdal/src/libafdal.la $(top_builddir)/libcoyote/src/libcoyote.la 


test_afdal_kernel_permission_SOURCES = test_afdal_kernel_permission.c

test_afdal_kernel_permission_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libafdalkernel/src/libafdalkernel.la $(top_builddir)/libafdal/src/libafdal.la $(top_builddir)/libcoyote/src/libcoyote.la 