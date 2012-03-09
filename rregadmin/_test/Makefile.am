
AM_CPPFLAGS = \
	$(MELUNIT_CXX_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(fuse_CFLAGS)

AM_LDFLAGS = $(MELUNIT_CXX_LIBS) -L..
LDADD = \
	../librregadmin.la \
	../util/librregadmin_util.la \
	../hive/librregadmin_hive.la \
	../reg/librregadmin_reg.la \
	../cli/librregadmin_cli.la

check_SCRIPTS = \
	test_headers \
	test_rra_headers

TESTS = \
	test_rra_headers

EXTRA_DIST = \
	$(check_SCRIPTS)

if HAVE_MELUNIT_CXX

check_PROGRAMS =

#TESTS += $(check_PROGRAMS)

endif

TESTS_ENVIRONMENT = \
	top_builddir=$(top_builddir) \
	top_srcdir=$(top_srcdir)

include $(top_srcdir)/m4/test_headers.am
