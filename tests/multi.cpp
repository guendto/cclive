#include "test.h"

#define TEST_URL \
    "http://break.com/index/beach-tackle-whip-lash.html"

#define TEST_URL2 \
    "http://www.youtube.com/watch?v=DeWsZ2b_pK4"

MAIN_BEGIN
#ifndef HOST_W32
    return runtest_multi(TEST_URL, TEST_URL2);
#else
    std::cout << "SKIP: Requires /dev/null" << std::endl;
    return 0;
#endif
MAIN_END


