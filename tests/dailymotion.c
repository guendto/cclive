#include "test.h"

#include <stdio.h>
#include <stdlib.h>

#define TEST_URL \
    "http://www.dailymotion.com/hd/video/" \
        "x9fkzj_battlefield-1943-coral-sea-trailer_videogames"

static const char *formats[] = {
    "flv",
    "spark-mini",
    "vp6-hq",
    "vp6-hd",
    "vp6",
    "h264",
    NULL
};

int
main (int argc, char *argv[]) {
    register int i,rc;
    char *opts;

    for (i=0,rc=0,opts=0; formats[i] && !rc; ++i) {
        asprintf(&opts, "-f %s", formats[i]);
        rc = runtest_host(opts, TEST_URL);
        free(opts);
    }

    return(rc);
}
