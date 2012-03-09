
SUBDIRS = . _test

if HAVE_FUSE

noinst_HEADERS = \
	ops.h \
	log.h

noinst_LTLIBRARIES = librregadmin_fuse.la

librregadmin_fuse_la_SOURCES = \
	init.c \
	oper.c \
	data.c \
	access.c \
	chmod.c \
	chown.c \
	fsync.c \
	getattr.c \
	getxattr.c \
	link.c \
	listxattr.c \
	mkdir.c \
	mknod.c \
	open.c \
	read.c \
	readdir.c \
	readlink.c \
	release.c \
	removexattr.c \
	rename.c \
	rmdir.c \
	setxattr.c \
	statfs.c \
	symlink.c \
	truncate.c \
	unlink.c \
	utimens.c \
	write.c

librregadmin_fuse_la_LIBADD = \
	$(fuse_LIBS) \
	$(top_builddir)/rregadmin/librregadmin.la
librregadmin_fuse_la_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(fuse_CFLAGS)
librregadmin_fuse_la_LDFLAGS = \
	-version-info $(RRA_LTLIB_VERSION)

endif
