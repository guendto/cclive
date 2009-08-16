#!/bin/sh

# These steps describe mingw32 installation and curl building
# on FreeBSD. They serve as a reference for any furture head
# scratching. Although they have been intended for FreeBSD,
# they should port fairly easily to other systems.
#
# mingw32:
# * pkg-add -r mingw32-gcc mingw32-binutils mingw32-bin-msvcrt
#
# libcurl:
# * wget $curl_url ; tar xjf $curl_release.tar.bz2; cd $curl_release
# * CFLAGS="-O2 -pipe -march=i586" ./configure --host=mingw32 --prefix=`pwd`/dist --without-ssl --without-ipv6 --without-random --disable-ldap && make install-strip
# libiconv:
# * wget $iconv_url ; tar xjf $iconv_release.tar.bz2; cd $iconv_release
# * ./configure --host=mingw32 --prefix=`pwd`/dist && make install
#
# pcre:
# * wget ...
# * ./configure --host=mingw32 --prefix=`pwd`/dist && make install-strip
#
# cclive:
# * Edit paths as needed below
# * Run this script

# Edit as needed:
curl_config="/usr/home/legatvs/src/curl-7.19.6/dist/bin/curl-config"
iconv_prefix="/usr/home/legatvs/src/libiconv-1.13.1/dist"
pcre_config="/usr/home/legatvs/src/pcre-7.9/dist/bin/pcre-config"

export libcurl_CFLAGS="`$curl_config --cflags`"
export libcurl_LIBS="`$curl_config --libs`"
#echo $libcurl_CFLAGS
#echo $libcurl_LIBS

export CXXFLAGS="-Wall -O2 -march=i586 -fno-strict-aliasing -pipe"
export CFLAGS="$CXXFLAGS"
#echo $CXXFLAGS
#echo $CFLAGS

export libpcrecpp_CFLAGS="`$pcre_config --cflags`"
export libpcrecpp_LIBS="`$pcre_config --libs` -lpcrecpp"
#echo $libpcrecpp_CFLAGS
#echo $libpcrecpp_LIBS

# Tested only on FreeBSD. Edit prefix as needed.
./configure --host=mingw32 --prefix=`pwd`/dist --with-libiconv-prefix="$iconv_prefix" --without-man \
    && make \
    && echo "'make install' installs the binaries to ./dist" \
    && echo "'./mingw32-package.sh' creates zip archive containing the binaries"
exit $?

