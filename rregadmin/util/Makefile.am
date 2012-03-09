
SUBDIRS = data . _test

LDADD = @LIBS@ $(SSL_LIBS)
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-DLOCALEDIR=\"$(localedir)\" \
	$(SSL_CFLAGS)

noinst_LTLIBRARIES = librregadmin_util.la

EXTRA_DIST = \
	macros.h.in \
	docs.h

librregadmin_util_la_LIBADD = \
	$(SSL_LIBS)
librregadmin_util_la_LDFLAGS = \
	$(SSL_LDFLAGS)
librregadmin_util_la_SOURCES = \
	binary_search.c \
	check.c \
	conversion_utils.c \
	flag_map.c \
	fs.c \
	giochan_helpers.c \
	iconv_helper.c \
	init.c \
	intutils.c \
	log.c \
	offsets.c \
	offset_list.c \
	options.c \
	password.c \
	path.c \
	path_info.c \
	path_part.c \
	text_utils.c \
	timestamp.c \
	ustring.c \
	ustring_list.c \
	value.c \
	value_type.c

# 	uchar_info.c

pkginclude_HEADERS = \
	binary_search.h \
	check.h \
	conversion_utils.h \
	flag_map.h \
	fs.h \
	giochan_helpers.h \
	iconv_helper.h \
	init.h \
	intutils.h \
	local_config.h \
	log.h \
	macros.h \
	malloc.h \
	offsets.h \
	offsets.inl \
	offset_list.h \
	options.h \
	password.h \
	path.h \
	path_info.h \
	path_part.h \
	text_utils.h \
	timestamp.h \
	types.h \
	ustring.h \
	ustring_list.h \
	value.h \
	value_type.h

#	uchar_info.h

pkgincludedir = $(includedir)/rregadmin/util
