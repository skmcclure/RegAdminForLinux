
SUBDIRS = . _test

EXTRA_DIST = \
	docs.h

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = librregadmin_cli.la

librregadmin_cli_la_SOURCES = \
	basic.c \
	basic_result.c \
	init.c \
	options.c

pkginclude_HEADERS = \
	basic.h \
	basic_result.h \
	init.h \
	options.h \
	log.h

pkgincludedir = $(includedir)/rregadmin/cli
