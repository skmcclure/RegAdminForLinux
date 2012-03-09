dnl @synopsis AX_JM_CONFIG_PKGCONFIG_IN [(LIBRARY [, DESCRIPTION [, DESTINATION]])]
dnl
dnl Creates a custom pkg-config script.  The script supports
dnl --cflags, --libs and --version options.
dnl
dnl This macro saves you all the typing for a pkg-config.in script;
dnl you don't even need to distribute one along. Place this macro
dnl in your configure.ac, et voila, you got one that you want to install.
dnl
dnl The options:
dnl
dnl  $1 = LIBRARY       e.g. gtk, ncurses
dnl  $2 = DESCRIPTION    one line description of library
dnl  $3 = DESTINATION   directory path, e.g. src/scripts
dnl
dnl It is suggested that the following CFLAGS and LIBS variables are
dnl used in your configure.ac.  library_libs is *essential*.
dnl library_cflags is important, but not always needed.  If they do not
dnl exist, defaults will be taken from LIBRARY_CFLAGS, LIBRARY_LIBS
dnl (should be -llibrary *only*) and LIBRARY_LIBDEPS (-l options for
dnl libraries your library depends upon.
dnl LIBLIBRARY_LIBS is simply $LIBRARY_LIBS $LIBRARY_LIBDEPS.
dnl NB. LIBRARY and library are the name of your library, in upper and
dnl lower case repectively e.g. GTK, gtk.
dnl
dnl  LIBRARY_CFLAGS:    cflags for compiling libraries and example progs
dnl  LIBRARY_LIBS:      libraries for linking programs
dnl  LIBRARY_LIBDEPS*:  libraries for linking libraries against (needed
dnl                     to link -static
dnl  LIBRARY_REQUIRES:  packages required by your library
dnl  LIBRARY_CONFLICTS: packages to conflict with your library
dnl  library_cflags*:   cflags to store in library-config
dnl  library_libs*:     libs to store in library-config
dnl  LIBLIBRARY_LIBS:   libs to link programs IN THIS PACKAGE ONLY against
dnl  LIBRARY_VERSION*:  the version of your library (x.y.z recommended)
dnl    *=required if you want sensible output, otherwise they will be
dnl      *guessed* (DWIM, but usually correct)
dnl
dnl There is also an AC_SUBST(LIBRARY_PKGCONFIG) that will be set to
dnl the name of the file that we output in this macro. Use as:
dnl
dnl  install-data-local: install-pkgconfig
dnl  install-pkgconfig:
dnl     $(mkinstalldirs) $(DESTDIR)$(bindir)
dnl     $(INSTALL_DATA) @LIBRARY_PKGCONFIG@ $(DESTDIR)$(prefix)/lib/pkgconfig
dnl
dnl Or, if using automake:
dnl
dnl  pkgconfigdatadir = $(prefix)/lib/pkgconfig
dnl  pkgconfigdata_DATA = gimpprint.pc = @LIBRARY_PKGCONFIG@
dnl
dnl Example usage:
dnl
dnl  GIMPPPRINT_LIBS="-lgimpprint"
dnl  AC_CHECK_LIB(m,pow,
dnl               GIMPPRINT_DEPLIBS="${GIMPPRINT_DEPLIBS} -lm")
dnl  AX_JM_CONFIG_PKGCONFIG_IN([gimpprint], [GIMP Print Top Quality Printer Drivers], [src/main])
dnl
dnl @version $Id: ac_config_pkgconfig_in.m4,v 1.1 2004/01/27 10:27:38 bastiaan Exp $
dnl @author Roger Leigh <roger@whinlatter.uklinux.net>
dnl
## AX_JM_CONFIG_PKGCONFIG_IN(LIBRARY, DESCRIPTION, DESTINATION)
## ---------------------------------------------------------
## Create a custom pkg-config script for LIBRARY.  Include a one-line
## DESCRIPTION.  The script will be created in a DESTINATION
## directory.
AC_DEFUN([AX_JM_CONFIG_PKGCONFIG_IN],
[
  # create a custom pkg-config file ($1.pc.in)
  m4_pushdef([PKGCONFIG_DIR], [m4_if([$3], , , [$3/])])
  m4_pushdef([PKGCONFIG_FILE], [PKGCONFIG_DIR[]$1.pc])

  AC_SUBST(target)dnl
  AC_SUBST(host)dnl
  AC_SUBST(build)dnl
  # create directory if it does not preexist
  m4_if([$3], , , [AS_MKDIR_P([$3])])
  # we're going to need uppercase, lowercase and user-friendly versions of the
  # string `MODULE'
  m4_pushdef([MODULE_UP], m4_translit([$1], [a-z], [A-Z]))dnl
  m4_pushdef([MODULE_DOWN], m4_translit([$1], [A-Z], [a-z]))dnl
  if test -z "$MODULE_DOWN[]_cflags" ; then
    if test -n "$MODULE_UP[]_CFLAGS" ; then
      MODULE_DOWN[]_cflags="$MODULE_UP[]_CFLAGS"
    else
      dnl AC_MSG_WARN([variable `MODULE_DOWN[]_cflags' undefined])
      MODULE_DOWN[]_cflags=''
    fi
  fi
  AC_SUBST(MODULE_DOWN[]_cflags)dnl
  AC_DEFINE_UNQUOTED(MODULE_DOWN[]_cflags, ["$MODULE_DOWN[]_cflags"],
                     [Cflags used in pkgconfig file])
  if test -z "$MODULE_DOWN[]_libs" ; then
    if test -n "$MODULE_UP[]_LIBS" ; then
      MODULE_DOWN[]_libs="$MODULE_UP[]_LIBS"
    else
      AC_MSG_WARN([variable `MODULE_DOWN[]_libs' and `MODULE_UP[]_LIBS' undefined])
      MODULE_DOWN[]_libs='-l$1'
    fi
    if test -n "$MODULE_UP[]_LIBDEPS" ; then
      MODULE_DOWN[]_libs="$MODULE_DOWN[]_libs $MODULE_UP[]_LIBDEPS"
    fi
  fi
  AC_SUBST(MODULE_DOWN[]_libs)dnl
  AC_DEFINE_UNQUOTED(MODULE_DOWN[]_libs, ["$MODULE_DOWN[]_libs"],
                     [Libs used in pkgconfig file])
  AC_SUBST(MODULE_UP[]_REQUIRES)
  AC_DEFINE_UNQUOTED(MODULE_UP[]_REQUIRES, ["$MODULE_UP[]_REQUIRES"],
                     [Requires used in pkgconfig file])
  AC_SUBST(MODULE_UP[]_CONFLICTS)
  AC_DEFINE_UNQUOTED(MODULE_UP[]_CONFLICTS, ["$MODULE_UP[]_CONFLICTS"],
                     [Conflicts used in pkgconfig file])
  if test -z "$MODULE_UP[]_VERSION" ; then
    AC_MSG_WARN([variable `MODULE_UP[]_VERSION' undefined])
    MODULE_UP[]_VERSION="$VERSION"
  fi
  AC_SUBST(MODULE_UP[]_VERSION)dnl
  AC_DEFINE_UNQUOTED(MODULE_UP[]_VERSION, ["$MODULE_UP[]_VERSION"],
                     [Version used in pkgconfig file])

  AC_CONFIG_COMMANDS(PKGCONFIG_FILE,
    (
      echo "prefix=$prefix"
      echo "exec_prefix=$exec_prefix"
      echo "libdir=$libdir"
      echo "includedir=$includedir"
      echo
      echo "Name: $P_NAME"
      echo 'Description: $2'
      if test -n "$MODULE_DOWN[]_requires" ; then
        echo "Requires: $MODULE_DOWN[]_requires"
      fi
      if test -n "$MODULE_DOWN[]_conflicts" ; then
        echo "Conflicts: $MODULE_DOWN[]_conflicts"
      fi
      echo "Version: $VERSION"
      echo "Libs: -L${libdir} $MODULE_DOWN[]_libs"
      echo "Cflags: -I${includedir} $MODULE_DOWN[]_cflags"
    ) > PKGCONFIG_FILE
  ,
    prefix="$prefix"
    exec_prefix="$exec_prefix"
    libdir="$libdir"
    includedir="$includedir"
    MODULE_DOWN[]_requires="$MODULE_UP[]_REQUIRES"
    MODULE_DOWN[]_conflicts="$MODULE_UP[]_CONFLICTS"
    P_NAME="$1"
    VERSION="$VERSION"
    MODULE_DOWN[]_libs="$MODULE_DOWN[]_libs"
    MODULE_DOWN[]_cflags="$MODULE_DOWN[]_cflags"
  )

dnl   m4_pushdef([PKGCONFIG_UP], [m4_translit([$1], [a-z-], [A-Z_])])dnl
dnl   PKGCONFIG_UP[]_PKGCONFIG="PKGCONFIG_DIR[]$1-config"
dnl   AC_SUBST(PKGCONFIG_UP[]_PKGCONFIG)
dnl   AC_CONFIG_COMMANDS($PKGCONFIG_DIR[]$1-config,
dnl     (
dnl       echo "#! /bin/sh"
dnl     ) > PKGCONFIG_DIR[]$1-config
dnl     chmod +x PKGCONFIG_DIR[]$1-config
dnl   ,
dnl     PKGCONFIG_DIR[]$1-config="$PKGCONFIG_DIR[]$1-config"
dnl   )
dnl   m4_popdef([PKGCONFIG_UP])

  m4_popdef([MODULE_DOWN])dnl
  m4_popdef([MODULE_UP])dnl
  m4_popdef([PKGCONFIG_DIR])dnl
  m4_popdef([PKGCONFIG_FILE])dnl
])
