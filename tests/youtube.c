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
};

int
main (int argc, char *argv[]) {
    register int i, rc;
    char *cmd;

    for (i=0,rc=0,cmd=0; formats[i] && !rc; ++i) {
        asprintf(&cmd, "../src/cclive -nf %s", formats[i]);
        rc = runtest_host(cmd, urls[0]);
        free(cmd);
    }

    for (i=0,cmd=0; i<urls[i] && !rc; ++i) {
        asprintf(&cmd, "../src/cclive -n %s", urls[i]);
        rc = runtest_host(cmd, urls[i]);
        free(cmd);
    }

    return(rc);
}

