#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test.h"

int
runtest_host (const char *opts, const char *url) {
    char *cclive, *cmd;
    int rc;

    cclive  = 0;
    cmd     = 0;
    rc      = 0;

    assert(opts);
    assert(url);

    asprintf(&cclive,
        "../src/cclive %s", opts);

    if (!cclive) {
        perror("asprintf");
        return(-1);
    }

    asprintf(&cmd, "%s \"%s\"", cclive, url);

    if (!cmd) {
        free(cclive);
        perror("asprintf");
        return(-1);
    }

    printf("# %s\n", cmd);

    rc = system(cmd) >> 8;

    free(cclive);
    free(cmd);

    return(rc);
}
