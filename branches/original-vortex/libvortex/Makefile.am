if ENABLE_AF_ARCH_DOC
VORTEX_DOC_DIR = doc
endif

if ENABLE_READLINE 
VORTEX_TEST_DIR = test
endif

SUBDIRS = src $(VORTEX_TEST_DIR) $(VORTEX_DOC_DIR) data logo
EXTRA_DIST = vortex.pc.in VERSION
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = vortex.pc
