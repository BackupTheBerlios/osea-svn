noinst_PROGRAMS = test_coyote \
	 	  test_coyote_connection \
		  test_coyote_simple \
	 	  test_coyote_xml  \
		  test_coyote_dataset

INCLUDES = -I $(top_srcdir)/libcoyote $(LIBRARIES_CFLAGS) -Wall

test_coyote_xml_SOURCES = test_coyote_xml.c

test_coyote_xml_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libcoyote/src/libcoyote.la

test_coyote_SOURCES = test_coyote.c

test_coyote_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libcoyote/src/libcoyote.la

test_coyote_simple_SOURCES = test_coyote_simple.c

test_coyote_simple_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libcoyote/src/libcoyote.la

test_coyote_connection_SOURCES = test_coyote_connection.c

test_coyote_connection_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libcoyote/src/libcoyote.la

test_coyote_dataset_SOURCES = test_coyote_dataset.c

test_coyote_dataset_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/libcoyote/src/libcoyote.la
