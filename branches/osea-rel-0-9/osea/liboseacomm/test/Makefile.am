noinst_PROGRAMS = test_oseacomm \
	 	  test_oseacomm_connection \
		  test_oseacomm_simple \
	 	  test_oseacomm_xml  \
		  test_oseacomm_dataset

INCLUDES = -I $(top_srcdir)/liboseacomm $(LIBRARIES_CFLAGS) -Wall

test_oseacomm_xml_SOURCES = test_oseacomm_xml.c

test_oseacomm_xml_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/liboseacomm/src/liboseacomm.la

test_oseacomm_SOURCES = test_oseacomm.c

test_oseacomm_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/liboseacomm/src/liboseacomm.la

test_oseacomm_simple_SOURCES = test_oseacomm_simple.c

test_oseacomm_simple_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/liboseacomm/src/liboseacomm.la

test_oseacomm_connection_SOURCES = test_oseacomm_connection.c

test_oseacomm_connection_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/liboseacomm/src/liboseacomm.la

test_oseacomm_dataset_SOURCES = test_oseacomm_dataset.c

test_oseacomm_dataset_LDADD = $(LIBRARIES_LIBS) $(top_builddir)/liboseacomm/src/liboseacomm.la
