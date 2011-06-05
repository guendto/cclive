
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

cclive_CPPLAGS= \
    -I$(top_srcdir)/src \
    $(libquvi_CFLAGS) \
    $(libcurl_CFLAGS) \
    $(libpcre_CFLAGS) \
    $(libpcrecpp_CFLAGS) \
    $(BOOST_CPPFLAGS) \
    $(AM_CPPFLAGS)

cclive_LDFLAGS= \
    $(BOOST_DATE_TIME_LDFLAGS) \
    $(BOOST_PROGRAM_OPTIONS_LDFLAGS) \
    $(BOOST_IOSTREAMS_LDFLAGS) \
    $(BOOST_FILESYSTEM_LDFLAGS) \
    $(BOOST_SYSTEM_LDFLAGS) \
    $(BOOST_THREAD_LDFLAGS) \
    $(AM_LDFLAGS)

cclive_LDADD = \
    $(libquvi_LIBS) \
    $(libcurl_LIBS) \
    $(libpcre_LIBS) \
    $(libpcrecpp_LIBS) \
    $(BOOST_DATE_TIME_LIBS) \
    $(BOOST_PROGRAM_OPTIONS_LIBS) \
    $(BOOST_IOSTREAMS_LIBS) \
    $(BOOST_FILESYSTEM_LIBS) \
    $(BOOST_SYSTEM_LIBS) \
    $(BOOST_THREAD_LIBS)

