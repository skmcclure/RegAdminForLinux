dnl formerly GNOME_COMPILE_WARNINGS

AC_DEFUN([AX_JM_COMPILE_WARNINGS_C],[
    dnl ******************************
    dnl More compiler warnings
    dnl ******************************

    AC_LANG_PUSH(C)
    if test -z "$1" ; then
	default_compile_warnings=yes
    else
	default_compile_warnings="$1"
    fi

    AC_ARG_ENABLE([c-warnings],[
AS_HELP_STRING([--enable-c-warnings=no|minimum|yes|maximum|error],
               [Turn on GCC compiler warnings.])
    ],[], [
      enable_c_warnings="$default_compile_warnings"
    ])

    if test "x$GCC" != xyes; then
	enable_c_warnings=no
    fi

    gcc_warning_flags=
    realsave_CFLAGS="$CFLAGS"

    case "$enable_c_warnings" in
    no)
	possible_gcc_warning_flags=
	;;
    minimum)
	possible_gcc_warning_flags="-Wall"
	;;
    yes)
	possible_gcc_warning_flags="-Wall -Wmissing-prototypes"
	;;
    maximum|error)
	possible_gcc_warning_flags="-Wall -Wmissing-prototypes -Wnested-externs -Wpointer-arith -Wimplicit -Wreturn-type -Wunused -Wswitch -Wcomment -Wunititialized -Wparentheses -Wno-sign-compare -Wfloat-equal -Wextra -W -Wformat -Wcomment -Wcast-align -Wwrite-strings -Wstrict-prototypes -Winline -Wredundant-decls -Wc++-compat"
	if test "$enable_c_warnings" = "error" ; then
	    possible_gcc_warning_flags="$gcc_warning_flags -Werror"
	fi
	;;
    *)
	AC_MSG_ERROR(Unknown argument '$enable_c_warnings' to --enable-c-warnings)
	;;
    esac

    if test -n "$possible_gcc_warning_flags" ; then
        for i in $possible_gcc_warning_flags ; do
            AX_JM_CHECK_GCC_FLAG($i, [gcc_warning_flags])
        done
    fi

    CFLAGS="$realsave_CFLAGS"
    AC_MSG_CHECKING(what warning flags to pass to the C compiler)
    AC_MSG_RESULT($gcc_warning_flags)

    AC_ARG_ENABLE([iso-c], [
AS_HELP_STRING([--enable-iso-c],
               [Try to warn if code is not ISO C ])
    ], [], [
      enable_iso_c=no
    ])

    AC_MSG_CHECKING(what language compliance flags to pass to the C compiler)
    gcc_compliance_flags=
    if test "x$enable_iso_c" != "xno"; then
	if test "x$GCC" = "xyes"; then
	case " $CFLAGS " in
	    *[\ \	]-ansi[\ \	]*) ;;
	    *) gcc_compliance_flags="$gcc_compliance_flags -ansi" ;;
	esac
	case " $CFLAGS " in
	    *[\ \	]-pedantic[\ \	]*) ;;
	    *) gcc_compliance_flags="$gcc_compliance_flags -pedantic" ;;
	esac
	fi
    fi
    AC_MSG_RESULT($gcc_compliance_flags)

    GCC_WARN_FLAGS="$gcc_warning_flags $gcc_compliance_flags"
    AC_SUBST(GCC_WARN_FLAGS)
    AC_LANG_POP(C)
])
