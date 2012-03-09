
SUBDIRS = . _test

EXTRA_DIST = \
	docs.h

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = librregadmin_diff.la

librregadmin_diff_la_SOURCES = \
	diff_info.c \
	hive.c \
	key_cell.c \
	registry.c \
	value_key_cell.c

pkginclude_HEADERS = \
	diff_info.h \
	hive.h \
	key_cell.h \
	registry.h \
	value_key_cell.h \
	log.h

noinst_HEADERS = \
	internal_defs.h

pkgincludedir = $(includedir)/rregadmin/diff
