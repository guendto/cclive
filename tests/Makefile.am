
EXTRA_DIST = ccliveTest.pm.in $(TESTS) $(TESTS_POD)

if HAVE_PERL

TESTS_ENVIRONMENT = $(PERL) -w -mstrict -mccliveTest -I $(top_builddir)/tests

TESTS =\
noformat-eval.pl\
nosupport-eval.pl\
multi-segment.pl\
multi-dl.pl

TESTS_POD =\
pod-check.pl

if HAVE_PERL_POD_CHECKER
TESTS += $(TESTS_POD)
endif

else # HAVE_PERL
	@echo SKIP: perl not found.
endif # HAVE_PERL


