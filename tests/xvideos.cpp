#include <cstdlib>
#include "test.h"

MAIN_BEGIN
    char *skip = getenv("ADULT_OK");
    if (!skip) {
        std::cerr << "SKIP: Do not test adult websites." << std::endl;
        return (0);
    }
    return
        runtest_host("http://www.xvideos.com/video243887/devi_emmerson_body_painting");
MAIN_END


