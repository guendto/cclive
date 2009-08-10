#include "test.h"

#include <stdio.h>
#include <stdlib.h>

static const char *
formats[] = {
    "flv",
    "vp6_64",
    "vp6_576",
    "vp6_928",
    "h264_1400",
    NULL
};

#define TEST_URL \
    "http://www.spiegel.de/video/video-1012584.html"

int
main (int argc, char *argv[]) {
    register int i, rc;
    char *opts;

    for (i=0,rc=0,opts=0; formats[i] && !rc; ++i) {
        asprintf(&opts, "-f %s", formats[i]);
        rc = runtest_host(opts, TEST_URL);
        free(opts);
    }

    return(rc);
}

