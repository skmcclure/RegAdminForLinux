
SUBDIRS = hives registries other special inf

EXTRA_DIST = \
	hives.list.in \
	sec-desc.list.in \
	registry.names \
	special-registry.names \
	tar-exclude.list

made_files = \
	hives.list \
	sec-desc.list

shared_ef_files = shared-keys shared-values

CLEANFILES = $(made_files)
DISTCLEANFILES = $(shared_ef_files)

all-local: $(made_files) $(shared_ef_files)

%: %.in
	@cd $(top_builddir) && \
	  CONFIG_COMMANDS= \
	  CONFIG_FILES=$(subdir)/$@ \
	  CONFIG_HEADERS= \
	  ./config.status

shared-keys:
	TOP_BUILDDIR=$(abs_top_builddir) $(top_srcdir)/test/bin/get_shared_keys.sh

shared-values:
	TOP_BUILDDIR=$(abs_top_builddir) $(top_srcdir)/test/bin/get_shared_values.sh
