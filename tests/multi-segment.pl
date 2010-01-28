if ($ENV{NO_MULTISEGMENT}) {
    print STDERR "SKIP: No multi-segment.\n";
    exit 0;
}
exit ccliveTest::run(
    "-n",
    "http://space.tv.cctv.com/video/VIDE1247468077860061",
    "--remote" # implies internet requirement
);
