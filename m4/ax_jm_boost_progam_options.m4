dnl @synopsis AX_JM_BOOST_PROGRAM_OPTIONS
dnl
dnl This macro checks to see if the Boost.ProgramOptions library is
dnl installed. It also attempts to guess the currect library name using
dnl several attempts. It tries to build the library name using a user
dnl supplied name or suffix and then just the raw library.
dnl
dnl If the library is found, HAVE_BOOST_POPTIONS is defined and
dnl BOOST_PROGRAM_OPTIONS_LIB is set to the name of the library.
dnl
dnl This macro calls AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB).
dnl
dnl @category InstalledPackages
dnl @author Michael Tindal <mtindal@paradoxpoint.com>
dnl @author James Lewismoss <jamesx.r.lewismoss@intel.com
dnl @version 2005-09-15
dnl @license GPLWithACException

AC_DEFUN([AX_JM_BOOST_PROGRAM_OPTIONS],
[AC_REQUIRE([AC_CXX_NAMESPACES])dnl
AC_CACHE_CHECK(whether the Boost::Program_Options is available,
ax_cv_boost_program_options,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[#include <boost/program_options.hpp>]],
			           [[using namespace boost::program_options;
				   options_description od;
                                   od.add_options()("help,h", "display help");
				   return 0;]]),
  	           ax_cv_boost_program_options=yes, ax_cv_boost_program_options=no)
 AC_LANG_RESTORE
])
if test "$ax_cv_boost_program_options" = yes; then
  AC_DEFINE(HAVE_BOOST_POPTIONS,[1],[define if the Boost::Program_Options library is available])
  dnl Now determine the appropriate file names
  AC_ARG_WITH([boost-program-options],AS_HELP_STRING([--with-boost-program-options],
  [specify the boost program options library or suffix to use]),
  [if test "x$with_boost_program_options" != "xno"; then
    ax_filesystem_lib=$with_boost_program_options
    ax_boost_program_options_lib=boost_program_options-$with_boost_program_options
  fi])
  for ax_lib in $ax_program_options_lib $ax_boost_program_options_lib boost_program_options boost_program_options-gcc; do
    AC_CHECK_LIB($ax_lib, main, [BOOST_PROGRAM_OPTIONS_LIB=$ax_lib BOOST_PO_LIBS="-l$ax_lib" break])
  done
  AC_SUBST(BOOST_PROGRAM_OPTIONS_LIB)
  AC_SUBST(BOOST_PO_LIBS)
fi
])dnl
