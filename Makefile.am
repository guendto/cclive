SUBDIRS = src tests
EXTRA_DIST = cmdline.ggo cclive.pod HACKING TODO

if WITH_MAN
dist_man_MANS = cclive.1
endif

.PHONY: cmdline man
cmdline:
if HAVE_GENGETOPT
	@GENGETOPT@ < cmdline.ggo -C --unamed-opts=URL \
		--output-dir ./src --no-version
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
