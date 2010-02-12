my $rc = ccliveTest::run("-f", "invalid");
$rc = 0 if $rc == 0x2; # CCLIVE_OPTARG (0x2) is what we expect.
exit $rc;
