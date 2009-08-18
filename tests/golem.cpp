#include "test.h"

#define TEST_URL\
    "http://video.golem.de/internet/2174/firefox-3.5-test.html"

static const char *
formats[] = {
    "flv",
    "high",
    "ipod",
    NULL
};

MAIN_BEGIN
    int i,rc;
    for (i=0,rc=0; formats[i] && !rc; ++i) {
        std::stringstream b;
        b << "-f " << formats[i];
        rc = runtest_host( TEST_URL, b.str() );
    }
    return (rc);
MAIN_END


