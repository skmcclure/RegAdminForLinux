
SUBDIRS = . _test

EXTRA_DIST = \
	docs.h

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = librregadmin_registry.la

librregadmin_registry_la_SOURCES = \
	check.c \
	foreach.c \
	hive_info.c \
	init.c \
	registry.c \
	registry_key.c \
	regkeygen.c

noinst_HEADERS = \
	hive_info.h \
	regkeygen.h

pkginclude_HEADERS = \
	check.h \
	consts.h \
	foreach.h \
	init.h \
	registry.h \
	registry_key.h \
	search_order.h \
	types.h \
	log.h

pkgincludedir = $(includedir)/rregadmin/registry
