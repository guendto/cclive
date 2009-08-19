#include "test.h"

MAIN_BEGIN
    int rc = runtest_returncode("http://invalid.host");
    return (rc == 6) ? 0:1;
MAIN_END
