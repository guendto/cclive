my $rc = ccliveTest::run("http://invalid.host");
$rc = 0 if $rc == 0x6; # CCLIVE_NOSUPPORT (0x6) is what we expect.
exit $rc;
