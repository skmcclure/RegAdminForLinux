if HAVE_FUSE
FUSE_SUBDIRS = fuse
endif

SUBDIRS = \
	util \
	secdesc \
	hive \
	registry \
	cli \
	sam \
	diff \
	. \
	doc \
	$(FUSE_SUBDIRS) \
	_test

EXTRA_DIST = \
	docs.h

CLEANFILES = Doxyfile.log
DISTCLEANFILES = Doxyfile rra_config.h

AM_LDFLAGS = -all-static

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir)

BUILT_SOURCES = \
	rra_config.h

lib_LTLIBRARIES = librregadmin.la

librregadmin_la_SOURCES =

librregadmin_la_LIBADD = \
	util/librregadmin_util.la \
	hive/librregadmin_hive.la \
	registry/librregadmin_registry.la \
	cli/librregadmin_cli.la \
	sam/librregadmin_sam.la \
	diff/librregadmin_diff.la \
	secdesc/librregadmin_secdesc.la \
	$(SSL_LIBS)

librregadmin_la_LDFLAGS = \
	$(SSL_LDFLAGS) \
	-version-info $(RRA_LTLIB_VERSION)

nodist_pkginclude_HEADERS = \
	rra_config.h

pkginclude_HEADERS = \
	basic.h

if USE_DOXYGEN
doxydocdir = $(DESTDIR)/$(prefix)/share/doc/$(PACKAGE)-$(VERSION)/html/rregadmin

html-local: doxygen-docs

doxygen-docs: Doxyfile $(shell find . -name '*.c' -o -name '*.h')
	cd .. ; \
	  doxygen $(subdir)/Doxyfile

install-data-local: doxygen-docs
	$(mkinstalldirs) $(doxydocdir)
	cd doxygen-docs && \
	  tar cf - . | ( cd $(doxydocdir) ; tar xf - )

uninstall-local:
	-test -d $(doxydocdir) && rm -rf $(doxydocdir)
	-rm -rf $(DESTDIR)$(prefix)/share/doc/$(PACKAGE)-$(VERSION)

clean-local:
	rm -rf doxygen-docs

endif
