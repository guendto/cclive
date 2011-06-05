
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc

EXTRA_DIST = scripts/astyle_cpp.sh

ChangeLog:
	@if test -f $(top_srcdir)/.git/HEAD; then \
		@GIT@ log --stat --after="Thu Aug 12 15:54:58 2010" > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog
