unless ($ENV{TEST_DOWNLOAD}) {
    print STDERR "SKIP: No download. Set TEST_DOWNLOAD to enable.\n";
    exit 0;
}
exit ccliveTest::run(
    "http://www.youtube.com/watch?v=DeWsZ2b_pK4",
    "http://break.com/index/beach-tackle-whip-lash.html",
    "--remote", # implies internet requirement
    "-f",
    "mobile",
    "-O",
    "/dev/null"
);
