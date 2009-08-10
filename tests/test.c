#include "config.h"

#include <stdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "test.h"

int
runtest_host (const char *opts, const char *url) {
    const char *no_inet = getenv("NO_INTERNET");
    const char *extra_opts = getenv("CCLIVE_TEST_OPTS");
    register int i=0, rc=0;
    char *cmd = 0, *tmp = 0;

    if (no_inet) {
        puts("SKIP: No internet during package build");
        return(0);
    }

    setenv("CCLIVE_NO_CONFIG", "1", 1);

    if (opts)
        asprintf(&tmp, "%s -n %s %s", CCLIVE_PATH, opts, url);
    else
        asprintf(&tmp, "%s -n %s", CCLIVE_PATH, url);

    if (extra_opts)
        asprintf(&cmd, "%s %s", tmp, extra_opts);
    else
        cmd = tmp;

    printf("# ");
    puts(cmd);

    rc = system(cmd);

    if (extra_opts)
        free(cmd);

    free(tmp);

    return (rc >> 8);
}
