dnl @synopsis AC_PYTHON_DEVEL([version])
dnl
dnl Checks for Python and tries to get the include path to 'Python.h'.
dnl It provides the $(PYTHON_CPPFLAGS) and $(PYTHON_LDFLAGS) output
dnl variables. Also exports $(PYTHON_EXTRA_LIBS) and
dnl $(PYTHON_EXTRA_LDFLAGS) for embedding Python in your code.
dnl
dnl You can search for some particular version of Python by passing a
dnl parameter to this macro, for example ">= '2.3.1'", or "== '2.4'".
dnl Please note that you *have* to pass also an operator along with the
dnl version to match, and pay special attention to the single quotes
dnl surrounding the version number.
dnl
dnl If the user wants to employ a particular version of Python, she can
dnl now pass to configure the PYTHON_VERSION environment variable. This
dnl is only limited by the macro parameter set by the packager.
dnl
dnl This macro should work for all versions of Python >= 2.1.0. You can
dnl disable the check for the python version by setting the
dnl PYTHON_NOVERSIONCHECK environment variable to something else than
dnl the empty string.
dnl
dnl If you need to use this macro for an older Python version, please
dnl contact the authors. We're always open for feedback.
dnl
dnl @category InstalledPackages
dnl @author Sebastian Huber <sebastian-huber@web.de>
dnl @author Alan W. Irwin <irwin@beluga.phys.uvic.ca>
dnl @author Rafael Laboissiere <laboissiere@psy.mpg.de>
dnl @author Andrew Collier <colliera@nu.ac.za>
dnl @author Matteo Settenvini <matteo@member.fsf.org>
dnl @version 2006-02-05
dnl @license GPLWithACException

