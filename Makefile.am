SUBDIRS= src tests

EXTRA_DIST=\
cmdline.ggo\
cclive.pod\
examples/config\
docs/AddingSupport\
docs/SubmittingPatches

if WITH_MAN
dist_man_MANS= cclive.1
endif

.PHONY: cmdline man
cmdline:
if HAVE_GENGETOPT
	@GENGETOPT@ < $(top_srcdir)/cmdline.ggo -C --unamed-opts=URL \
		--output-dir $(top_srcdir)/src --no-version -c cpp
else
	@echo "error: configure did not find 'gengetopt' command"
endif
man:
if HAVE_POD2MAN
	@POD2MAN@ -c "cclive manual" -n cclive \
		 -s 1 -r @VERSION@ $(top_srcdir)/cclive.pod $(top_builddir)/cclive.1
else
	@echo "error: configure did not find 'pod2man' command"
endif
if HAVE_POD2HTML
	@POD2HTML@ --title="cclive manual" \
		--infile=$(top_srcdir)/cclive.pod \
        --outfile=$(top_builddir)/cclive.1.html
else
	@echo "error: configure did not find 'pod2html' command"
endif

local-clean:
	@rm -rf *.tmp *.core *.html

distclean-local: local-clean
