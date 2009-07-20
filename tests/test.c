#include "config.h"

#include <stdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "test.h"

int
runtest_host (const char *opts, const char *url) {
    const char *no_inet = (const char *)getenv("NO_INTERNET");
    register int i = 0;
    char *path = 0;
    int rc = 0;

    if (no_inet) {
        puts("SKIP: No internet during package build");
        return(0);
    }

    printf("# ");
    if (opts)
        asprintf(&path, "%s -n %s %s", CCLIVE_PATH, opts, url);
    else
        asprintf(&path, "%s -n %s", CCLIVE_PATH, url);
    puts(path);

    rc = system(path);

    free(path);

    return (rc >> 8);
}
