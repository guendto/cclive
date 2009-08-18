#include "test.h"

#define TEST_URL \
    "http://video.google.com/videoplay?docid=-8669127848070159803"

#define TEST_EMBED_URL \
    "http://video.google.com/googleplayer.swf?docid=-8669127848070159803"

static const char
*formats[] = {
    "flv",
    "mp4",
    NULL
};

MAIN_BEGIN
    int i,rc;

    for (i=0,rc=0; formats[i] && !rc; ++i) {
        std::stringstream b;
        b << "-f " << formats[i];
        rc = runtest_host( TEST_URL, b.str() );
    }

    if (rc == 0)
        rc = runtest_host(TEST_EMBED_URL);

    return (rc);
MAIN_END


