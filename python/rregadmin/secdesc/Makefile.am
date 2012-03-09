
SUBDIRS = . _test

wrapper_pys = \
	acentry_wrapper.py \
	aclist_wrapper.py \
	privilege_wrapper.py \
	security_descriptor_wrapper.py \
	sid_wrapper.py

all-local: assure_init_py $(wrapper_pys)

include $(top_srcdir)/python/wrap-rules.am

rregadminpy_PYTHON = \
	__init__.py \
	$(wrapper_pys)

rregadminpydir = $(pythondir)/rregadmin/security_descriptor

$(top_builddir)/$(subdir)/__init__.py: assure_init_py

acentry_wrapper.py:: $(top_srcdir)/rregadmin/secdesc/acentry.h \
              $(UTIL_WRAPPER_DEPENDS) \
              sid_wrapper.py

aclist_wrapper.py:: $(top_srcdir)/rregadmin/secdesc/aclist.h \
              $(UTIL_WRAPPER_DEPENDS) \
              acentry_wrapper.py

privilege_wrapper.py:: $(top_srcdir)/rregadmin/secdesc/privilege.h \
              $(UTIL_WRAPPER_DEPENDS)

security_descriptor_wrapper.py:: $(top_srcdir)/rregadmin/secdesc/security_descriptor.h \
              $(UTIL_WRAPPER_DEPENDS) \
              aclist_wrapper.py sid_wrapper.py

sid_wrapper.py:: $(top_srcdir)/rregadmin/secdesc/sid.h \
              $(UTIL_WRAPPER_DEPENDS)

EXTRA_DIST = \
	common-env.sh \
	acentry_wrapper.conf \
	aclist_wrapper.conf \
	privilege_wrapper.conf \
	security_descriptor_wrapper.conf \
	sid_wrapper.conf

CLEANFILES = *.xml *.pyc *.pyo

MAINTAINERCLEANFILES = $(wrapper_pys)
