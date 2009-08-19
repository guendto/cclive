#include "test.h"

#define TEST_URL \
    "http://www.dailymotion.com/hd/video/" \
        "x9fkzj_battlefield-1943-coral-sea-trailer_videogames"

static const char *
formats[] = {
    "flv",
    "spark-mini",
    "vp6-hq",
    "vp6-hd",
    "vp6",
    "h264",
    "best",
    NULL
};

MAIN_BEGIN
    int i,rc;
    for (i=0,rc=0; formats[i] && !rc; ++i) {
        std::stringstream b;
        b << "-f " << formats[i];
        rc = runtest_host( TEST_URL, b.str() );
    }
    return(rc);
MAIN_END


