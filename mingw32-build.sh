#!/bin/sh

# These tips cover the steps on FreeBSD, although the steps
# should be fairly easy to port to any other unix-like system.

# 1) pkg-add -r mingw32-gcc mingw32-binutils mingw32-bin-msvcrt
# 2) wget $curl_url
# 3) tar xjf $curl_release.tar.bz2; cd $curl_release
# 4) ./configure --host=mingw32 --prefix=`pwd`/dist --without-ssl --without-ipv6 --without-random
# 5) Run this script.

# NOTE: Edit this path to the above.
curl_config="/usr/home/legatvs/src/curl-7.19.4/dist/bin/curl-config"

export libcurl_CFLAGS="`$curl_config --cflags`"
export libcurl_LIBS="`$curl_config --libs`"

#echo $libcurl_CFLAGS
#echo $libcurl_LIBS

# Tested only on FreeBSD. Edit prefix as needed.
./configure --host=mingw32 --prefix=`pwd`/dist && make install

exit 0
