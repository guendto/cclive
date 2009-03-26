src = main.c cmdline.c mem.c host.c dl.c llst.c \
      util.c progress.c login.c strlcat.c strlcpy.c \
      cclive.h cmdline.h llst.h progress.h

INCLUDES = $(libcurl_CFLAGS)
LIBS = $(libcurl_LIBS)

if WITH_PERL
INCLUDES += $(perl_CCOPTS)
LIBS += $(perl_LDOPTS)
src += title.c perl_xsinit.c
endif

bin_PROGRAMS = cclive
cclive_SOURCES = $(src)
if WITH_MAN
dist_man_MANS = cclive.1
endif

.PHONY: cmdline man
cmdline:
if HAVE_GENGETOPT
	@GENGETOPT@ < cmdline.ggo -C --unamed-opts=URL --no-version
else
	@echo "error: configure did not find 'gengetopt' command"
endif
man:
if HAVE_POD2MAN
	@POD2MAN@ -c "cclive manual" -n cclive \
		 -s 1 -r @VERSION@ cclive.pod cclive.1
else
	@echo "error: configure did not find 'pod2man' command'
endif
