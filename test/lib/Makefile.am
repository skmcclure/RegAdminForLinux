
SUBDIRS = sha1-0.2

LDADD = @LIBS@

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

noinst_LTLIBRARIES = librregadmin_test.la

BUILT_SOURCES = \
	rra_test_consts.h

librregadmin_test_la_SOURCES = \
	FileInfo.cpp \
	example_file.c \
	example_registry.cpp \
	example_hive.cpp \
	good_stuff.cpp \
	sha1.cpp

noinst_HEADERS = \
	FileInfo.hpp \
	example_file.h \
	example_registry.h \
	example_hive.h \
	good_stuff.h \
	log_level_setter.hpp \
	sha1.h \
	vector_utils.h \
	vector_utils.inl \
	ustring_to_string.hpp

nodist_noinst_HEADERS = \
	rra_test_consts.h

EXTRA_DIST = \
	rra_test_consts.h.in

made_files = rra_test_consts.h
CLEANFILES = $(made_files)

all-local: $(made_files)

$(made_files): %: %.in
	@cd $(top_builddir) && \
	  CONFIG_COMMANDS= \
	  CONFIG_FILES=$(subdir)/$@ \
	  CONFIG_HEADERS= \
	  ./config.status
