#include "test.h"

static const char *
formats[] = {
    "flv",
    "fmt17",
    "fmt18",
    "fmt22",
    "fmt35",
    "best",
    NULL
};

static const char *
urls[] = {
    "http://www.youtube.com/watch?v=DeWsZ2b_pK4",
    "http://www.youtube.com/v/DeWsZ2b_pK4",
    "http://www.youtube-nocookie.com/v/3PuHGKnboNY",
    "http://www.last.fm/music/Rammstein/+videos/+1-3jwXQFFLSHo",
    NULL
};

MAIN_BEGIN
    int i, rc;

    for (i=0,rc=0; formats[i] && !rc; ++i) {
        std::stringstream b;
        b << "-f " << formats[i];
        rc = runtest_host( urls[0], b.str() );
    }

    for (i=0; urls[i] && !rc; ++i)
        rc = runtest_host(urls[i]);

    return (rc);
MAIN_END


