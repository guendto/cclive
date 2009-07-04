#include <stdio.h>

#include "test.h"
    
static const char *urls[] = {
"http://en.sevenload.com/videos/IUL3gda-Funny-Football-Clips",
"http://de.sevenload.com/videos/GN4Hati/EM08-Spanien-Russland-4-1-Highlights",
"http://en.sevenload.com/shows/TheSailingChannel-TV/episodes/zLM5OvT-Cruising-with-Bettie-Trailer",
"http://de.sevenload.com/sendungen/halbzeit-in/folgen/Kbv3CsN-Wechselgesang-Sieger-beste-Bewertungen",
NULL
};

int
main (int argc, char *argv[]) {
    register int i,rc;

    for (i=0,rc=0; urls[i] && !rc; ++i)
        rc = runtest_host("../src/cclive -n", urls[i]);

    return(rc);
}

