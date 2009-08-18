#include "test.h"

MAIN_BEGIN
    int rc = runtest_host("http://www.redtube.com/3644");
    if (rc == 0)
        rc = runtest_host("http://www.redtube.com/embed/3644");
    return (rc);
MAIN_END


