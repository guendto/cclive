#include <stdio.h>
#include <assert.h>

#include "test.h"

int
runtest_host (const char *cclive, const char *url) {
    char *cmd;
    int rc;

    assert(url);

    asprintf(&cmd, "%s \"%s\"", cclive, url);
    printf("RUN: %s\n", cmd);
    rc = system(cmd) >> 8;
    free(cmd);

    return(rc);
}
