#include <stdio.h>
#include <stdlib.h>

#include "test.h"

#define TEST_URL \
    "http://video.google.com/videoplay?docid=-8669127848070159803"

#define TEST_EMBED_URL \
    "http://video.google.com/googleplayer.swf?docid=-8669127848070159803"

static const char
*formats[] = {
    "flv",
    "mp4",
    NULL
};

int
main (int argc, char *argv[]) {
    register int i,rc;
    char *opts;

    for (i=0,rc=0,opts=0; formats[i] && !rc; ++i) {
        asprintf(&opts, "-nf %s", formats[i]);
        rc = runtest_host(opts, TEST_URL);
        free(opts);
    }

    if (rc == 0)
        rc = runtest_host("-n", TEST_EMBED_URL);

    return (rc);
}
