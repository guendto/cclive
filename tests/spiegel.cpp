#include "test.h"

static const char *
formats[] = {
    "flv",
    "vp6_64",
    "vp6_576",
    "vp6_928",
    "h264_1400",
    NULL
};

#define TEST_URL \
    "http://www.spiegel.de/video/video-1012584.html"

MAIN_BEGIN
    int i, rc;
    for (i=0,rc=0; formats[i] && !rc; ++i) {
        std::stringstream b;
        b << "-f " << formats[i];
        rc = runtest_host( TEST_URL, b.str() );
    }
    return(rc);
MAIN_END


