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
    char *cmd;

    for (i=0,rc=0,cmd=0; formats[i] && !rc; ++i) {
        asprintf(&cmd, "../src/cclive -nf %s", formats[i]);
        rc = runtest_host(cmd, "http://vimeo.com/1485507");
        free(cmd);
    }

    return (rc);
}
