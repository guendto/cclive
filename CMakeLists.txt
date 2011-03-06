
cmake_minimum_required (VERSION 2.8.0)

project (cclive)

add_definitions (-Wall)
#add_definitions (-Werror)

# Version.

set (VERSION "0.7.3")
set (VERSION_LONG "${VERSION}")
set (BRANCH "master")

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    find_program (GIT_PROG "git")
    if (GIT_PROG)
        execute_process (
            COMMAND ${GIT_PROG} "describe" "${BRANCH}"
            OUTPUT_VARIABLE DESCR
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if (DESCR)
            set (VERSION_LONG "${DESCR}")
        endif ()
    endif ()
endif ()

find_program (DATE_PROG "date")
if (DATE_PROG)
    execute_process (
        COMMAND ${DATE_PROG} "+%Y-%m-%d"
        OUTPUT_VARIABLE DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (DATE)
        set (VERSION_LONG "${VERSION_LONG} built on ${DATE}")
    endif ()
endif ()

set (HOST_SETUP "${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
set (VERSION_LONG "${VERSION_LONG} for ${HOST_SETUP}")

# gzip.

find_program (GZIP_PROG "gzip")

# pod2man.

find_program (POD2MAN_PROG "pod2man")
if (POD2MAN_PROG)
    add_custom_target (pod2man
        "${POD2MAN_PROG}" -c "cclive manual" -n cclive -s 1 -r ${VERSION}
        ${CMAKE_CURRENT_SOURCE_DIR}/man1/cclive.1.pod
        ${CMAKE_CURRENT_BINARY_DIR}/man1/cclive.1
    )
endif ()

# Set build type default.

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "debug")
endif ()

# Set verbose makefile default.

if (NOT CMAKE_VERBOSE_MAKEFILE)
    set (CMAKE_VERBOSE_MAKEFILE false)
endif ()

# Set WITHOUT_DOC default.

if (NOT WITHOUT_DOC)
    set (WITHOUT_DOC false)
endif ()

# Include dir.

include_directories (BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Prerequisites.

include (FindPkgConfig)

pkg_check_modules (quvi libquvi>=0.2.0 REQUIRED QUIET)
include_directories (AFTER ${quvi_INCLUDE_DIRS})
link_directories (${quvi_LIBRARY_DIRS})

if (quvi_VERSION VERSION_GREATER 0.2.7)
    set (HAVE_QUVIOPT_NOSHORTENED true)
endif ()

if (quvi_VERSION VERSION_GREATER 0.2.11)
    set (HAVE_QUVIOPT_CATEGORY true)
endif ()

pkg_check_modules (curl libcurl>=7.20 REQUIRED QUIET)
include_directories (AFTER ${curl_INCLUDE_DIRS})
link_directories (${curl_LIBRARY_DIRS})

pkg_check_modules (pcre libpcre>=8.02 REQUIRED QUIET)
include_directories (AFTER ${pcre_INCLUDE_DIRS})
link_directories (${pcre_LIBRARY_DIRS})

pkg_check_modules (pcrecpp libpcrecpp>=8.02 REQUIRED QUIET)
include_directories (AFTER ${pcrecpp_INCLUDE_DIRS})
link_directories (${pcrecpp_LIBRARY_DIRS})

# Prerequisites: Boost

set (boost_LIBS
    program_options
    iostreams
    filesystem
    system
# foreach   -- this is a header-only library
# format    -- ditto
# date_time -- ...
# random    -- ...
)

set (Boost_USE_STATIC_LIBS   OFF)
set (Boost_USE_MULTITHREADED OFF)
set (Boost_FIND_QUIETLY      ON)

find_package (Boost 1.42.0 COMPONENTS ${boost_LIBS} REQUIRED)

include_directories (AFTER ${Boost_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIRS})

set (boost_VERSION
    "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")

# Version 3 is a major revision with many new and improved features. It
# may, however, break some user code written for Version 2.
#
# To ease the transition, Boost releases 1.44 through 1.47 will supply
# both V2 and V3. Version 2 is the default version for Boost release 1.44
# and 1.45. Version 3 will be the default starting with release 1.46.
#   <http://www.boost.org/libs/filesystem/index.html>

if (boost_VERSION VERSION_GREATER 1.43.0)
    set (HAVE_BOOST_FILESYSTEM_VERSION_3 true)
endif ()

# Check for headers.

include (CheckIncludeFile)
check_include_file (unistd.h    HAVE_UNISTD_H)
check_include_file (sys/types.h HAVE_SYS_TYPES_H)
check_include_file (sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_file (sys/stat.h  HAVE_SYS_STAT_H)
check_include_file (signal.h    HAVE_SIGNAL_H)

# Check for functions

include (CheckFunctionExists)
check_function_exists (strerror_r   HAVE_STRERROR_R)
check_function_exists (strerror     HAVE_STRERROR)
check_function_exists (fork         HAVE_FORK)
check_function_exists (getcwd       HAVE_GETCWD)
check_function_exists (getpid       HAVE_GETPID)
check_function_exists (signal       HAVE_SIGNAL)

# Subdirs.

subdirs (src man1)

# Status.

message (STATUS)
message (STATUS "Configured:")
message (STATUS "   cclive ${VERSION_LONG}")
message (STATUS)
message (STATUS "Found:")
message (STATUS "   Compiler: ${CMAKE_CXX_COMPILER}")
message (STATUS "   Linker  : ${CMAKE_LINKER}")
message (STATUS "   Make    : ${CMAKE_MAKE_PROGRAM}")
message (STATUS "   Host    : ${HOST_SETUP}")
message (STATUS "   quvi    : ${quvi_VERSION}")
message (STATUS "   curl    : ${curl_VERSION}")
message (STATUS "   pcre    : ${pcre_VERSION}")
message (STATUS "   pcrecpp : ${pcrecpp_VERSION}")
message (STATUS "   boost   : ${boost_VERSION}")
message (STATUS)
message (STATUS "Options:")
message (STATUS "   Install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS "   Verbose make  : ${CMAKE_VERBOSE_MAKEFILE}")
message (STATUS "   Without doc   : ${WITHOUT_DOC}")
message (STATUS "   Build type    : ${CMAKE_BUILD_TYPE}")
message (STATUS "       (debug, release, relwithdebinfo, minsizerel)")
message (STATUS)

# Force these variables to be written to cache.

set (CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE PATH
    "Install destination for ${PROJECT_NAME}" FORCE)

set (CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Type of build: none, debug, release, relwithdebinfo minsizerel" FORCE)

set (CMAKE_VERBOSE_MAKEFILE "${CMAKE_VERBOSE_MAKEFILE}" CACHE BOOL
    "Build with verbose makefiles: true, false" FORCE)

set (WITHOUT_DOC "${WITHOUT_DOC}" CACHE BOOL
    "Install without documentation: true, false" FORCE)

# config.h

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

include_directories (${CMAKE_CURRENT_BINARY_DIR})

# Uninstall.

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target (
    uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
)


