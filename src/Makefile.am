src=\
main.cpp\
app.cpp	\
except.cpp\
util.cpp\
hosthandler.cpp\
opts.cpp\
video.cpp\
curl.cpp\
progressbar.cpp\
exec.cpp\
log.cpp	\
cmdline.c\
hostfactory.cpp

host=\
youtube.cpp\
break.cpp\
evisor.cpp\
sevenload.cpp\
liveleak.cpp\
dailymotion.cpp\
vimeo.cpp\
golem.cpp\
clipfish.cpp\
funnyhub.cpp\
myubo.cpp\
google.cpp

hdr=\
app.h\
except.h\
util.h\
opts.h\
hosthandler.h\
video.h\
singleton.h\
macros.h\
curl.h\
progressbar.h\
exec.h\
error.h\
log.h\
cmdline.h\
hostfactory.h

INCLUDES=$(libcurl_CFLAGS)
LIBS=$(libcurl_LIBS) $(LIBICONV) $(LTLIBICONV)

bin_PROGRAMS=cclive
cclive_SOURCES=$(src) $(host) $(hdr)
