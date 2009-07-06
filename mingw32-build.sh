#!/bin/sh

# These steps describe mingw32 installation and curl building
# on FreeBSD. They serve as a reference for any furture head
# scratching. Although they have been intended for FreeBSD,
# they should port fairly easily to other systems.
#
# 1) pkg-add -r mingw32-gcc mingw32-binutils mingw32-bin-msvcrt
# 2) wget $curl_url
# 3) tar xjf $curl_release.tar.bz2; cd $curl_release
# 4) ./configure --host=mingw32 --prefix=`pwd`/dist --without-ssl --without-ipv6 --without-random
# 5) Run this script.

# Edit as needed:
curl_config="/usr/home/legatvs/src/curl-7.19.5/dist/bin/curl-config"

export libcurl_CFLAGS="`$curl_config --cflags`"
export libcurl_LIBS="`$curl_config --libs`"
#echo $libcurl_CFLAGS
#echo $libcurl_LIBS

# Tested only on FreeBSD. Edit prefix as needed.
./configure --host=mingw32 --prefix=`pwd`/dist --without-man \
    && make \
    && echo "'make install' installs the binaries to ./dist" \
    && echo "'./mingw32-package.sh' creates zip archive containing the binaries"

exit $?

