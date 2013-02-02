
src=\
  cc/application.cpp\
  cc/background.cpp\
  cc/curl.cpp\
  cc/error.cpp\
  cc/exec.cpp\
  cc/file.cpp\
  cc/get.cpp\
  cc/license.cpp\
  cc/log.cpp\
  cc/main.cpp\
  cc/options.cpp\
  cc/progressbar.cpp\
  cc/re.cpp\
  cc/wait.cpp

src+=\
  compat/error_pt4.cpp\
  compat/media_pt4.cpp\
  compat/query_pt4.cpp\
  compat/util.cpp

hdr=\
  ccapplication\
  ccfile\
  ccinternal\
  cclog\
  ccoptions\
  ccprogressbar\
  ccquvi\
  ccre\
  ccutil

hdr+=\
  cc/application.h\
  cc/file.h\
  cc/internal.h\
  cc/log.h\
  cc/options.h\
  cc/progressbar.h\
  cc/re.h\
  cc/util.h

hdr+=\
  compat/error.h\
  compat/media.h\
  compat/options.h\
  compat/query.h\
  compat/util.h

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
