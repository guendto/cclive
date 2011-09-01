
ACLOCAL_AMFLAGS=-I m4
EXTRA_DIST=	scripts/astyle_cpp.sh
SUBDIRS=	src doc

.PHONY: ChangeLog

ChangeLog:
	@if test -d "$(top_srcdir)/.git" -o -f "$(top_srcdir)/.git"; then \
		git log --stat --name-only --date=short --abbrev-commit \
			--after="Thu Aug 12 15:54:58 2010" > $@; \
	fi

dist-hook: ChangeLog
