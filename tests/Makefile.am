
EXTRA_DIST=

check_PROGRAMS=\
returncode\
multi

TESTS=		$(check_PROGRAMS)
common=		test.cpp test.h
AM_CXXFLAGS=	-DCCLIVE_PATH="\"$(top_builddir)/src/cclive\""

returncode_SOURCES=	returncode.cpp $(common)
multi_SOURCES=		multi.cpp $(common)

local-clean:
	@rm -rf *.core

distclean-local: local-clean
