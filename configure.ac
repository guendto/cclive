#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([vmaj], [0])
m4_define([vmin], [4])
m4_define([vmic], [7])

AC_PREREQ(2.62)
AC_INIT([cclive], vmaj[.]vmin[.]vmic)
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_SYSTEM
AC_DEFINE_UNQUOTED(CANONICAL_TARGET, "$target", [Canonical target])
AC_DEFINE_UNQUOTED(CANONICAL_HOST, "$host", [Canonical host])

AM_INIT_AUTOMAKE()

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_GNU_SOURCE

AC_PATH_PROG(GENGETOPT, gengetopt, no)
AM_CONDITIONAL([HAVE_GENGETOPT], [test x"$GENGETOPT" != "xno"])
AC_SUBST(GENGETOPT)

AC_PATH_PROG(PODCHECKER, podchecker, no)
AM_CONDITIONAL([HAVE_PODCHECKER], [test x"$PODCHECKER" != "xno"])
AC_SUBST(PODCHECKER)

AC_PATH_PROG(POD2MAN, pod2man, no)
AM_CONDITIONAL([HAVE_POD2MAN], [test x"$POD2MAN" != "xno"])
AC_SUBST(POD2MAN)

AC_PATH_PROG(POD2HTML, pod2html, no)
AM_CONDITIONAL([HAVE_POD2HTML], [test x"$POD2HTML" != "xno"])
AC_SUBST(POD2HTML)

# Checks for libraries.
PKG_CHECK_MODULES(libcurl, libcurl >= 7.18.0)

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stdlib.h string.h sys/ioctl.h unistd.h sys/wait.h signal.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([memset strchr strcspn strspn strtol])

# --with-man
AC_ARG_WITH([man],
    [AS_HELP_STRING([--with-man],
        [install cclive.1 manual page @<:@default=yes@:>@])],
    [],
    [with_man=yes])
AM_CONDITIONAL([WITH_MAN], [test x"$with_man" != "xno"])

# --enable-sigwinch
AC_ARG_ENABLE([sigwinch],
    [AS_HELP_STRING([--enable-sigwinch],
        [enable SIGWINCH handling @<:@default=no@:>@])],
    [],
    [enable_sigwinch=no])
if [ test x"$enable_sigwinch" = "xyes" ]; then
    AC_DEFINE([USE_SIGWINCH], [1], [Define to enable SIGWINCH handling])
fi

# --enable-tests
AC_ARG_ENABLE([tests],
    [AS_HELP_STRING([--enable-tests],
        [enable "make check" tests @<:@default=no@:>@])],
    [],
    [enable_tests=no])
AM_CONDITIONAL([ENABLE_TESTS], [test x"$enable_tests" != "xno"])

# --with-iconv
AC_ARG_WITH([iconv],
    [AS_HELP_STRING([--with-iconv[=prefix]],
        [build with libiconv @<:@default=no@:>@])],
    [with_iconv=$withval],
    [with_iconv=no])
if [ test x"$with_iconv" != "xno" ]; then
    if [ test x"$with_iconv" = "xyes" ]; then
        AC_CHECK_HEADER([iconv.h],
            AC_CHECK_LIB([iconv], [iconv_open], [libiconv_LDFLAGS=-liconv])
        )
    else
        # Use prefix user supplied instead.
        libiconv_CFLAGS="-I$with_iconv/include"
        libiconv_LDFLAGS="-L$with_iconv/lib -liconv"
    fi
    AC_SUBST(libiconv_CFLAGS)
    AC_SUBST(libiconv_LDFLAGS)
    AC_DEFINE([WITH_ICONV], [1], [With libiconv])
fi
AM_CONDITIONAL([WITH_ICONV], [test x"$with_iconv" != "xno"])

# --with-perl
#AC_ARG_WITH([perl],
#    [AS_HELP_STRING([--with-perl],
#        [embed Perl to enable title parsing @<:@default=no@:>@])],
#    [],
#    [with_perl=no])
#if [ test x"$with_perl" = "xyes" ]; then
#    AC_PATH_PROG(PERL, perl, no)
#    if [ test x"$PERL" = "xno" ]; then
#        AC_MSG_ERROR([perl not found in path])
#    fi
#    perl_CCOPTS=`$PERL -MExtUtils::Embed -e ccopts`
#    perl_LDOPTS=`$PERL -MExtUtils::Embed -e ldopts`
#    AC_SUBST(perl_CCOPTS)
#    AC_SUBST(perl_LDOPTS)
#    AC_MSG_CHECKING([for HTML::TokeParser])
#    if $PERL -MHTML::TokeParser >/dev/null 2>&1; then
#        AC_MSG_RESULT([yes])
#    else
#        AC_MSG_ERROR([HTML::TokeParser module is required])
#    fi
#    AC_DEFINE([WITH_PERL], [1], [Whether Perl is embedded])
#fi
#AM_CONDITIONAL([WITH_PERL], [test x"$with_perl" != "xno"])

# Check host.
case "$host" in
    *-*-mingw32*)
        AC_DEFINE([HOST_W32], [1], [...])
        host_w32=true
    ;;
    *)
        AC_FUNC_MALLOC
        AC_FUNC_REALLOC
    ;;
esac

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile])
AC_OUTPUT
