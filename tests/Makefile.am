
EXTRA_DIST = ccliveTest.pm.in $(TESTS)

if HAVE_PERL

TESTS_ENVIRONMENT = $(PERL) -w -mstrict -mccliveTest -I $(top_builddir)/tests

TESTS =\
nosupport-eval.pl\
multi-dl.pl

else # HAVE_PERL
@echo perl not found.
endif # HAVE_PERL


