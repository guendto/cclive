# GNU Makefile for cclive.

SHELL = /bin/sh

.SUFFIXES:
.SUFFIXES: .c .o

prefix      = $(HOME)
bindir      = $(prefix)/bin
exec_prefix = $(prefix)
datarootdir = $(prefix)/share
datadir     = $(datarootdir)
mandir      = $(datarootdir)/man
man1dir     = $(mandir)/man1

CC          = cc
RM          = rm -f
INSTALL     = install -c
INSTALL_D   = install -d
INSTALL_M   = install
UNAME       = uname
AWK         = awk
CURL_CONFIG = curl-config
GENGETOPT   = gengetopt
POD2MAN     = pod2man
PERL        = perl

WITH_MAN        = yes
WITH_SIGWINCH   = yes
WITH_PERL       = no

ifndef V
QUIET_CC        = @echo '  ' CC $@;
QUIET_LINK      = @echo '  ' LINK $@;
QUIET_CMDLINE   = @echo '  ' GENGETOPT cmdline.c cmdline.h;
QUIET_POD2MAN   = @echo '  ' POD2MAN cclive.1;
endif

OS_NAME     := $(shell sh -c "$(UNAME) -s 2>/dev/null || echo -")
RELEASE     := $(shell sh -c "$(AWK) '/VERSION / {print \$$3}' cmdline.h")

CURL_CFLAGS  := $(shell sh -c "$(CURL_CONFIG) --cflags")
CURL_LDFLAGS := $(shell sh -c "$(CURL_CONFIG) --libs")

CFLAGS      = -g -Wall
ALL_CFLAGS  = -D_GNU_SOURCE -DOSNAME=\"$(OS_NAME)\" -I. $(CFLAGS)
ALL_CFLAGS += $(CURL_CFLAGS)

LDFLAGS     =
ALL_LDFLAGS = $(LDFLAGS)
ALL_LDFLAGS += $(CURL_LDFLAGS)

SRCS        = main.c cmdline.c mem.c host.c dl.c llst.c \
                util.c progress.c login.c strlcat.c strlcpy.c

ifeq ($(WITH_SIGWINCH),yes)
ALL_CFLAGS += -DWITH_SIGWINCH
endif

ifeq ($(WITH_PERL),yes)
PERL_CFLAGS  := $(shell sh -c "$(PERL) -MExtUtils::Embed -e ccopts")
PERL_LDFLAGS := $(shell sh -c "$(PERL) -MExtUtils::Embed -e ldopts")
ALL_CFLAGS   += -DWITH_PERL
ALL_CFLAGS   += $(PERL_CFLAGS)
ALL_LDFLAGS  += $(PERL_LDFLAGS)
SRCS         += title.c perl_xsinit.c
endif

PROG        = cclive
OBJS        = $(SRCS:%.c=%.o)

.PHONY: all
.SECONDARY: $(PROG) $(OBJS)

all: $(PROG)

$(PROG): $(OBJS)
	$(QUIET_LINK)$(CC) $(ALL_LDFLAGS) $(OBJS) -o $@

%.o: %.c
	$(QUIET_CC)$(CC) -c $(ALL_CFLAGS) $< -o $@

.PHONY: install install-strip
install: all
	$(INSTALL_D) $(DESTDIR)$(bindir)
	$(INSTALL) $(PROG) $(DESTDIR)$(bindir)/$(PROG)
ifeq ($(WITH_MAN),yes)
	$(INSTALL_D) $(DESTDIR)$(man1dir)
	$(INSTALL_M) -m 444 cclive.1 $(DESTDIR)$(man1dir)/cclive.1
endif

install-strip:
	$(MAKE) INSTALL='$(INSTALL) -s' install

.PHONY: cmdline man
cmdline:
	$(QUIET_CMDLINE)$(GENGETOPT) < cmdline.ggo -C --unamed-opts=URL \
        --no-version

man:
	$(QUIET_POD2MAN)$(POD2MAN) -c "cclive manual" -n cclive \
		-s 1 -r $(RELEASE) cclive.pod cclive.1

.PHONY: uninstall clean
uninstall:
	$(RM) $(DESTDIR)$(bindir)/$(PROG)
ifeq ($(WITH_MAN),yes)
	$(RM) $(DESTDIR)$(man1dir)/cclive.1
endif

clean:
	@$(RM) $(PROG) $(OBJS) cclive.core 2>/dev/null

