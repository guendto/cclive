
src=\
  cc/application.cpp\
  cc/background.cpp\
  cc/curl.cpp\
  cc/error.cpp\
  cc/exec.cpp\
  cc/file.cpp\
  cc/get.cpp\
  cc/input.cpp\
  cc/log.cpp\
  cc/main.cpp\
  cc/progressbar.cpp\
  cc/re.cpp\
  cc/wait.cpp

if HAVE_LIBQUVI_0_9
src+=\
  compat/error_pt9.cpp\
  compat/media_pt9.cpp\
  compat/query_pt9.cpp
else
src+=\
  compat/error_pt4.cpp\
  compat/media_pt4.cpp\
  compat/query_pt4.cpp
endif
src+=\
  compat/util.cpp

hdr=\
  ccapplication\
  ccerror\
  ccfile\
  ccfstream\
  ccinput\
  ccinternal\
  cclog\
  ccoptions\
  ccprogressbar\
  ccquvi\
  ccre\
  ccutil

hdr+=\
  cc/application.h\
  cc/error.h\
  cc/file.h\
  cc/fstream.h\
  cc/input.h\
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
AM_CPPFLAGS+= $(glibmm_CFLAGS)
AM_CPPFLAGS+= $(glib_CFLAGS)
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
cclive_LDADD+=	$(glibmm_LIBS)
cclive_LDADD+=	$(glib_LIBS)
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
