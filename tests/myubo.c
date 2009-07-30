#include <stdio.h>
#include <stdlib.h>

#include "test.h"

int
main (int argc, char *argv[]) {
    int rc;

    rc = runtest_host(NULL,
        "http://www.myubo.com/page/media_detail.html?movieid=1308f0fb-47c6-40c5-a6f9-1324dac12896");

    return (rc);
}
