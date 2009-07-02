#include <stdio.h>
#include <stdlib.h>

#include "test.h"

#define TEST_URL \
    "http://video.google.com/videoplay?docid=-8669127848070159803"

#define TEST_EMBED_URL \
    "http://video.google.com/googleplayer.swf?docid=-8669127848070159803"

static const char *formats[] = {
    "flv",
    "mp4",
    NULL
};

int
main (int argc, char *argv[]) {
    register int i,rc=0;
    char *cmd;
    for (i=0; formats[i]; ++i) {
        asprintf(&cmd, "../src/cclive -nf %s", formats[i]);
        rc = runtest_host(cmd, TEST_URL);
        free(cmd);
        if (rc != 0)
            break;
    }

    if (rc == 0)
        rc = runtest_host("../src/cclive -n", TEST_EMBED_URL);

    return (rc);
}
