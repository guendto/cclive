#include <stdio.h>
#include <stdlib.h>

#include "test.h"

static const char *formats[] = {
    "flv",
    "hd",
    NULL
};

int
main (int argc, char *argv[]) {
    register int i,rc;
    char *opts;

    for (i=0,rc=0,opts=0; formats[i] && !rc; ++i) {
        asprintf(&opts, "-nf %s", formats[i]);
        rc = runtest_host(opts, "http://vimeo.com/1485507");
        free(opts);
    }

    return (rc);
}
