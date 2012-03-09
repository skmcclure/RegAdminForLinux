
AM_CPPFLAGS = \
	$(MELUNIT_CXX_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_builddir)/test/lib \
	-I$(top_srcdir)/test/lib

AM_LDFLAGS = $(MELUNIT_CXX_LIBS) # $(EFENCE_LIBS)

LDADD = \
	../librregadmin_sam.la \
	../../cli/librregadmin_cli.la \
	../../registry/librregadmin_registry.la \
	../../hive/librregadmin_hive.la \
	../../secdesc/librregadmin_secdesc.la \
	../../util/librregadmin_util.la \
	$(top_builddir)/test/lib/librregadmin_test.la

noinst_HEADERS = \
	userxinfo.h

if HAVE_MELUNIT_CXX

check_PROGRAMS = \
	test_sam_info \
	test_user \
	test_userfinfo \
	test_userfinfo2 \
	test_uservinfo \
	test_uservinfo2

TESTS = $(check_PROGRAMS)

test_sam_info_SOURCES = test_sam_info.cpp
test_user_SOURCES = test_user.cpp
test_userfinfo_SOURCES = test_userfinfo.cpp
test_userfinfo2_SOURCES = test_userfinfo2.cpp
test_uservinfo_SOURCES = test_uservinfo.cpp
test_uservinfo2_SOURCES = test_uservinfo2.cpp

endif
