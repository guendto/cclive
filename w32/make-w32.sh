#!/bin/sh

# Configure
HOST=i486-mingw32 # ./configure --host=$HOST
PREFIX=`pwd`/dist # ./configure --prefix=$PREFIX
CXXFLAGS="-Os -pipe -march=i686"

QUVI_PREFIX=\
"/home/legatvs/quvi.git/w32/quvi-0.2.0"

CURL_PREFIX=\
"/home/legatvs/src/non-installed/curl-7.20.1"

CURL_CONFIG=\
"$CURL_PREFIX/release/dist/bin/curl-config"

PCRE_PREFIX=\
"/home/legatvs/src/non-installed/pcre-8.02"

PCRE_CONFIG=\
"$PCRE_PREFIX/release/dist/bin/pcre-config"

ICONV_PREFIX=\
"/home/legatvs/src/non-installed/libiconv-1.13.1"

ICONV_DIST=\
"$ICONV_PREFIX/release/dist"

pack_it()
{
    quvi_dll="$QUVI_PREFIX/bin/libquvi-0.dll"
    quvi_lua="$QUVI_PREFIX/bin/lua"
    lua_dll="$QUVI_PREFIX/bin/lua51.dll"

    curl_prefix="`$CURL_CONFIG --prefix`"
    curl_dll="$curl_prefix/bin/libcurl-4.dll"

    pcre_prefix="`$PCRE_CONFIG --prefix`"
    pcre_dll="$pcre_prefix/bin/libpcre-0.dll"
    pcrecpp_dll="$pcre_prefix/bin/libpcrecpp-0.dll"

    iconv_dll="$ICONV_DIST/bin/libiconv-2.dll"

    version=`awk '/PACKAGE_VERSION = / {print $3}' Makefile`
    archive="cclive-$version-win32-i686-bin.7z"
    distdir="cclive-$version"

    rm -rf dist cclive-$version $archive \
    && make install-strip \
    && make man \
    && cp $quvi_dll dist/bin \
    && cp -r $quvi_lua dist/bin \
    && cp $lua_dll dist/bin \
    && cp $curl_dll dist/bin \
    && cp $pcre_dll dist/bin \
    && cp $pcrecpp_dll dist/bin \
    && cp $iconv_dll dist/bin \
    && mkdir -p dist/licenses/ \
    && cp $QUVI_PREFIX/../../COPYING dist/licenses/libquvi-COPYING.TXT \
    && cp $QUVI_PREFIX/licenses/liblua-COPYRIGHT.TXT dist/licenses/ \
    && cp $CURL_PREFIX/COPYING dist/licenses/libcurl-COPYING.TXT \
    && cp $PCRE_PREFIX/LICENCE dist/licenses/libpcre-LICENSE.TXT \
    && cp $ICONV_PREFIX/COPYING.LIB dist/licenses/libiconv-COPYING.TXT \
    && cp ../COPYING dist/licenses/cclive-COPYING.TXT \
    && cp ../ChangeLog dist/ChangeLog.TXT \
    && cp ChangeLog.w32.TXT dist/ChangeLog.w32.TXT \
    && cp README.w32.TXT dist/ \
    && cp cclive.1.html dist/ \
    && rm -rf dist/share \
    && mv dist $distdir \
    && 7za a $archive $distdir
    exit $?
}

clean_up() {
    make distclean 2>/dev/null
    rm -rf src tests cclive.1
    exit $?
}

pack_flag=off
clean_flag=off
while [ $# -gt 0 ]
do
    case "$1" in
        -p) pack_flag=on;;
        -c) clean_flag=on;;
         *) break;;
    esac
    shift
done

if [ x"$pack_flag" = "xon" ]; then
    pack_it
fi

if [ x"$clean_flag" = "xon" ]; then
    clean_up
fi

# No tweaking usually required.

export libquvi_CFLAGS="-I$QUVI_PREFIX/include"
export libquvi_LIBS="-L$QUVI_PREFIX/lib -lquvi"

export libcurl_CFLAGS="`$CURL_CONFIG --cflags`"
export libcurl_LIBS="`$CURL_CONFIG --libs`"

export libpcrecpp_CFLAGS="`$PCRE_CONFIG --cflags`"
export libpcrecpp_LIBS="`$PCRE_CONFIG --libs` -lpcrecpp"

CXXFLAGS=$CXXFLAGS ../configure \
    --prefix="$PREFIX" \
    --host="$HOST" \
     --without-man


