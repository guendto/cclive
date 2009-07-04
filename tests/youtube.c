#include <stdio.h>
#include <stdlib.h>

#include "test.h"

static const char *
formats[] = {
    "flv",
    "fmt17",
    "fmt18",
    "fmt22",
    "fmt35",
    NULL
};

static const char *
urls[] = {
    "http://www.youtube.com/watch?v=d0j_ONmVcXA",
    "http://www.youtube.com/v/d0j_ONmVcXA",
    "http://www.youtube-nocookie.com/v/3PuHGKnboNY",
    "http://www.last.fm/music/Rammstein/+videos/+1-3jwXQFFLSHo",
    NULL
};

int
main (int argc, char *argv[]) {
    register int i, rc;
    char *opts;

    for (i=0,rc=0,opts=0; formats[i] && !rc; ++i) {
        asprintf(&opts, "--format=%s", formats[i]);
        rc = runtest_host(opts, urls[0]);
        free(opts);
    }

    for (i=0,opts=0; urls[i] && !rc; ++i)
        rc = runtest_host(NULL, urls[i]);

    return(rc);
}

