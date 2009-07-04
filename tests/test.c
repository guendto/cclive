#include "config.h"

#include <stdio.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "test.h"

int
runtest_host (const char *opts, const char *url) {
    const char *args[] = { CCLIVE_PATH, url, "-n", opts, (char *)0 };
    register int i = 0;

    printf("# ");
    for (i=0; args[i]; ++i)
        printf("%s ", args[i]);
    printf("\n");

    return (execv(CCLIVE_PATH, args));
}
