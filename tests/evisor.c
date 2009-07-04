#include <stdio.h>

#include "test.h"

int
main (int argc, char *argv[]) {
    int rc;
    
    rc = runtest_host(NULL,
        "http://www.evisor.tv/tv/rennstrecken/"
        "1-runde-oschersleben-14082008--6985.htm");

    return (rc);
}
