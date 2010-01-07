#include <cstdlib>
#include "test.h"

MAIN_BEGIN
    char *skip = getenv("ADULT_OK");
    if (!skip) {
        std::cerr << "SKIP: Do not test adult websites." << std::endl;
        return (0);
    }
    return
        runtest_host("http://www.tube8.com/fetish/japanese-melon-gal-censored/186133/");

MAIN_END


