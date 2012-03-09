
SUBDIRS = . _test

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = librregadmin_sam.la

librregadmin_sam_la_SOURCES = \
	sam_info.c \
	user.c \
	userfinfo.c \
	uservinfo.c

noinst_HEADERS = \
	docs.h

pkginclude_HEADERS = \
	sam_info.h \
	types.h \
	user.h \
	userfinfo.h \
	uservinfo.h \
	log.h

pkgincludedir = $(includedir)/rregadmin/sam
