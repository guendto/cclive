#include "test.h"

#define TEST_URL\
    "http://www.spyfilms.com/neill_blomkamp/alive_in_joburg"

static const char *
formats[] = {
    "flv",
    "hd",
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


