#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([vmaj], [0])
m4_define([vmin], [5])
m4_define([vmic], [0])

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
AM_ICONV

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

# Checks for (mandatory) libraries.
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

# --with-pcrecpp
AC_ARG_WITH([pcrecpp],
    [AS_HELP_STRING([--with-pcrecpp=PREFIX],
        [search for libpcrecpp in PREFIX @<:@default=pkg-config returned path@:>@])],
    [with_pcrecpp=$withval],
    [with_pcrecpp=no]
)
if [ test x"$with_pcrecpp" != "xno" ]; then
    if [ test x"$with_pcrecpp" = "xyes" ]; then
        PKG_CHECK_MODULES(libpcrecpp, libpcrecpp >= 7.8)
    else
        # User knows better.
        libpcrecpp_CFLAGS="-I$with_pcrecpp/include"
        libpcrecpp_LIBS="-L$with_pcrecpp/lib -lpcrecpp"
    fi
    if [ test x"$libpcrecpp_LIBS" = "x" ]; then
        AC_MSG_ERROR([libpcrecpp not found])
    fi
    AC_SUBST(libpcrecpp_CFLAGS)
    AC_SUBST(libpcrecpp_LIBS)
    AC_DEFINE([WITH_PCRE], [1], [With libpcrecpp])
fi
AM_CONDITIONAL([WITH_PCRE], [test x"$with_pcre" != "xno"])

# --with-man
AC_ARG_WITH([man],
    [AS_HELP_STRING([--with-man],
        [install cclive.1 manual page @<:@default=yes@:>@])],
    [],
    [with_man=yes])
AM_CONDITIONAL([WITH_MAN], [test x"$with_man" != "xno"])

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
                 tests/Makefile
                 docs/Makefile])
AC_OUTPUT
