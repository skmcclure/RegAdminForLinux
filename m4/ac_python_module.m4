dnl @synopsis AC_PYTHON_MODULE(modname[, fatal])
dnl
dnl Checks for Python module.
dnl
dnl If fatal is non-empty then absence of a module will trigger an
dnl error.
dnl
dnl @category InstalledPackages
dnl @author Andrew Collier <colliera@nu.ac.za>.
dnl @version 2004-07-14
dnl @license AllPermissive

AC_DEFUN([AC_PYTHON_MODULE],[
  if test -z "$2" ; then
    ac_pm_import="import $1"
  else
    ac_pm_import="from $1 import $2"
  fi

  AC_MSG_CHECKING([python import: $ac_pm_import])
  py_command="$PYTHON -c \"$ac_pm_import\""
  echo "Running $py_command" >&5 2>&1
  eval $py_command >&5 2>&1
  if test $? -eq 0;
  then
    AC_MSG_RESULT(yes)
    apm_res=1
  else
    AC_MSG_RESULT(no)
    apm_res=0

    if test "$3" = "true"
    then
      AC_MSG_ERROR(failed to find required module $1)
      exit 1
    fi
  fi

  unset py_command
  AX_CONSTANT_UNQUOTED(AS_TR_CPP(HAVE_PYMOD_$1), [$apm_res],
                       [Whether module $ac_pm_import is valid])
  unset apm_res

])
