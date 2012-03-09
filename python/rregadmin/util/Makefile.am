
SUBDIRS = . _test

wrapper_pys = \
	glib_wrapper.py \
	icu_wrapper.py \
	ustring_wrapper.py \
	ustring_list_wrapper.py \
	value_wrapper.py \
	path_wrapper.py \
	path_info_wrapper.py \
	offset_wrapper.py \
	init_wrapper.py

all-local: $(wrapper_pys) __init__.py

include $(top_srcdir)/python/wrap-rules.am

rregadminpy_PYTHON = \
	__init__.py \
	path.py \
	$(wrapper_pys)

rregadminpydir = $(pythondir)/rregadmin/util

GLIB_DEPS = init_wrapper.py glib_wrapper.py icu_wrapper.py $(top_builddir)/$(subdir)/__init__.py

$(top_builddir)/$(subdir)/__init__.py: assure_init_py

glib_wrapper.py:: init_wrapper.py

icu_wrapper.py:: init_wrapper.py

init_wrapper.py:: $(top_srcdir)/rregadmin/util/init.h

ustring_wrapper.py:: $(top_srcdir)/rregadmin/util/ustring.h $(GLIB_DEPS)

ustring_list_wrapper.py:: $(top_srcdir)/rregadmin/util/ustring_list.h \
		$(GLIB_DEPS) \
		ustring_wrapper.py

value_wrapper.py:: $(top_srcdir)/rregadmin/util/value.h \
                    $(GLIB_DEPS) \
                    ustring_wrapper.py

path_wrapper.py:: $(top_srcdir)/rregadmin/util/path.h \
		$(GLIB_DEPS) \
		path_info_wrapper.py

path_info_wrapper.py:: $(top_srcdir)/rregadmin/util/path_info.h $(GLIB_DEPS)

offset_wrapper.py:: $(top_srcdir)/rregadmin/util/offsets.h $(GLIB_DEPS)

EXTRA_DIST = \
	glib_wrapper.conf \
	icu_wrapper.conf \
	path_wrapper.conf \
	path_info_wrapper.conf \
	ustring_wrapper.conf \
	ustring_list_wrapper.conf \
	value_wrapper.conf \
	offset_wrapper.conf \
	init_wrapper.conf

CLEANFILES = *.xml *.pyc *.pyo

MAINTAINERCLEANFILES = $(wrapper_pys)
