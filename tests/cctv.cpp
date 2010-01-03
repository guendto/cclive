#include "test.h"

static const char *links[] = {
"http://space.tv.cctv.com/video/VIDE1212909276513233", // regular single-segment
"http://space.tv.cctv.com/video/VIDE1247468077860061", // multi-segment
NULL
};

MAIN_BEGIN
    int i,rc;
    for (i=0,rc=0; links[i] && !rc; ++i)
        rc = runtest_host(links[i]);
    return rc;
MAIN_END


