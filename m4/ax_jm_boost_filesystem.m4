dnl @synopsis AX_JM_BOOST_FILESYSTEM
dnl
dnl This macro checks to see if the Boost.Filesystem library is
dnl installed. It also attempts to guess the currect library name using
dnl several attempts. It tries to build the library name using a user
dnl supplied name or suffix and then just the raw library.
dnl
dnl If the library is found, HAVE_BOOST_FILESYSTEM is defined and
dnl BOOST_FILESYSTEM_LIB is set to the name of the library.
dnl
dnl This macro calls AC_SUBST(BOOST_FILESYSTEM_LIB).
dnl
dnl @category InstalledPackages
dnl @author Michael Tindal <mtindal@paradoxpoint.com>
dnl @version 2004-09-20
dnl @license GPLWithACException

AC_DEFUN([AX_JM_BOOST_FILESYSTEM],
[AC_REQUIRE([AC_CXX_NAMESPACES])dnl
AC_CACHE_CHECK(whether the Boost::Filesystem library is available,
ax_cv_boost_filesystem,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[#include <boost/filesystem/path.hpp>]],
			           [[using namespace boost::filesystem;
				   path my_path( "foo/bar/data.txt" );
				   return 0;]]),
  	           ax_cv_boost_filesystem=yes, ax_cv_boost_filesystem=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_boost_filesystem" = yes; then
  AC_DEFINE(HAVE_BOOST_FILE,[1],[define if the Boost::FILESYSTEM library is available])
  dnl Now determine the appropriate file names
  AC_ARG_WITH([boost-filesystem],AS_HELP_STRING([--with-boost-filesystem],
  [specify the boost filesystem library or suffix to use]),
  [if test "x$with_boost_filesystem" != "xno"; then
    ax_filesystem_lib=$with_boost_filesystem
    ax_boost_filesystem_lib=boost_filesystem-$with_boost_filesystem
  fi])
  for ax_lib in $ax_filesystem_lib $ax_boost_filesystem_lib boost_filesystem boost_filesystem-gcc; do
    AC_CHECK_LIB($ax_lib, main, [BOOST_FILESYSTEM_LIB=$ax_lib BOOST_FS_LIBS="-l$ax_lib" break])
  done
  AC_SUBST(BOOST_FILESYSTEM_LIB)
  AC_SUBST(BOOST_FS_LIBS)
fi
])dnl
