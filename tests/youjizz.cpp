#include <cstdlib>
#include "test.h"

MAIN_BEGIN
    char *skip = getenv("ADULT_OK");
    if (!skip) {
        std::cerr << "SKIP: Do not test adult websites." << std::endl;
        return (0);
    }
    return
        runtest_host("http://www.youjizz.com/videos/glamour-girls---melissa-125602.html");
MAIN_END


