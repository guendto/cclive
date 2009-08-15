#include <stdio.h>

#include "test.h"

int
main (int argc, char *argv[]) {
    int rc = runtest_host(NULL, "http://www.redtube.com/115732");
    if (rc == 0)
        rc = runtest_host(NULL, "http://www.redtube.com/embed/115732");
    return (rc);
}
