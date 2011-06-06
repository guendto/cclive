
src= \
cclive/error.cpp \
cclive/main.cpp \
cclive/log.cpp \
cclive/background.cpp \
cclive/exec.cpp \
cclive/file.cpp \
cclive/license.cpp \
cclive/wait.cpp \
cclive/progressbar.cpp \
cclive/options.cpp \
cclive/re.cpp \
cclive/get.cpp \
cclive/application.cpp \
quvicpp/err.cpp \
quvicpp/query.cpp \
quvicpp/misc.cpp \
quvicpp/url.cpp \
quvicpp/opts.cpp \
quvicpp/media.cpp

hdr= \
cclive/internal.h \
cclive/progressbar.h \
cclive/wait.h \
cclive/options.h \
cclive/application.h \
cclive/file.h \
cclive/error.h \
cclive/log.h \
cclive/re.h \
cclive/get.h \
cclive/background.h \
cclive/exec.h \
quvicpp/quvicpp.h

bin_PROGRAMS=   cclive
cclive_SOURCES= $(src) $(hdr)

AM_CPPFLAGS= -I$(top_srcdir)/src
AM_CPPFLAGS+= $(libquvi_CFLAGS)
AM_CPPFLAGS+= $(libcurl_CFLAGS)
AM_CPPFLAGS+= $(libpcre_CFLAGS)
AM_CPPFLAGS+= $(libpcrepp_CFLAGS)
AM_CPPFLAGS+= $(BOOST_CPPFLAGS)

cclive_LDFLAGS= $(BOOST_DATE_TIME_LDFLAGS)
cclive_LDFLAGS+= $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
cclive_LDFLAGS+= $(BOOST_IOSTREAMS_LDFLAGS)
cclive_LDFLAGS+= $(BOOST_FILESYSTEM_LDFLAGS)
cclive_LDFLAGS+= $(BOOST_SYSTEM_LDFLAGS)

cclive_LDADD= $(libquvi_LIBS)
cclive_LDADD+= $(libcurl_LIBS)
cclive_LDADD+= $(libpcre_LIBS)
cclive_LDADD+= $(libpcrecpp_LIBS)
cclive_LDADD+= $(BOOST_DATE_TIME_LIBS)
cclive_LDADD+= $(BOOST_PROGRAM_OPTIONS_LIBS)
cclive_LDADD+= $(BOOST_IOSTREAMS_LIBS)
cclive_LDADD+= $(BOOST_FILESYSTEM_LIBS)
cclive_LDADD+= $(BOOST_SYSTEM_LIBS)
