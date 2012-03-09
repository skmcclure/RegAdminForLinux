
SUBDIRS = . _test

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = librregadmin_secdesc.la

librregadmin_secdesc_la_SOURCES = \
	acentry.c \
	aclist.c \
	privilege.c \
	secdesc_type.c \
	security_descriptor.c \
	sid.c

noinst_HEADERS = \
	docs.h

pkginclude_HEADERS = \
	acentry.h \
	aclist.h \
	privilege.h \
	secdesc_type.h \
	security_descriptor.h \
	sid.h \
	log.h

pkgincludedir = $(includedir)/rregadmin/secdesc
