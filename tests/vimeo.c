#include "test.h"

#include <stdio.h>
#include <stdlib.h>

static const char *formats[] = {
    "flv",
    "hd",
    NULL
};

#define EMBED_URL \
    "http://vimeo.com/moogaloop.swf?clip_id=6036944"

int
main (int argc, char *argv[]) {
    int i,rc;
    char *opts;

    for (i=0,rc=0,opts=0; formats[i] && !rc; ++i) {
        asprintf(&opts, "-f %s", formats[i]);
        rc = runtest_host(opts, "http://vimeo.com/1485507");
        free(opts);
    }

    if (rc == 0)
        rc = runtest_host(NULL, EMBED_URL);

    return (rc);
}
