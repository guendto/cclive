
EXTRA_DIST= pod.pl

check_PROGRAMS=\
youtube\
break\
liveleak\
dailymotion\
evisor\
google\
sevenload\
golem\
clipfish\
funnyhub\
vimeo

TESTS= $(check_PROGRAMS)

if WITH_PERL
TESTS+= pod.pl
endif

common= test.c test.h

AM_CFLAGS= -DCCLIVE_PATH="\"$(top_srcdir)/src/cclive\""

youtube_SOURCES=	youtube.c $(common)
break_SOURCES=		break.c $(common)
liveleak_SOURCES=	liveleak.c $(common)
dailymotion_SOURCES=	dailymotion.c $(common)
evisor_SOURCES=		evisor.c $(common)
google_SOURCES=		google.c $(common)
vimeo_SOURCES=		vimeo.c $(common)
sevenload_SOURCES=	sevenload.c $(common)
golem_SOURCES=		golem.c $(common)
clipfish_SOURCES=	clipfish.c $(common)
funnyhub_SOURCES=  funnyhub.c $(common)
