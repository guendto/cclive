#include "test.h"
    
static const char *urls[] = {
"http://en.sevenload.com/videos/IUL3gda-Funny-Football-Clips",
"http://de.sevenload.com/videos/GN4Hati/EM08-Spanien-Russland-4-1-Highlights",
"http://en.sevenload.com/shows/TheSailingChannel-TV/episodes/"
    "zLM5OvT-Cruising-with-Bettie-Trailer",
NULL
};

MAIN_BEGIN
    int i,rc;
    for (i=0,rc=0; urls[i] && !rc; ++i)
        rc = runtest_host(urls[i]);
    return(rc);
MAIN_END