AC_DEFUN([AC_PYTHON_DEVEL],[

	#
	# Check for a version of Python >= 2.1.0
	#
	AC_MSG_CHECKING([for a version of Python >= '2.1.0'])
	ac_supports_python_ver=`$PYTHON -c "import sys, string; \
		ver = string.split(sys.version)[[0]]; \
		print ver >= '2.1.0'"`
	if test "$ac_supports_python_ver" != "True"; then
		if test -z "$PYTHON_NOVERSIONCHECK"; then
			AC_MSG_RESULT([no])
			AC_MSG_FAILURE([
This version of the AC@&t@_PYTHON_DEVEL macro
doesn't work properly with versions of Python before
2.1.0. You may need to re-run configure, setting the
variables PYTHON_CPPFLAGS, PYTHON_LDFLAGS, PYTHON_SITE_PKG,
PYTHON_EXTRA_LIBS and PYTHON_EXTRA_LDFLAGS by hand.
Moreover, to disable this check, set PYTHON_NOVERSIONCHECK
to something else than an empty string.
])dnl
		else
			AC_MSG_RESULT([skip at user request])
		fi
	else
		AC_MSG_RESULT([yes])
	fi

	#
	# if the macro parameter ``version'' is set, honour it
	#
	if test -n "$1"; then
		AC_MSG_CHECKING([for a version of Python $1])
		ac_supports_python_ver=`$PYTHON -c "import sys, string; \
			ver = string.split(sys.version)[[0]]; \
			print ver $1"`
		if test "$ac_supports_python_ver" = "True"; then
	   	   AC_MSG_RESULT([yes])
		else
			AC_MSG_RESULT([no])
			AC_MSG_ERROR([this package requires Python $1.
If you have it installed, but it isn't the default Python
interpreter in your system path, please pass the PYTHON_VERSION
variable to configure. See ``configure --help'' for reference.
])dnl
		fi
	fi

	#
	# Check if you have distutils, else fail
	#
	AC_MSG_CHECKING([for the distutils Python package])
	ac_distutils_result=`$PYTHON -c "import distutils" 2>&1`
	if test -z "$ac_distutils_result"; then
		AC_MSG_RESULT([yes])
	else
		AC_MSG_RESULT([no])
		AC_MSG_ERROR([cannot import Python module "distutils".
Please check your Python installation. The error was:
$ac_distutils_result])
	fi

	#
	# Check for Python include path
	#
	AC_MSG_CHECKING([for Python include path])
	if test -z "$PYTHON_CPPFLAGS"; then
		python_path=`$PYTHON -c "import distutils.sysconfig; \
           		print distutils.sysconfig.get_python_inc();"`
		if test -n "${python_path}"; then
		   	python_path="-I$python_path"
		fi
		PYTHON_CPPFLAGS=$python_path
	fi
	AC_MSG_RESULT([$PYTHON_CPPFLAGS])
	AC_SUBST([PYTHON_CPPFLAGS])

	#
	# Check for Python library path
	#
	AC_MSG_CHECKING([for Python library path])
	if test -z "$PYTHON_LDFLAGS"; then
		# (makes two attempts to ensure we've got a version number
		# from the interpreter)
		py_version=`$PYTHON -c "from distutils.sysconfig import *; \
			from string import join; \
			print join(get_config_vars('VERSION'))"`
		if test "$py_version" == "[None]"; then
			if test -n "$PYTHON_VERSION"; then
				py_version=$PYTHON_VERSION
			else
				py_version=`$PYTHON -c "import sys; \
					print sys.version[[:3]]"`
			fi
		fi

		PYTHON_LDFLAGS=`$PYTHON -c "from distutils.sysconfig import *; \
			from string import join; \
			print '-L' + get_python_lib(0,1), \
		      	'-lpython';"`$py_version
	fi
	AC_MSG_RESULT([$PYTHON_LDFLAGS])
	AC_SUBST([PYTHON_LDFLAGS])

	#
	# Check for site packages
	#
	AC_MSG_CHECKING([for Python site-packages path])
	if test -z "$PYTHON_SITE_PKG"; then
		PYTHON_SITE_PKG=`$PYTHON -c "import distutils.sysconfig; \
		        print distutils.sysconfig.get_python_lib(0,0);"`
	fi
	AC_MSG_RESULT([$PYTHON_SITE_PKG])
	AC_SUBST([PYTHON_SITE_PKG])

	#
	# libraries which must be linked in when embedding
	#
	AC_MSG_CHECKING(python extra libraries)
	if test -z "$PYTHON_EXTRA_LIBS"; then
	   PYTHON_EXTRA_LIBS=`$PYTHON -c "import distutils.sysconfig; \
                conf = distutils.sysconfig.get_config_var; \
                print conf('LOCALMODLIBS'), conf('LIBS')"`
	fi
	AC_MSG_RESULT([$PYTHON_EXTRA_LIBS])
	AC_SUBST(PYTHON_EXTRA_LIBS)

	#
	# linking flags needed when embedding
	#
	AC_MSG_CHECKING(python extra linking flags)
	if test -z "$PYTHON_EXTRA_LDFLAGS"; then
		PYTHON_EXTRA_LDFLAGS=`$PYTHON -c "import distutils.sysconfig; \
			conf = distutils.sysconfig.get_config_var; \
			print conf('LINKFORSHARED')"`
	fi
	AC_MSG_RESULT([$PYTHON_EXTRA_LDFLAGS])
	AC_SUBST(PYTHON_EXTRA_LDFLAGS)

        AX_CHECK_STANDARD_PYTHON_MODULES
])

