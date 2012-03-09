# doxygen stuff
AC_DEFUN([AX_JM_CHECK_DOXYGEN], [

  AC_MSG_CHECKING([whether to generate the doxygen documentation])
  AC_ARG_ENABLE([doxygen-docs-generation],
AS_HELP_STRING([--disable-doxygen-docs-generation],
               [disable doxygen docs generation]), [
    if test x$enableval = xyes ; then
      DOXYGEN_ENABLED=yes
    else
      DOXYGEN_ENABLED=no
    fi
  ], [
    DOXYGEN_ENABLED=yes
  ])
  AC_MSG_RESULT([$DOXYGEN_ENABLED])
  AC_SUBST(DOXYGEN_ENABLED)

  if test x$DOXYGEN_ENABLED = xyes ; then
    AC_PATH_PROG([DOXYGEN], [doxygen])

    AC_MSG_CHECKING([whether to use dot to generate doxygen graphs.])
    AC_ARG_ENABLE([doxygen-dot],
      AS_HELP_STRING([--disable-doxygen-dot],
                     [disable doxygens use of dot for graphs]), [
      if test x$enableval = xyes ; then
        USE_DOT=yes
      else
        USE_DOT=no
      fi
    ], [
      USE_DOT=yes
    ])
    AC_MSG_RESULT($USE_DOT)

    SHOULD_DOT="NO"
    if test x$USE_DOT = xyes ; then
      AC_PATH_PROG([DOT], [dot])
      if test ! -z "$DOT" ; then
        SHOULD_DOT="YES"
      fi
    fi
    AC_SUBST(SHOULD_DOT)

    AC_MSG_CHECKING([whether to create complex dot graphs])
    AC_ARG_ENABLE([doxygen-complex-dot],
      AS_HELP_STRING([--enable-doxygen-complex-dot],
                       [enable complex dot graphs]), [
      if test x$enableval = xyes ; then
        USE_COMPLEX_DOT=YES
      else
        USE_COMPLEX_DOT=NO
      fi
    ] , [
      USE_COMPLEX_DOT=NO
    ])
    AC_MSG_RESULT($USE_COMPLEX_DOT)
    AC_SUBST(USE_COMPLEX_DOT)

    ##
    AC_MSG_CHECKING(whether to include sources in doxygen output)
    AC_ARG_ENABLE([doxygen-sources],
      AS_HELP_STRING([--disable-doxygen-sources],
                     [disable source inclusion in doxygen docs]), [
      if test x$enableval = xyes ; then
        SHOULD_DOXYGEN_SOURCE=YES
      else
        SHOULD_DOXYGEN_SOURCE=NO
      fi
    ],[
      SHOULD_DOXYGEN_SOURCE=YES
    ])
    AC_SUBST(SHOULD_DOXYGEN_SOURCE)
    AC_MSG_RESULT([$SHOULD_DOXYGEN_SOURCE])

    AC_MSG_CHECKING([what formats of documentation doxygen should generate])
    AC_ARG_WITH([doxygen-formats],
      AS_HELP_STRING([--with-doxygen-formats=(html,pdf,man,def,xml,rtf,perlmod)],
                     [enable different formats]), [
      DOXYGEN_FORMATS=$withval
    ],[
      DOXYGEN_FORMATS=html
    ])
    AC_MSG_RESULT([$DOXYGEN_FORMATS])

    AX_JM_CHECK_DOXYGEN_FORMAT([HTML], $DOXYGEN_FORMATS)
    AX_JM_CHECK_DOXYGEN_FORMAT([PDF], $DOXYGEN_FORMATS)
    AX_JM_CHECK_DOXYGEN_FORMAT([MAN], $DOXYGEN_FORMATS)
    AX_JM_CHECK_DOXYGEN_FORMAT([DEF], $DOXYGEN_FORMATS)
    AX_JM_CHECK_DOXYGEN_FORMAT([XML], $DOXYGEN_FORMATS)
    AX_JM_CHECK_DOXYGEN_FORMAT([RTF], $DOXYGEN_FORMATS)
    AX_JM_CHECK_DOXYGEN_FORMAT([PERLMOD], $DOXYGEN_FORMATS)

    AX_JM_CHECK_DOXYGEN_FEATURE([USE_HTAGS], [
      AC_MSG_CHECKING([whether to use htags (if installed)])
      AC_ARG_ENABLE([htags],
        AS_HELP_STRING([--disable-htags],
                       [disable using htags]), [
        if test x$enableval = xyes ; then
          SHOULD_USE_HTAGS=YES
        else
          SHOULD_USE_HTAGS=NO
        fi
      ],[
        SHOULD_USE_HTAGS=YES
      ])
      AC_MSG_RESULT([$SHOULD_USE_HTAGS])
      AC_CHECK_PROG([DOXYGEN_HTAGS], [htags], [yes], [no])
      if test x$DOXYGEN_HTAGS = xno ; then
        AC_MSG_NOTICE([htags not found.  disabling])
        SHOULD_USE_HTAGS=NO
      fi
      AC_SUBST(SHOULD_USE_HTAGS)
    ])

  fi

  AC_PATH_PROG([PERL_PATH], [perl], [/usr/bin/perl])

  AM_CONDITIONAL(USE_DOXYGEN, test -n "$DOXYGEN" -a "$DOXYGEN_ENABLED" = "yes")

])
