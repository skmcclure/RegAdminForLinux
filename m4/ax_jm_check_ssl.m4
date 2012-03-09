AC_DEFUN([AX_JM_CHECK_SSL],[

  AC_MSG_CHECKING([checking for OpenSSL])
  AC_MSG_RESULT([])

  for dir in $withval /usr/local/ssl /usr/lib/ssl /usr/ssl /usr/pkg; do
    ssldir="$dir"
    if test -f "$dir/include/openssl/des.h"; then
      SSL_CFLAGS="-I$ssldir/include"
      SSL_CXXFLAGS="I$ssldir/include"
      SSL_LDFLAGS="-L$ssldir/lib"
      break
    fi
  done

  AC_CHECK_HEADERS([openssl/des.h openssl/md4.h])

  HAVE_SSL="1"
  SSL_LIBS=""
  AC_CHECK_LIB([crypto], [main], [SSL_LIBS="-lcrypto"])
  dnl AC_CHECK_LIB([ssl], [main], [SSL_LIBS="$SSL_LIBS -lssl"])

  HAVE_GOOD_CRYPTO="no"
  if test -n "$SSL_LIBS" ; then
    HAVE_GOOD_CRYPTO="yes"
    save_LIBS="$LIBS"
    LIBS="$SSL_LIBS $LIBS"

    HAVE_UNDERSCORE_M4_FUNCS="0"
    AC_CHECK_FUNCS([MD4Init MD4Update MD4Final], [
      ajcs_found_md4="yes"
    ], [
      ajcs_found_md4="no"
    ])
    if test x$ajcs_found_md4 = "xno" ; then
      AC_CHECK_FUNCS([MD4_Init MD4_Update MD4_Final], [
        HAVE_UNDERSCORE_M4_FUNCS="1"
      ], [
        HAVE_GOOD_CRYPTO="no"
      ])
    fi
    if test x$HAVE_UNDERSCORE_M4_FUNCS = "x1" ; then
      AC_DEFINE([HAVE_UNDERSCORE_M4_FUNCS], [1], [Whether m4 funcs have an underscore])
      AC_MSG_NOTICE([Setting HAVE_UNDERSCORE_M4_FUNCS])
    fi
    unset ajcs_found_md4

    HAVE_UPPER_CASE_DES_FUNCS="0"
    AC_CHECK_FUNCS([des_ecb_encrypt des_set_key], [
      ajcs_found_des="yes"
    ], [
      ajcs_found_des="no"
    ])
    if test x$ajcs_found_des = "xno" ; then
      AC_CHECK_FUNCS([DES_ecb_encrypt DES_set_key], [
        HAVE_UPPER_CASE_DES_FUNCS="1"
      ], [
        HAVE_GOOD_CRYPTO="no"
      ])
    fi
    if test x$HAVE_UPPER_CASE_DES_FUNCS = "x1"; then
      AC_DEFINE([HAVE_UPPER_CASE_DES_FUNCS], [1], [Whether des funcs use uppercase prefixes])
      AC_MSG_NOTICE([Setting HAVE_UPPER_CASE_DES_FUNCS])
    fi
    unset ajcs_found_des

    LIBS="$save_LIBS"
    unset save_LIBS
  fi

  if test x$HAVE_GOOD_CRYPTO = xno ; then
    SSL_CFLAGS=""
    SSL_CXXFLAGS=""
    SSL_LIBS=""
    SSL_LDFLAGS=""
    HAVE_SSL="0"
    AC_MSG_ERROR([Crypto library doesn't exist or doesn't have required functions.  Failing...])
  else
    AC_DEFINE([HAVE_SSL], [1], [Whether we have openssl])
  fi

  AC_SUBST([HAVE_GOOD_CRYPTO])
  AC_SUBST(HAVE_SSL)
  AC_SUBST(SSL_CFLAGS)
  AC_SUBST(SSL_CXXFLAGS)
  AC_SUBST(SSL_LIBS)
  AC_SUBST(SSL_LDFLAGS)
])
