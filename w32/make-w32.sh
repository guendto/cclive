#!/bin/sh

# Configure: misc. settings

HOST=i486-mingw32 # FreeBSD: mingw32
PREFIX="`pwd`/dist"

export CXXFLAGS="-Os -pipe -march=i686"


# Configure: paths

CURL_CONFIG=\
"/home/legatvs/src/non-installed/curl-7.19.6/tmp/dist/bin/curl-config"

PCRE_CONFIG=\
"/home/legatvs/src/non-installed/pcre-7.9/tmp/dist/bin/pcre-config"

ICONV_PREFIX=\
"/home/legatvs/src/non-installed/libiconv-1.13.1/tmp/dist"


# No tweaking usually required.

export libcurl_CFLAGS="`$CURL_CONFIG --cflags`"
export libcurl_LIBS="`$CURL_CONFIG --libs`"

export libpcrecpp_CFLAGS="`$PCRE_CONFIG --cflags`"
export libpcrecpp_LIBS="`$PCRE_CONFIG --libs` -lpcrecpp"

../configure --prefix="$PREFIX" --host="$HOST" \
     --with-libiconv-prefix="$ICONV_PREFIX" --without-man \
    && make install-strip


