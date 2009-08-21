#ifndef test_h
#define test_h

#include "config.h"

#include <iostream>
#include <sstream>

#ifdef HOST_W32
#include <windows.h>
#define sleep(n) Sleep(n*1000)
#endif

#define MAIN_BEGIN \
    int main (int argc, char *argv[]) { \
        std::cout << "\nTEST: " << argv[0] << std::endl;

#define MAIN_END }

int
runtest_returncode (const std::string& url);

int
runtest_host (const std::string& url, const std::string& opts="");

int
runtest_multi(const std::string& url, const std::string& url2);

#endif
