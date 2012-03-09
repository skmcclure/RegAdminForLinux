dnl For C++, do basically the same thing.

AC_DEFUN([AX_JM_COMPILE_WARNINGS_CXX],[

  AC_LANG_PUSH(C++)

  if test -z "$1" ; then
    default_cxx_compile_warnings=minimum
  else
    default_cxx_compile_warnings="$1"
  fi

  AC_ARG_ENABLE([cxx-warnings], [
AS_HELP_STRING([--enable-cxx-warnings=no|minimum|yes|toomuch],
               [Turn on compiler warnings.])
  ], [], [
    enable_cxx_warnings=$default_cxx_compile_warnings
  ])

  if test "x$GCC" != xyes; then
    enable_cxx_warnings=no
  fi

  gxx_warning_flags=
  if test "x$enable_cxx_warnings" != "xno"; then
    case " $CXXFLAGS " in
    *[\ \	]-Wall[\ \	]*)
      ;;
    *)
      gxx_warning_flags="-Wall -Wno-unused"
      ;;
    esac

    case "$enable_cxx_warnings" in
    minimum)
      ;;
    yes|toomuch)
      gxx_warning_flags="$gxx_warning_flags -Wshadow -Woverloaded-virtual"
      to_check_flags="-Wno-sign-compare -Wfloat-equal -Wextra -W"
      if test "x$enable_cxx_warnings" = "xtoomuch" ; then
        to_check_flags="$to_check_flags -Wctor-dtor-privacy -Weffc++ -Wold-style-cast"
      fi
      for i in $to_check_flags ; do
        AX_JM_CHECK_GCC_FLAG($i, [gxx_warning_flags])
      done
      unset to_check_flags
      ;;
    esac

  fi
  AC_MSG_CHECKING(what warning flags to pass to the G++ compiler)
  AC_MSG_RESULT($gxx_warning_flags)

  AC_ARG_ENABLE([iso-cxx], [
AS_HELP_STRING([--enable-iso-cxx],
               [Try to warn if code is not ISO C++ ])
   ], [], [
     enable_iso_cxx=no
   ])

   AC_MSG_CHECKING(what language compliance flags to pass to the G++ compiler)
   gxx_compliance_flags=
   if test "x$enable_iso_cxx" != "xno"; then
     if test "x$GCC" = "xyes"; then
      case " $CXXFLAGS " in
      *[\ \	]-ansi[\ \	]*) ;;
      *) gxx_compliance_flags="$gxx_compliance_flags -ansi" ;;
      esac

      case " $CXXFLAGS " in
      *[\ \	]-pedantic[\ \	]*) ;;
      *) gxx_compliance_flags="$gxx_compliance_flags -pedantic" ;;
      esac
     fi
   fi
  AC_MSG_RESULT($gxx_compliance_flags)

  GXX_WARN_FLAGS="$gxx_warning_flags $gxx_compliance_flags"
  AC_SUBST(GXX_WARN_FLAGS)

  AC_LANG_POP(C++)

])
