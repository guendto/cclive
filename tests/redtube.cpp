#include <cstdlib>
#include "test.h"

MAIN_BEGIN
    char *skip = getenv("ADULT_OK");
    if (!skip) {
        std::cerr << "SKIP: Do not test adult websites." << std::endl;
        return (0);
    }
    int rc = runtest_host("http://www.redtube.com/3644");
    if (rc == 0)
        rc = runtest_host("http://www.redtube.com/embed/3644");
    return (rc);
MAIN_END


