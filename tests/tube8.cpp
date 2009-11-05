#include <cstdlib>
#include "test.h"

MAIN_BEGIN
    char *skip = getenv("ADULT_OK");
    if (!skip) {
        std::cerr << "SKIP: Do not test adult websites." << std::endl;
        return (0);
    }
    return
        runtest_host("http://www.tube8.com/erotic/sadie-jones-sheer-undies/185361/");
MAIN_END


