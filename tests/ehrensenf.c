#include <stdio.h>

#include "test.h"

int
main (int argc, char *argv[]) {
    int rc;

    rc = runtest_host(NULL,
        "http://www.ehrensenf.de/shows/ehrensenf/"
        "getarnte-bienen-schaukelmotorrad-devitohorror");

    return(rc);
}
