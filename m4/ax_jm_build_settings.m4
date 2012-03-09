AC_DEFUN([AX_JM_BUILD_SETTINGS], [

  ax_build_type=

  if test -z "$1" ; then
    ax_build_type=test
  else
    ax_build_type="$1"
  fi

  AC_MSG_CHECKING(what build type to use)

  AC_ARG_ENABLE([build], [
AS_HELP_STRING([--enable-build=debug|test|profile|coverage|release|fast-release|debug-release],
               [What type of build to be doing.])
  ], [
    ax_build_type=$enableval
  ])

  ax_build_local_compile_settings=""

  case "$ax_build_type" in

    debug)
      ax_build_local_compile_settings="debug"
      BUILD_TYPE=$ax_build_type
      ;;

    test)
      ax_build_local_compile_settings="test"
      BUILD_TYPE=$ax_build_type
      ;;

    release)
      ax_build_local_compile_settings="space"
      BUILD_TYPE="release"
      ;;

    fast-release)
      ax_build_local_compile_settings="very"
      BUILD_TYPE="release"
      ;;

    debug-release)
      ax_build_local_compile_settings="default"
      BUILD_TYPE="debug-release"
      ;;

    profile)
      ax_build_local_compile_settings="profile"
      BUILD_TYPE=$ax_build_type
      ;;

    coverage)
      ax_build_local_compile_settings="coverage"
      BUILD_TYPE=$ax_build_type
      ;;

    *)
      AC_MSG_ERROR(
        [Unknown argument '$ax_build_type' to --enable-build])
      ;;

  esac

  AC_SUBST(BUILD_TYPE)
  AC_DEFINE_UNQUOTED([BUILD_TYPE], ["$BUILD_TYPE"],
                     [The build type used.])
  AM_CONDITIONAL(IS_RELEASE_BUILD, test "$BUILD_TYPE" = "release" -o "$BUILD_TYPE" = "fast-release" -o "$BUILD_TYPE" = "debug-release")
  AM_CONDITIONAL(IS_TEST_BUILD, test "$BUILD_TYPE" = "test")
  AM_CONDITIONAL(IS_DEBUG_BUILD, test "$BUILD_TYPE" = "debug")

  AC_MSG_RESULT([$BUILD_TYPE])

  AX_JM_DO_COMPILE_SETTINGS($ax_build_local_compile_settings)

])