AC_DEFUN([AX_CHECK_STANDARD_PYTHON_MODULES], [

m4_pushdef([py_default_failure_mode], [true])

AC_PYTHON_MODULE([__builtin__], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([array], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([atexit], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([base64], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([binascii], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([cPickle], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([cPickle], [dumps], py_default_failure_mode)
dnl AC_PYTHON_MODULE([cPickle], [loads], py_default_failure_mode)
dnl AC_PYTHON_MODULE([collections], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([collections], [deque], py_default_failure_mode)
dnl AC_PYTHON_MODULE([commands], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([copy], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([copy], [deepcopy], py_default_failure_mode)
dnl AC_PYTHON_MODULE([distutils.core], [setup], py_default_failure_mode)
dnl AC_PYTHON_MODULE([errno], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([exceptions], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([fcntl], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([gc], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([getopt], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([glob], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([glob], [glob], py_default_failure_mode)
dnl AC_PYTHON_MODULE([grp], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([gzip], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([httplib], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([imp], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([inspect], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([math], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([math], [modf], py_default_failure_mode)
dnl AC_PYTHON_MODULE([math], [sqrt], py_default_failure_mode)
dnl AC_PYTHON_MODULE([md5], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([mailbox], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([mimetools], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([multifile], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([new], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([operator], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([operator], [setitem], py_default_failure_mode)
dnl AC_PYTHON_MODULE([os], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([os], [chmod], py_default_failure_mode)
dnl AC_PYTHON_MODULE([os], [unlink], py_default_failure_mode)
dnl AC_PYTHON_MODULE([os.path], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([os.path], [join], py_default_failure_mode)
dnl AC_PYTHON_MODULE([pickle], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([pickle], [dumps], py_default_failure_mode)
dnl AC_PYTHON_MODULE([pickle], [loads], py_default_failure_mode)
dnl AC_PYTHON_MODULE([popen2], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([popen2], [Popen3], py_default_failure_mode)
dnl AC_PYTHON_MODULE([posixfile], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([pprint], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([pprint], [pprint], py_default_failure_mode)
dnl AC_PYTHON_MODULE([profile], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([pstats], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([pwd], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([random], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([random], [Random], py_default_failure_mode)
dnl AC_PYTHON_MODULE([random], [random], py_default_failure_mode)
dnl AC_PYTHON_MODULE([random], [randrange], py_default_failure_mode)
dnl AC_PYTHON_MODULE([re], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([resource], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([sha], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([shelve], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([shutil], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([smtplib], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([select], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([signal], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([socket], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([socket], [*], py_default_failure_mode)
dnl AC_PYTHON_MODULE([socket], [AF_INET, SOCK_STREAM], py_default_failure_mode)
dnl AC_PYTHON_MODULE([socket], [gethostname], py_default_failure_mode)
dnl AC_PYTHON_MODULE([socket], [htons, ntohs], py_default_failure_mode)
dnl AC_PYTHON_MODULE([stat], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([stat], [*], py_default_failure_mode)
dnl AC_PYTHON_MODULE([stat], [ST_MTIME], py_default_failure_mode)
dnl AC_PYTHON_MODULE([stat], [S_ISDIR, ST_MODE, S_IXUSR, S_IXGRP, S_IXOTH], py_default_failure_mode)
dnl AC_PYTHON_MODULE([stat], [S_ISREG, ST_MODE, S_IXUSR, S_IXGRP, S_IXOTH, S_ISUID], py_default_failure_mode)
dnl AC_PYTHON_MODULE([statvfs], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([string], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([string], [find], py_default_failure_mode)
dnl AC_PYTHON_MODULE([string], [replace], py_default_failure_mode)
dnl AC_PYTHON_MODULE([string], [split], py_default_failure_mode)
dnl AC_PYTHON_MODULE([string], [join], py_default_failure_mode)
dnl AC_PYTHON_MODULE([string], [upper], py_default_failure_mode)
dnl AC_PYTHON_MODULE([string], [zfill], py_default_failure_mode)
dnl AC_PYTHON_MODULE([struct], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([struct], [*], py_default_failure_mode)
dnl AC_PYTHON_MODULE([sys], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([sys], [stdout], py_default_failure_mode)
dnl AC_PYTHON_MODULE([telnetlib], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([tempfile], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([tempfile], [mkdtemp], py_default_failure_mode)
dnl AC_PYTHON_MODULE([tempfile], [mkstemp], py_default_failure_mode)
dnl AC_PYTHON_MODULE([termios], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([textwrap], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([textwrap], [fill], py_default_failure_mode)
dnl AC_PYTHON_MODULE([thread], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([threading ], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([threading], [*], py_default_failure_mode)
dnl AC_PYTHON_MODULE([threading], [Thread], py_default_failure_mode)
dnl AC_PYTHON_MODULE([threading], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([time], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([time], [sleep], py_default_failure_mode)
dnl AC_PYTHON_MODULE([time], [localtime], py_default_failure_mode)
dnl AC_PYTHON_MODULE([time], [strftime], py_default_failure_mode)
dnl AC_PYTHON_MODULE([time], [strptime], py_default_failure_mode)
dnl AC_PYTHON_MODULE([time], [asctime], py_default_failure_mode)
dnl AC_PYTHON_MODULE([time], [time], py_default_failure_mode)
dnl AC_PYTHON_MODULE([traceback], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([types], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([types], [*], py_default_failure_mode)
dnl AC_PYTHON_MODULE([unittest], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([urllib2], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([urllib], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([urlparse], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([urlparse], [urlparse], py_default_failure_mode)
dnl AC_PYTHON_MODULE([weakref], [], py_default_failure_mode)
dnl AC_PYTHON_MODULE([weakref], [ref], py_default_failure_mode)

m4_popdef([py_default_failure_mode])
])
