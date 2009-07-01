check_PROGRAMS=	\
youtube			\
break			\
liveleak		\
dailymotion		\
evisor			\
google			\
sevenload		\
vimeo

TESTS= $(check_PROGRAMS)

if WITH_PERL
TESTS+= pod.pl
endif

common= test.c test.h

youtube_SOURCES=	youtube.c $(common)
break_SOURCES=		break.c $(common)
liveleak_SOURCES=	liveleak.c $(common)
dailymotion_SOURCES=	dailymotion.c $(common)
evisor_SOURCES=		evisor.c $(common)
google_SOURCES=		google.c $(common)
vimeo_SOURCES=		vimeo.c $(common)
sevenload_SOURCES=	sevenload.c $(common)
