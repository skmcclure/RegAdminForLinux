
AC_DEFUN([AX_JM_DO_COMPILE_SETTINGS],[

  if test -z "$1" ; then
    COMPILE_SETTING=default
  else
    COMPILE_SETTING="$1"
  fi

  AC_MSG_CHECKING(what optimization flags to pass to the C++ and C compiler)

  AC_ARG_ENABLE([compile-settings], [
AS_HELP_STRING([--enable-compile-settings=space|debug|very|default|profile|coverage|test],
               [What type of compile to do.])
  ], [
    COMPILE_SETTING=$enableval
  ])

  case "$COMPILE_SETTING" in

    default )
      CXXFLAGS="-g -O2 -DNDEBUG"
      CFLAGS="-g -O2 -DNDEBUG"
      ;;

    test )
      CXXFLAGS="-g -O2"
      CFLAGS="-g -O2"
      ;;

    space )
      CXXFLAGS="-Os -fno-enforce-eh-specs -fvisibility-inlines-hidden -DNDEBUG"
      CFLAGS="-Os -DNDEBUG"
      ;;

    debug )
      CXXFLAGS="-g3 -O0"
      CFLAGS="-g3 -O0"
      ;;

    very )
      CXXFLAGS="-O3 -DNDEBUG"
      CFLAGS="-O3 -DNDEBUG"
      ;;

    profile )
      CXXFLAGS="-g -pg -O2 -DNDEBUG"
      CFLAGS="-g -pg -O2 -DNDEBUG"
      ;;

    coverage )
      CXXFLAGS="-g -pg -O2 --coverage"
      CFLAGS="-g -pg -O2 --coverage"
      LDFLAGS="$LDFLAGS -lgcov"
      ;;

    * )
      AC_MSG_ERROR(
        [Unknown argument '$COMPILE_SETTING' to --enable-compile-settings])
  esac

  AC_MSG_RESULT(CXXFLAGS="$CXXFLAGS" CFLAGS="$CFLAGS")

  AC_PROG_CC
  AC_PROG_CXX

  AC_SUBST(COMPILE_SETTING)
  AC_DEFINE_UNQUOTED([COMPILE_SETTING], ["$COMPILE_SETTING"], [
    The compiler settings.])
])
