
src= \
 cc/error.cpp \
 cc/main.cpp \
 cc/log.cpp \
 cc/background.cpp \
 cc/exec.cpp \
 cc/file.cpp \
 cc/license.cpp \
 cc/wait.cpp \
 cc/progressbar.cpp \
 cc/options.cpp \
 cc/re.cpp \
 cc/get.cpp \
 cc/application.cpp \
 cc/curl.cpp \
 quvi/err.cpp \
 quvi/query.cpp \
 quvi/misc.cpp \
 quvi/url.cpp \
 quvi/opts.cpp \
 quvi/media.cpp

hdr= \
 ccoptions \
 ccinternal \
 ccapplication \
 ccquvi \
 ccprogressbar \
 cclog \
 ccutil \
 ccfile \
 ccre \
 cc/progressbar.h \
 cc/options.h \
 cc/application.h \
 cc/file.h \
 cc/util.h \
 cc/internal.h \
 cc/log.h \
 cc/re.h \
 quvi/ccquvi.h

bin_PROGRAMS=	cclive
cclive_SOURCES=	$(src) $(hdr)

AM_CPPFLAGS=	-I$(top_srcdir)/src
AM_CPPFLAGS+=	$(libquvi_CFLAGS)
AM_CPPFLAGS+=	$(libcurl_CFLAGS)
AM_CPPFLAGS+=	$(libpcre_CFLAGS)
AM_CPPFLAGS+=	$(libpcrepp_CFLAGS)
AM_CPPFLAGS+=	$(BOOST_CPPFLAGS)

cclive_LDFLAGS=	$(BOOST_DATE_TIME_LDFLAGS)
cclive_LDFLAGS+=$(BOOST_PROGRAM_OPTIONS_LDFLAGS)
cclive_LDFLAGS+=$(BOOST_IOSTREAMS_LDFLAGS)
cclive_LDFLAGS+=$(BOOST_FILESYSTEM_LDFLAGS)
cclive_LDFLAGS+=$(BOOST_SYSTEM_LDFLAGS)

cclive_LDADD=	$(libquvi_LIBS)
cclive_LDADD+=	$(libcurl_LIBS)
cclive_LDADD+=	$(libpcre_LIBS)
cclive_LDADD+=	$(libpcrecpp_LIBS)
cclive_LDADD+=	$(BOOST_DATE_TIME_LIBS)
cclive_LDADD+=	$(BOOST_PROGRAM_OPTIONS_LIBS)
cclive_LDADD+=	$(BOOST_IOSTREAMS_LIBS)
cclive_LDADD+=	$(BOOST_FILESYSTEM_LIBS)
cclive_LDADD+=	$(BOOST_SYSTEM_LIBS)

install-exec-hook:
if ENABLE_CCL
	cd $(DESTDIR)$(bindir) && rm -f ccl$(EXEEXT) && \
		$(LN_S) cclive$(EXEEXT) ccl$(EXEEXT)
endif

uninstall-hook:
if ENABLE_CCL
	cd $(DESTDIR)$(bindir) && \
		rm -f ccl$(EXEEXT)
endif
