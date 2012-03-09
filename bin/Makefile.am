
SUBDIRS = . _test

LDADD = \
	$(top_builddir)/rregadmin/librregadmin.la

AM_LDFLAGS = -all-static

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

bin_PROGRAMS = \
	rra_basic_cl \
	rra_registry_equal \
	rra_registry_diff

if HAVE_FUSE
FUSE_PROGS = rrafs
endif

noinst_PROGRAMS = \
	rra_registry_check \
	$(FUSE_PROGS)

rra_basic_cl_SOURCES = rra_basic_cl.c
rra_registry_check_SOURCES = rra_registry_check.c
rra_registry_equal_SOURCES = rra_registry_equal.c
rra_registry_diff_SOURCES = rra_registry_diff.c

rrafs_SOURCES = rrafs.c
rrafs_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	$(fuse_CFLAGS)
rrafs_LDADD = \
	$(LDADD) \
	$(top_builddir)/rregadmin/fuse/librregadmin_fuse.la

# include $(top_srcdir)/m4/splint-rules.am
