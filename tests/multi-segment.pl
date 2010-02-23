# We will need utf8 for this.
ccliveTest::check_codeset();

unless ($ENV{TEST_MULTISEGMENT}) {
    print STDERR "SKIP: No multi-segment. Set TEST_MULTISEGMENT to enable.\n";
    exit 0;
}
exit ccliveTest::run(
    "-n",
    "http://space.tv.cctv.com/video/VIDE1247468077860061",
    "--remote" # implies internet requirement
);
