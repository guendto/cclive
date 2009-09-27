#!/bin/sh

# Configure
HOST=i486-mingw32 # ./configure --host=$HOST
PREFIX=`pwd`/dist # ./configure --prefix=$PREFIX
CXXFLAGS="-Os -pipe -march=i686"

CURL_CONFIG=\
"/home/legatvs/src/non-installed/curl-7.19.6/tmp/dist/bin/curl-config"

PCRE_CONFIG=\
"/home/legatvs/src/non-installed/pcre-7.9/tmp/dist/bin/pcre-config"

#ICONV_PREFIX=\ # Unused
#"/home/legatvs/src/non-installed/libiconv-1.13.1/tmp/dist"

pack_it()
{
    curl_prefix="`$CURL_CONFIG --prefix`"
    curl_dll="$curl_prefix/bin/libcurl-4.dll"

    pcre_prefix="`$PCRE_CONFIG --prefix`"
    pcre_dll="$pcre_prefix/bin/libpcre-0.dll"
    pcrecpp_dll="$pcre_prefix/bin/libpcrecpp-0.dll"

    version=`awk '/PACKAGE_VERSION = / {print $3}' Makefile`
    archive="cclive-$version-win32-i686-bin.7z"
    distdir="cclive-$version"

    rm -rf dist cclive-$version $archive \
    && make install-strip \
    && make man \
    && cp $curl_dll dist/bin \
    && cp $pcre_dll dist/bin \
    && cp $pcrecpp_dll dist/bin \
    && cp ../COPYING dist/cclive-COPYING.TXT \
    && cp README.w32.TXT dist/ \
    && cp cclive.1.html dist/ \
    && rm -rf dist/share \
    && mv dist $distdir \
    && 7za a $archive $distdir
    exit $?
}

pack_flag=off
while [ $# -gt 0 ]
do
    case "$1" in
        -p) pack_flag=on;;
         *) break;;
    esac
    shift
done

if [ x"$pack_flag" = "xon" ]; then
    pack_it
fi

# No tweaking usually required.

export libcurl_CFLAGS="`$CURL_CONFIG --cflags`"
export libcurl_LIBS="`$CURL_CONFIG --libs`"

export libpcrecpp_CFLAGS="`$PCRE_CONFIG --cflags`"
export libpcrecpp_LIBS="`$PCRE_CONFIG --libs` -lpcrecpp"

CXXFLAGS=$CXXFLAGS ../configure \
    --prefix="$PREFIX" \
    --host="$HOST" \
     --without-man
#     --with-libiconv-prefix="$ICONV_PREFIX"


