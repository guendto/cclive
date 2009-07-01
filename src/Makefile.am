src =			\
	main.cpp	\
	app.cpp		\
	except.cpp	\
	util.cpp	\
	hosthandler.cpp	\
	opts.cpp	\
	video.cpp	\
	curl.cpp	\
	progressbar.cpp	\
	exec.cpp	\
	log.cpp		\
	cmdline.c	\
	hostfactory.cpp

if WITH_PERL
src += xsinit.c
endif

host =			\
	youtube.cpp	\
	break.cpp	\
	evisor.cpp	\
	sevenload.cpp	\
	liveleak.cpp	\
	dailymotion.cpp	\
	vimeo.cpp	\
	google.cpp

hdr =			\
	app.h		\
	except.h	\
	util.h		\
	opts.h		\
	hosthandler.h	\
	video.h		\
	singleton.h	\
	macros.h	\
	curl.h		\
	progressbar.h	\
	exec.h		\
	error.h		\
	log.h		\
	cmdline.h	\
	hostfactory.h

INCLUDES	=$(libcurl_CFLAGS)
LIBS 		=$(libcurl_LIBS)

if WITH_PERL
INCLUDES	+=$(perl_CCOPTS)
LIBS		+=$(perl_LDOPTS)
endif

bin_PROGRAMS		=cclive
cclive_SOURCES 		=$(src) $(host) $(hdr)
