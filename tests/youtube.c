#include <stdio.h>

#include "test.h"

#define TEST_URL "http://www.youtube.com/watch?v=d0j_ONmVcXA"

static const char *
formats[] = {
    "flv",
    "fmt17",
    "fmt18",
    "fmt22",
    "fmt35",
    NULL
};

int
main (int argc, char *argv[]) {
    register int i,rc=0;
    char *cmd;

    for (i=0; formats[i]; ++i) {
        asprintf(&cmd, "../src/cclive -nf %s", formats[i]);
        rc = runtest_host(cmd, TEST_URL);
        free(cmd);
        if (rc > 0)
            break;
    }

    if (rc == 0) {
        rc = runtest_host("../src/cclive -n",
            "http://www.youtube.com/v/d0j_ONmVcXA");
    }

    if (rc == 0) {
        rc = runtest_host("../src/cclive -n",
            "http://www.youtube-nocookie.com/v/3PuHGKnboNY");
    }

    if (rc == 0) {
        rc = runtest_host("../src/cclive -n",
            "http://www.last.fm/music/Rammstein/+videos/+1-3jwXQFFLSHo");
    }

    return(rc);

}
