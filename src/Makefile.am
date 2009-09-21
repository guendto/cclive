src = \
main.cpp \
app.cpp	\
except.cpp \
util.cpp \
hosthandler.cpp \
opts.cpp \
video.cpp \
curl.cpp \
progressbar.cpp \
exec.cpp \
retry.cpp \
log.cpp	\
cmdline.cpp \
hostfactory.cpp

host = \
youtube.cpp \
break.cpp \
evisor.cpp \
sevenload.cpp \
liveleak.cpp \
dailymotion.cpp \
vimeo.cpp \
golem.cpp \
clipfish.cpp \
funnyhub.cpp \
myubo.cpp \
cctv.cpp \
ehrensenf.cpp \
spiegel.cpp \
redtube.cpp \
google.cpp

hdr = \
app.h \
except.h \
util.h \
opts.h \
hosthandler.h \
video.h \
singleton.h \
macros.h \
curl.h \
progressbar.h \
exec.h \
retry.h \
error.h \
log.h \
cmdline.h \
hostfactory.h

bin_PROGRAMS = cclive
cclive_SOURCES = $(src) $(host) $(hdr)

cclive_CXXFLAGS = \
$(libcurl_CFLAGS) \
$(libpcrecpp_CFLAGS)

cclive_LDFLAGS = \
$(libcurl_LIBS) \
$(LIBICONV) \
$(LTLIBICONV) \
$(libpcrecpp_LIBS)


