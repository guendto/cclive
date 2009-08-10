#include <stdio.h>

#include "test.h"

int
main (int argc, char *argv[]) {
    int rc;

    rc = runtest_host(NULL,
        "http://space.tv.cctv.com/video/VIDE1212909276513233");

    return(rc);
}
