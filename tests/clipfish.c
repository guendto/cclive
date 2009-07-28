#include <stdio.h>
#include <stdlib.h>

#include "test.h"

int
main (int argc, char *argv[]) {
    int rc;

    rc = runtest_host(NULL,
        "http://www.clipfish.de/video/3100131/matratzendomino/");

    return (rc);
}
