
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc

EXTRA_DIST = scripts/astyle_cpp.sh

ChangeLog:
	@if test -f $(top_srcdir)/.git/HEAD; then \
		@GIT@ log --stat --after="Sun Nov 14 11:03:50 2010" > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog
