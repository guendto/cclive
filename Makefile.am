SUBDIRS= src
if ENABLE_TESTS
SUBDIRS+= tests
endif

EXTRA_DIST=\
cmdline.ggo\
cclive.pod\
examples/config\
docs/AddingSupport\
docs/SubmittingPatches

if WITH_MAN
dist_man_MANS= cclive.1
endif

.PHONY: cmdline man checkpod
cmdline:
if HAVE_GENGETOPT
	@GENGETOPT@ < cmdline.ggo -C --unamed-opts=URL \
		--output-dir ./src --no-version -c cpp
else
	@echo "error: configure did not find 'gengetopt' command"
endif
man:
if HAVE_POD2MAN
	@POD2MAN@ -c "cclive manual" -n cclive \
		 -s 1 -r @VERSION@ cclive.pod cclive.1
else
	@echo "error: configure did not find 'pod2man' command"
endif
if HAVE_POD2HTML
	@POD2HTML@ --title="cclive manual" \
		--infile=cclive.pod --outfile=cclive.1.html
else
	@echo "error: configure did not find 'pod2html' command"
endif
checkpod:
if HAVE_PODCHECKER
	@PODCHECKER@ cclive.pod
else
	@echo "error: configure did not find 'podchecker' command"
endif

local-clean:
	@rm -rf *.tmp *.core *.html

distclean-local: local-clean
