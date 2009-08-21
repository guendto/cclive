#include "test.h"

static const char *
urls[] = {
    "http://www.liveleak.com/view?i=704_1228511265",
    "http://www.liveleak.com/e/6ff_1228698283",
    NULL
};

MAIN_BEGIN
    int i,rc;
    for (i=0,rc=0; urls[i] && !rc; ++i) {
        rc = runtest_host(urls[i]);
        if (rc == 0 && i == 0) {
            std::cout << "# wait 10s." << std::endl;
            sleep(10);
        }
    }
    return(rc);
MAIN_END


