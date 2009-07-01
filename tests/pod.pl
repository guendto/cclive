#!/usr/bin/env perl

use warnings;
use strict;

use Test::More tests => 1;

SKIP: {
    eval "use Pod::Checker";
    skip "Pod::Checker required for testing Pod" if $@;

    my $r = podchecker("../cclive.pod");
    ok( $r == 0 )
        or diag explain $r;
}
