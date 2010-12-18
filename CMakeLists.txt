
cmake_minimum_required (VERSION 2.8.0)

project (cclive)

add_definitions (-Wall)
#add_definitions (-Werror)

# Version.

set (VERSION "0.7.0.1")
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

# Prerequisites.

include (FindPkgConfig)

pkg_check_modules (quvi     libquvi>=0.2.0 REQUIRED)
include_directories (AFTER ${quvi_INCLUDE_DIRS})
link_directories (${quvi_LIBRARY_DIRS})

pkg_check_modules (curl     libcurl>=7.20 REQUIRED)
include_directories (AFTER ${curl_INCLUDE_DIRS})
link_directories (${curl_LIBRARY_DIRS})

pkg_check_modules (pcre libpcrecpp>=8.10 REQUIRED)
include_directories (AFTER ${pcre_INCLUDE_DIRS})
link_directories (${pcre_LIBRARY_DIRS})

pkg_check_modules (pcrecpp  libpcrecpp>=8.10 REQUIRED)
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

find_package (Boost 1.42.0 COMPONENTS ${boost_LIBS} REQUIRED)

include_directories (AFTER ${Boost_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIRS})

set (boost_VERSION
    "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")

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

# Subdirs.

subdirs (lib cclive man1)

# Status.

message (STATUS)
message (STATUS "Configuration for")
message (STATUS "   cclive ${VERSION_LONG}")
message (STATUS)
message (STATUS "Options:")
message (STATUS "   Install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS "   Host setup    : ${HOST_SETUP}")
message (STATUS "   Build type    : ${CMAKE_BUILD_TYPE}")
message (STATUS "       (debug, release, relwithdebinfo, minsizerel)")
message (STATUS "   Verbose make  : ${CMAKE_VERBOSE_MAKEFILE}")
message (STATUS "   Without doc   : ${WITHOUT_DOC}")
message (STATUS)
message (STATUS "Found:")
message (STATUS "   Compiler: ${CMAKE_CXX_COMPILER}")
message (STATUS "   Linker  : ${CMAKE_LINKER}")
message (STATUS "   Make    : ${CMAKE_MAKE_PROGRAM}")
message (STATUS "   quvi    : ${quvi_VERSION}")
message (STATUS "   curl    : ${curl_VERSION}")
message (STATUS "   pcre    : ${pcre_VERSION}")
message (STATUS "   boost   : ${boost_VERSION}")
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

# CPack.

include (InstallRequiredSystemLibraries)

set (CPACK_SOURCE_PACKAGE_FILE_NAME
    "cclive-${VERSION}"
    CACHE INTERNAL
    "tarball basename"
)

set (CPACK_PACKAGE_FILE_NAME ${CPACK_SOURCE_PACKAGE_FILE_NAME})
set (CPACK_GENERATOR "TGZ")
set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/COPYING)
set (CPACK_RESOURCE_FILE_README  ${CMAKE_CURRENT_SOURCE_DIR}/README)

set (CPACK_SOURCE_IGNORE_FILES
    "/.git"
    "/.cmake/"
    "/CMakeFiles/"
    "/CMakeCache/"
    "/Makefile/"
    "/tmp/"
    "/OLD/"
    "/old/"
    "TODO"
    "/.gar"
)

# Notice the inclusion here, after setting the cpack variables.
include (CPack)

# dist.

add_custom_target (
    dist
    git log --stat --after="Thu Aug 12 15:54:58 2010" >
        "${CMAKE_CURRENT_SOURCE_DIR}/ChangeLog" && make package_source
)

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


