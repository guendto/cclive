#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([vmaj], [0])
m4_define([vmin], [3])
m4_define([vmic], [1])
m4_define([bugs], [legatvs@gmail.com])

AC_PREREQ(2.62)
AC_INIT([cclive], vmaj[.]vmin[.]vmic, [bugs])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_SYSTEM
AC_DEFINE_UNQUOTED(CANONICAL_TARGET, "$target", [Canonical target])
AC_DEFINE_UNQUOTED(CANONICAL_HOST, "$host", [Canonical host])

AM_INIT_AUTOMAKE()

# Checks for programs.
AC_PROG_CC

AC_PATH_PROG(GENGETOPT, gengetopt, no)
AM_CONDITIONAL([HAVE_GENGETOPT], [test x"$GENGETOPT" != "xno"])
AC_SUBST(GENGETOPT)

AC_PATH_PROG(POD2MAN, pod2man, no)
AM_CONDITIONAL([HAVE_POD2MAN], [test x"$POD2MAN" != "xno"])
AC_SUBST(POD2MAN)

# Checks for libraries.
PKG_CHECK_MODULES(libcurl, libcurl >= 7.18.0)

# Checks for header files.
AC_CHECK_HEADERS([memory.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([atexit bzero getpass memset strchr strcspn strdup strspn strstr strtol getopt_long])

# --with-man
AC_ARG_WITH([man],
    [AS_HELP_STRING([--with-man],
        [install with manual page @<:@default=yes@:>@])],
    [],
    [with_man=yes])
AM_CONDITIONAL([WITH_MAN], [test x"$with_man" != "xno"])

# --with-sigwinch
AC_ARG_WITH([sigwinch],
    [AS_HELP_STRING([--with-sigwinch],
        [support SIGWINCH @<:@default=yes@:>@])],
    [],
    [with_sigwinch=yes])
if [ test x"$with_sigwinch" = "xyes" ]; then
    AC_DEFINE([WITH_SIGWINCH], [1], [Define to enable support])
fi

# --with-perl
AC_ARG_WITH([perl],
    [AS_HELP_STRING([--with-perl],
        [embed Perl @<:@default=no@:>@])],
    [],
    [with_perl=no])
if [ test x"$with_perl" = "xyes" ]; then
    AC_PATH_PROG(PERL, perl, no)
    if [ test x"$PERL" = "xno" ]; then
        AC_MSG_ERROR([perl not found in path])
    fi
    perl_CCOPTS=`$PERL -MExtUtils::Embed -e ccopts`
    perl_LDOPTS=`$PERL -MExtUtils::Embed -e ldopts`
    AC_SUBST(perl_CCOPTS)
    AC_SUBST(perl_LDOPTS)
    AC_MSG_CHECKING([for HTML::TokeParser])
    if $PERL -MHTML::TokeParser >/dev/null 2>&1; then
        AC_MSG_RESULT([yes])
    else
        AC_MSG_ERROR([HTML::TokeParser module is required])
    fi
    AC_DEFINE([WITH_PERL], [1], [Whether Perl is embedded])
fi
AM_CONDITIONAL([WITH_PERL], [test x"$with_perl" != "xno"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT