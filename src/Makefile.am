src = \
main.cpp \
app.cpp	\
except.cpp \
util.cpp \
opts.cpp \
quvi.cpp \
curl.cpp \
progressbar.cpp \
exec.cpp \
retry.cpp \
log.cpp	\
cmdline.cpp

hdr = \
app.h \
except.h \
util.h \
opts.h \
singleton.h \
macros.h \
quvi.h \
curl.h \
progressbar.h \
exec.h \
retry.h \
error.h \
log.h \
cmdline.h

bin_PROGRAMS = cclive
cclive_SOURCES = $(src) $(hdr)

AM_CPPFLAGS = \
$(libquvi_CFLAGS) \
$(libcurl_CFLAGS) \
$(libpcrecpp_CFLAGS)

LIBS = \
$(libquvi_LIBS) \
$(libcurl_LIBS) \
$(LIBICONV) \
$(LTLIBICONV) \
$(libpcrecpp_LIBS)


