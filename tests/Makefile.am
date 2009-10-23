
EXTRA_DIST=

check_PROGRAMS=\
returncode\
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
vimeo\
youjizz\
multi
#redtube

TESTS=		$(check_PROGRAMS)
common=		test.cpp test.h
AM_CXXFLAGS=	-DCCLIVE_PATH="\"$(top_builddir)/src/cclive\""

returncode_SOURCES=	returncode.cpp $(common)
youtube_SOURCES=	youtube.cpp $(common)
break_SOURCES=		break.cpp $(common)
liveleak_SOURCES=	liveleak.cpp $(common)
dailymotion_SOURCES=	dailymotion.cpp $(common)
evisor_SOURCES=		evisor.cpp $(common)
google_SOURCES=		google.cpp $(common)
vimeo_SOURCES=		vimeo.cpp $(common)
sevenload_SOURCES=	sevenload.cpp $(common)
golem_SOURCES=		golem.cpp $(common)
clipfish_SOURCES=	clipfish.cpp $(common)
funnyhub_SOURCES=	funnyhub.cpp $(common)
myubo_SOURCES=		myubo.cpp $(common)
cctv_SOURCES=		cctv.cpp $(common)
ehrensenf_SOURCES=	ehrensenf.cpp $(common)
spiegel_SOURCES=	spiegel.cpp $(common)
#redtube_SOURCES=	redtube.cpp $(common)
youjizz_SOURCES=	youjizz.cpp $(common)
multi_SOURCES=		multi.cpp $(common)

local-clean:
	@rm -rf *.core

distclean-local: local-clean
