
AC_DEFUN([AX_JM_ADD_INTEGRATION_TARGETS], [

  if test -d build ; then

    AX_JM_ADD_RECURSIVE_AM_MACRO([integration-check], [

include \$(top_srcdir)/build/make/integration-check.am

    ])
  elif test -d m4 ; then
    AX_JM_ADD_RECURSIVE_AM_MACRO([integration-check], [

include \$(top_srcdir)/make/integration-check.am

    ])
  fi

])
