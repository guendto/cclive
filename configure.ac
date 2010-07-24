#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.66)

m4_include([m4/version.m4])
AC_INIT([cclive], [_CCLIVE_VERSION], [legatvs@gmail.com])

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config.aux])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_SYSTEM
AC_GNU_SOURCE

AC_DEFINE_UNQUOTED(CANONICAL_TARGET, "$target", [Canonical target])
AC_DEFINE_UNQUOTED(CANONICAL_HOST, "$host", [Canonical host])

AM_INIT_AUTOMAKE([-Wall -Werror foreign dist-bzip2])
AM_SILENT_RULES([yes])

# Check host.
case "$host" in
    *-*-mingw32*)
        host_w32=yes
    ;;
esac

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P

AC_PATH_PROG(GENGETOPT, gengetopt, no)
AM_CONDITIONAL([HAVE_GENGETOPT], [test x"$GENGETOPT" != "xno"])
AC_SUBST(GENGETOPT)

AC_PATH_PROG(PERL, perl, no)
AM_CONDITIONAL([HAVE_PERL], [test x"$PERL" != "xno"])
AC_SUBST(PERL)

have_pod_checker=no
AC_MSG_CHECKING([for Pod::Checker])
if [ $PERL -mPod::Checker >/dev/null 2>&1 ]; then
    have_pod_checker=yes
fi
AC_MSG_RESULT([${have_pod_checker}])
AM_CONDITIONAL([HAVE_PERL_POD_CHECKER], [test x"$have_pod_checker" = "xyes"])

AC_PATH_PROG(POD2MAN, pod2man, no)
AM_CONDITIONAL([HAVE_POD2MAN], [test x"$POD2MAN" != "xno"])
AC_SUBST(POD2MAN)

AC_PATH_PROG(POD2HTML, pod2html, no)
AM_CONDITIONAL([HAVE_POD2HTML], [test x"$POD2HTML" != "xno"])
AC_SUBST(POD2HTML)

build_date=
AC_PATH_PROG(DATE, date, no)
if [ test x"$DATE" != "xno" ]; then
    build_date=`"$DATE" +%Y%m%d`
fi
AC_DEFINE_UNQUOTED([BUILD_DATE], "$build_date", [...])

# Checks for (mandatory) libraries.
PKG_CHECK_MODULES(libquvi, libquvi >= 0.2.0)
PKG_CHECK_MODULES(libcurl, libcurl >= 7.18.0)
PKG_CHECK_MODULES(libpcrecpp, libpcrecpp >= 7.8)

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stdlib.h string.h sys/ioctl.h unistd.h sys/wait.h signal.h locale.h langinfo.h fcntl.h limits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
if [ test x"$host_w32" = "xno" ]; then
    # mingw32-g++ (4.2.1) returns otherwise:
    #   error: '::malloc' has not been declared etc.
    AC_FUNC_MALLOC
    AC_FUNC_REALLOC
fi
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([memset setenv strchr strcspn strspn strtol setlocale nl_langinfo getcwd strerror strstr])

# --enable-tests
AC_ARG_ENABLE([tests],
    [AS_HELP_STRING([--enable-tests],
        [enable "make check" tests @<:@default=no@:>@])],
    [],
    [enable_tests=no])
AM_CONDITIONAL([ENABLE_TESTS], [test x"$enable_tests" != "xno"])

# --with-man
AC_ARG_WITH([man],
    [AS_HELP_STRING([--with-man],
        [install cclive.1 manual page @<:@default=yes@:>@])],
    [],
    [with_man=yes])
AM_CONDITIONAL([WITH_MAN], [test x"$with_man" != "xno"])

# Other.
if [ test x"$host_w32" = "xyes" ]; then
    AC_DEFINE([HOST_W32], [1], [Define to enable w32])
fi

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile
                 tests/ccliveTest.pm])
AC_OUTPUT

AC_MSG_NOTICE([Configured to build cclive:

    cclive version: ${VERSION}
    host setup:     ${host}
    install prefix: ${prefix}
    compiler:       ${CXX}
    cxxflags:       ${CXXFLAGS}

Options:
    install manual: ${with_man}

Other:
    gengetopt:      ${GENGETOPT}
    perl:           ${PERL}
    Pod::Checker:   ${have_pod_checker}
    pod2man:        ${POD2MAN}
    pod2html:       ${POD2HTML}
])


