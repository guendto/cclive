#include <stdio.h>

#include "test.h"

static const char *formats[] = {
    "flv",
    "hd",
    NULL
};

int
main (int argc, char *argv[]) {
    register int i,rc=0;
    char *cmd;
    for (i=0; formats[i]; ++i) {
        asprintf(&cmd, "../src/cclive -nf %s", formats[i]);
        rc = runtest_host(cmd, "http://vimeo.com/1485507");
        free(cmd);
        if (rc > 0)
            break;
    }
    return (rc);
}
