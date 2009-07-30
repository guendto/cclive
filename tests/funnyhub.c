#include <stdio.h>
#include <stdlib.h>

#include "test.h"

int
main (int argc, char *argv[]) {
    int rc;

    rc = runtest_host(NULL,
        "http://www.funnyhub.com/videos/pages/crazy-hole-in-one.html");

    return (rc);
}
