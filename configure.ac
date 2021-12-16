AC_INIT([lundftpd],[3.18], [bug-report@lundman.net])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign])
# The big search for OpenSSL
# copied from openssh's configure.ac
tryssldir=""
AC_ARG_WITH(ssl-dir,
        [  --with-ssl-dir=PATH     Specify path to OpenSSL installation ],
        [
                if test "x$withval" != "xno" ; then
                        tryssldir=$withval
                fi
        ]
)
AC_PROG_CC

AC_CHECK_PROG(BUILD, build, build)

# Run ./configure in lion
AC_CONFIG_SUBDIRS([lion])

#AC_CHECK_FILE([lion/src/lion.h], ,
#   AC_MSG_ERROR([lion.h not found in lion/src/ directory]))

CFLAGS="$CFLAGS -I../lion/src/ -I../lion/contrib/libdirlist/"

AC_TYPE_SIGNAL

AC_HEADER_TIME

AC_CHECK_HEADERS([stdarg.h varargs.h sys/time.h sys/types.h dirent.h sys/file.h sys/param.h sys/statfs.h sys/vfs.h sys/statvfs.h errno.h])

# mount depends on param
AC_CHECK_HEADERS([sys/mount.h], [], [], [#if HAVE_SYS_PARAM_H
                                         #include <sys/param.h>
                                         #endif])

AC_STRUCT_DIRENT_D_TYPE

AH_TEMPLATE([NEED_SYS_ERRLIST_DECLARED],
        [Define this if sys_errlist must be declared (if it exists).])

AC_SYS_LARGEFILE



AC_SEARCH_LIBS([gethostbyname], [nsl])
AC_SEARCH_LIBS([socket], [socket inet])
AC_SEARCH_LIBS([crypt], [crypt])

AC_CHECK_FUNC([snprintf],
                [AC_DEFINE(HAVE_SNPRINTF,1,[Presence of snprintf found])],
                [AC_MSG_WARN(Adding internal snprintf function)])



# Check what DB include files we have.
AC_CHECK_HEADERS(db.h, has_db_h=1)

AC_CHECK_HEADERS(gdbm.h, has_gdbm_h=1)

AC_CHECK_HEADERS(ndbm.h, has_ndbm_h=1)

if test "x$ac_cv_prog_gcc" = xyes ; then
   CFLAGS="$CFLAGS -Wall -g "
fi
# Check what extra libraries we might need:
# Also, on systems that might have more than one db support present,
# we pick just one. You may end up with more than one HAVE_* but it should
# only define ONE USE_*
if test -n "$has_db_h" ; then
 AC_DEFINE([DUPECHECK],[],[Support for dupecheck])
 AC_DEFINE([USE_DB_H],1,[Support for db.h])
 AC_CHECK_LIB(db, dbopen, [LIBS="$LIBS -ldb"])
 AC_MSG_NOTICE([Enabling DUPECHECK using ... db.h])
elif test -n "$has_gdbm_h" ; then
 AC_DEFINE([DUPECHECK],[],[Support for dupecheck])
 AC_DEFINE([USE_GDBM_H],1,[Support for gdbm.h])
 AC_CHECK_LIB(gdbm, gdbm_open, [LIBS="$LIBS -lgdbm"])
 AC_MSG_NOTICE([Enabling DUPECHECK using ... gdbm.h])
elif test -n "$has_ndbm_h" ; then
 AC_DEFINE([DUPECHECK],[],[Support for dupecheck])
 AC_DEFINE([USE_NDBM_H],1,[Support for ndbm.h])
 AC_CHECK_LIB(ndbm, dbm_open, [LIBS="$LIBS -lndbm"])
 AC_MSG_NOTICE([Enabling DUPECHECK using ... ndbm.h])
else
 AC_MSG_NOTICE([Disabling DUPECHECK ... no working db.h, gdbm.h or ndbm.h found])
fi


AC_DEFINE([RACESTATS],[],[Support for racestats])
AC_MSG_NOTICE([Enabling RACESTATS...])

AC_CHECK_FUNCS([strtoul strtouq strtoull strtoll])


dnl ------------------------------------------------------
dnl Where do you live, openssl?  And how do we call you?
AC_ARG_WITH([openssl],
            [AS_HELP_STRING([--without-openssl],[Disable openssl support])],
            [],
            [with_openssl=yes])

dnl If SSL was NOT disable, then go look for it.
if test x"$with_openssl" != x"no" ; then

AC_CACHE_CHECK([for OpenSSL directory], ac_cv_openssl_dir, [
  saved_LIBS="$LIBS"
  saved_LDFLAGS="$LDFLAGS"
  saved_CPPFLAGS="$CPPFLAGS"
  ssl_found=no
  for ssldir in $tryssldir "" $prefix /usr/local/openssl /usr/lib/openssl /usr/local/ssl /usr/lib/ssl /usr/local /usr/athena /usr/pkg /opt /opt/openssl ; do
    LDFLAGS="$saved_LDFLAGS"
    LIBS="$saved_LIBS -lssl -lcrypto"

    # Skip the directory if it isn't there.
    if test ! -z "$ssldir" -a ! -d "$ssldir" ; then
       continue;
    fi
    if test ! -z "$ssldir" ; then
      if test -d "$ssldir/lib" ; then
        LDFLAGS="-L$ssldir/lib $LDFLAGS"
      else
        LDFLAGS="-L$ssldir $LDFLAGS"
      fi
      if test -d "$ssldir/include" ; then
        CPPFLAGS="-I$ssldir/include $CPPFLAGS"
      else
        CPPFLAGS="-I$ssldir $CPPFLAGS"
      fi
    fi
    # Can I link it?
    AC_TRY_LINK([#include <openssl/rand.h>],
       [ RAND_add((void*)0,0,0); ],
       [ openssl_linked=yes ], [ openssl_linked=no ])
    if test $openssl_linked = yes; then
       if test ! -z "$ssldir" ; then
         ac_cv_openssl_dir=$ssldir
       else
         ac_cv_openssl_dir="(system)"
       fi
       ssl_found=yes
       break
    fi
  done
  LIBS="$saved_LIBS"
  LDFLAGS="$saved_LDFLAGS"
  CPPFLAGS="$saved_CPPFLAGS"
  if test $ssl_found = no ; then
    AC_MSG_ERROR([Could not find a linkable OpenSSL. You can specify an explicit path using --with-ssl-dir])
  fi
])
AC_DEFINE([WITH_SSL], 1, [Define if we found OpenSSL - Legacy name])
AC_DEFINE([IRCBOT],[],[Support for ircbots])
AC_MSG_NOTICE([Enabling IRCBOT...])
LIBS="$LIBS -lssl -lcrypto"
if test "$ac_cv_openssl_dir" != "(system)"; then
  if test -d "$ac_cv_openssl_dir/lib" ; then
    LDFLAGS="-L$ac_cv_openssl_dir/lib $LDFLAGS"
    ssl_libdir="$ac_cv_openssl_dir/lib"
  else
    LDFLAGS="-L$ac_cv_openssl_dir $LDFLAGS"
    ssl_libdir="$ac_cv_openssl_dir"
  fi
  if test -d "$ac_cv_openssl_dir/include" ; then
    CPPFLAGS="-I$ac_cv_openssl_dir/include $CPPFLAGS"
  else
    CPPFLAGS="-I$ac_cv_openssl_dir $CPPFLAGS"
  fi
fi

if test -z $CROSS_COMPILE
then
AC_CACHE_CHECK([whether we need extra options to link OpenSSL],
               ac_cv_openssl_linker_option, [
    saved_LDFLAGS="$LDFLAGS"
    ssl_runs=no
    linked_with=nothing
    for ssl_extra in "" "-Wl,-R$ssl_libdir" "-R$ssl_libdir" ; do
      LDFLAGS="$ssl_extra $saved_LDFLAGS"
      AC_TRY_RUN([
#include <string.h>
#include <openssl/rand.h>
int main(void)
{
        char a[2048];
        memset(a, 0, sizeof(a));
        RAND_add(a, sizeof(a), sizeof(a));
        return(RAND_status() <= 0);
}
                        ],
                openssl_runs=yes, openssl_runs=no)
      if test $openssl_runs = yes ; then
        if test "$linked_with" = nothing; then
           linked_with="$ssl_extra"
        fi
        AC_TRY_RUN([
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
int main(void) {
return (OPENSSL_VERSION_NUMBER == SSLeay()) == 0;
}],
        right_version=yes, right_version=no)
        if test "$right_version" = yes; then
          if test -z "$ssl_extra" ; then
            ac_cv_openssl_linker_option='(none)'
          else
            ac_cv_openssl_linker_option=$ssl_extra
          fi
          ssl_runs=yes
          break
        fi
     fi
    done
    if test $ssl_runs = no ; then
      if test "$linked_with" = 'nothing' ; then
        AC_MSG_ERROR([Found linkable OpenSSL in $ac_cv_openssl_dir, but it doesn't run, even with -R.  Maybe specify another using --with-ssl-dir?])
      else
        if test -z "$linked_with" ; then
            ac_cv_openssl_linker_option='(none)'
        else
            ac_cv_openssl_linker_option=$linked_with
        fi
        AC_MSG_WARN([I managed to make OpenSSL link and run, but I couldn't make it link against the same version I found header files for.])
      fi
    fi
    LDFLAGS="$saved_LDFLAGS"
])

if test "$ac_cv_openssl_linker_option" != '(none)' ; then
   LDFLAGS="$ac_cv_openssl_linker_option $LDFLAGS"
fi
fi

dnl if NOSSL
fi


AC_LANG_PUSH(C)
dnl solaris has some odd defines it needs in order to propperly compile ctime_r
 	AC_MSG_CHECKING([if ctime_r need special care to act posixly correct])
 	AC_LINK_IFELSE(
 	    AC_LANG_PROGRAM(
 	           [[#include <time.h>]],
 	           [[ctime_r(NULL,NULL,0)]]
 	                   ),
 	    [ CPPFLAGS="$CPPFLAGS -D_POSIX_PTHREAD_SEMANTICS"
 	      AC_LINK_IFELSE(
 	          AC_LANG_PROGRAM(
 	                [[#include <time.h>]],
 	                [[ctime_r(NULL,NULL)]]
 	                         ),
 	          [AC_MSG_RESULT([yes, this seems to be solaris style])],
 	          [AC_MSG_ERROR([Can't figure how to compile ctime_r])]
 	      )
 	    ],
 	    [ AC_LINK_IFELSE(
 	          AC_LANG_PROGRAM(
 	                [[#include <time.h>]],
 	                [[ctime_r(NULL,NULL)]]
 	                         ),
 	          [AC_MSG_RESULT(no)],
 	          [AC_MSG_ERROR([Can't figure how to compile ctime_r])]
 	      )
 	    ]
 	)
AC_LANG_POP(C)

dnl AC_CONFIG_HEADERS([config.h])
AM_CONFIG_HEADER(config.h)
AC_CONFIG_FILES([Makefile src/Makefile win32/Makefile win32/lundftpd/Makefile win32/lundftpd/lundftpd/Makefile eyot/Makefile eyot/conf/Makefile])
AC_OUTPUT

