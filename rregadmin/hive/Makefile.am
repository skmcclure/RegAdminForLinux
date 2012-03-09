
SUBDIRS = . _test

EXTRA_DIST = \
	TODO \
	docs.h

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = librregadmin_hive.la

librregadmin_hive_la_SOURCES = \
	bin.c \
	cell.c \
	foreach.c \
	hcheck_gc_pass.c \
	hive.c \
	hive_check.c \
	hive_offset.c \
	init.c \
	key_cell.c \
	key_list_cell.c \
	security_descriptor_cell.c \
	value_cell.c \
	value_key_cell.c \
	value_list_cell.c \
	util.c

pkginclude_HEADERS = \
	bin.h \
	cell.h \
	defines.h \
	foreach.h \
	hcheck_decl.h \
	hive.h \
	hive_check.h \
	hive_offset.h \
	hive_offset.inl \
	init.h \
	key_cell.h \
	key_list_cell.h \
	types.h \
	security_descriptor_cell.h \
	value_cell.h \
	value_key_cell.h \
	value_list_cell.h \
	util.h \
	log.h

pkgincludedir = $(includedir)/rregadmin/hive
