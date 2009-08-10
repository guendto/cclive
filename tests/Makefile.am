
EXTRA_DIST=

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
myubo\
cctv\
ehrensenf\
spiegel\
vimeo

TESTS= $(check_PROGRAMS)

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
funnyhub_SOURCES=	funnyhub.c $(common)
myubo_SOURCES=		myubo.c $(common)
cctv_SOURCES=		cctv.c $(common)
ehrensenf_SOURCES=	ehrensenf.c $(common)
spiegel_SOURCES=	spiegel.c $(common)
