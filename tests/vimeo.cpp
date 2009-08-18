#include "test.h"

static const char *formats[] = {
    "flv",
    "hd",
    NULL
};

#define EMBED_URL \
    "http://vimeo.com/moogaloop.swf?clip_id=6036944"

MAIN_BEGIN
    int i,rc;

    for (i=0,rc=0; formats[i] && !rc; ++i) {
        std::stringstream b;
        b << "-f " << formats[i];
        rc = runtest_host( "http://vimeo.com/1485507", b.str() );
    }

    if (rc == 0)
        rc = runtest_host(EMBED_URL);

    return (rc);
MAIN_END


