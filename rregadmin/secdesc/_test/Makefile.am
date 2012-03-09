
AM_CPPFLAGS = \
	$(MELUNIT_CXX_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	-I$(top_builddir)/test/lib \
	-I$(top_srcdir)/test/lib

AM_LDFLAGS = $(MELUNIT_CXX_LIBS) # $(EFENCE_LIBS)

LDADD = \
	../librregadmin_secdesc.la \
	../../util/librregadmin_util.la

if HAVE_MELUNIT_CXX

check_PROGRAMS = \
	test_privilege \
	test_sid \
	test_acentry \
	test_aclist \
	test_security_descriptor \
	test_secdesc_type

test_privilege_SOURCES = test_privilege.cpp
test_sid_SOURCES = test_sid.cpp
test_acentry_SOURCES = test_acentry.cpp
test_aclist_SOURCES = test_aclist.cpp
test_security_descriptor_SOURCES = test_security_descriptor.cpp
test_secdesc_type_SOURCES = test_secdesc_type.cpp

TESTS = $(check_PROGRAMS)

endif
