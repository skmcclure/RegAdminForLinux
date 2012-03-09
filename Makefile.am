
# Copyright (C) 2006 Sean Loaring <sloaring@tec-man.com>
# Copyright 2008 Racemi Inc
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if SHOULD_INCLUDE_PYTHON_INTERFACE
PYTHON_DIRS = python
endif

SUBDIRS = po m4 rregadmin test bin examples $(PYTHON_DIRS) docs

EXTRA_DIST = \
	config.rpath \
	rregadmin.spec \
	rregadmin.spec.in \
	autogen.sh \
	realpath.sh \
	BUGS \
	TODO

DISTCLEANFILES = \
	rregadmin.pc \
	GPATH \
	GRTAGS \
	GSYMS \
	GTAGS

MAINTAINERCLEANFILES = rregadmin.spec

pkgconfig_FILES = \
	rregadmin.pc
pkgconfigdir = $(libdir)/pkgconfig

ACLOCAL_AMFLAGS = -I m4

@INC_AMRPMINCLUDE@
