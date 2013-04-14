#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])

AC_INIT([cclive], m4_esyscmd([./gen-ver.sh -c | tr -d '\n']),
        [http://cclive.sf.net/bugs/],[],[http://cclive.sf.net/])

AC_CONFIG_SRCDIR([src/cc/application.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([config.aux])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET
AC_USE_SYSTEM_EXTENSIONS

AC_DEFINE_UNQUOTED([CANONICAL_TARGET], "$target", [...])
AC_DEFINE_UNQUOTED([CANONICAL_HOST], "$host", [...])

AM_INIT_AUTOMAKE([1.11.1 -Wall -Werror dist-xz no-dist-gzip tar-ustar])
AM_SILENT_RULES([yes])
#AM_PROG_AR

LT_INIT([disable-static])
LT_PREREQ([2.2.6])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LN_S

AC_PATH_PROG([A2X], [a2x], [no])
AM_CONDITIONAL([HAVE_A2X], [test x"$A2X" != "xno"])
AC_SUBST([A2X])

# Checks for libraries.

have_libquvi_0_9=
PKG_CHECK_MODULES([libquvi], [libquvi-0.9 >= 0.9],
  [
    AC_DEFINE([HAVE_LIBQUVI_0_9], [1], [We have libquvi 0.9])
    have_libquvi_0_9=yes
  ],
  [PKG_CHECK_MODULES([libquvi], [libquvi >= 0.4])])
AM_CONDITIONAL([HAVE_LIBQUVI_0_9], [test x"$have_libquvi_0_9" = "xyes"])

PKG_CHECK_MODULES([libcurl], [libcurl >= 7.18.0])
PKG_CHECK_MODULES([libpcre], [libpcre >= 8.02])
PKG_CHECK_MODULES([libpcrecpp], [libpcrecpp >= 8.02])

BOOST_REQUIRE([1.42.0])
# Boost: Header-only libs
BOOST_FOREACH
BOOST_FORMAT
# Boost: Libs requiring linking
BOOST_DATE_TIME
BOOST_PROGRAM_OPTIONS
BOOST_IOSTREAMS
BOOST_FILESYSTEM
BOOST_SYSTEM

# Checks for header files.
AC_CHECK_HEADERS([sys/ioctl.h unistd.h sys/types.h sys/stat.h signal.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_PID_T
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([strerror strstr getpid signal memset setlocale])
AC_FUNC_ERROR_AT_LINE
AC_FUNC_STRERROR_R
AC_FUNC_FORK

# Version.
VN=`$srcdir/gen-ver.sh`
AC_DEFINE_UNQUOTED([VN],["$VN"], [We have version number from gen-ver.sh])

# --with-manual
AC_ARG_WITH([manual],
  [AS_HELP_STRING([--with-manual],
    [Install manual page(s) @<:@default=yes@:>@])],
  [],
  [with_manual=yes])
AM_CONDITIONAL([WITH_MANUAL], [test x"$with_manual" != "xno"])

# --enable-ccl
AC_ARG_ENABLE([ccl],
  [AS_HELP_STRING([--enable-ccl],
    [Symlink from 'cclive' executable to 'ccl' @<:@default=yes@:>@])],
  [],
  [enable_ccl=yes])
AM_CONDITIONAL([ENABLE_CCL], [test x"$enable_ccl" = "xyes"])

AC_CONFIG_FILES([
  Makefile
  doc/Makefile
  doc/man1/Makefile
  src/Makefile
])

AC_OUTPUT

AC_MSG_NOTICE([
  version:    ${VERSION}
  prefix:     ${prefix}
  compiler:   ${CXX}
  flags:      ${CXXFLAGS}
System types:
  target      ${target}
  build       ${build}
  host        ${host}
Options:
  add ccl symlink: ${enable_ccl}
  install manual : ${with_manual}])

# vim: set ts=2 sw=2 tw=72 expandtab:
