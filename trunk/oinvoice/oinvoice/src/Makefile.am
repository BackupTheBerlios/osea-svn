## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DHAVE_CONFIG_H  -DCOMPILATION_DATE=`date +%s` -g -Wall \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = oinvoice

%.c %.h: %.gob
	@GOB2@ --no-private-header $<

oinvoice_SOURCES = \
	main.c \
	login_window.c login_window.h \
	settings.c settings.h \
	connection_settings_window.c connection_settings_window.h \
	connection_settings_wizard.c connection_settings_wizard.h \
	connection_progress_window.c connection_progress_window.h \
	event_source.c event_source.h \
	event_process.c event_process.h \
	dialog.c dialog.h \
	main_window.c main_window.h \
	about_window.c about_window.h \
	transaction_progress_window.c transaction_progress_window.h \
	detailedview-manager.c detailedview-manager.h \
	entry-manager.c entry-manager.h \
	tax.c tax.h \
	tax_vat_customer.c tax_vat_customer.h \
	tax_vat_item.c tax_vat_item.h \
	tax_vat_value.c tax_vat_value.h \
	usergroup.c usergroup.h \
	usergroup_user.c usergroup_user.h \
	usergroup_group.c usergroup_group.h \
	usergroup_permission.c usergroup_permission.h \
	osea_datalist.c osea_datalist.h \
	error.c error.h \
	support.c support.h \
	widgets.c widgets.h \
	refresh_session_dialog.c refresh_session_dialog.h \
	customer.c customer.h \
	customer_actions.c customer_actions.h \
	customer_window.c customer_window.h \
	osea_listmodel.c osea_listmodel.h 

oinvoice_LDFLAGS = -rdynamic

oinvoice_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

