
#include "test.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

static void
init (std::stringstream &b, const std::string& opts="") {
    const char *no_inet =
        getenv("NO_INTERNET");

    if (no_inet)
        throw std::runtime_error("SKIP: No internet during package build");

    const char *extra_opts =
        getenv("CCLIVE_TEST_OPTS");

    b << CCLIVE_PATH << " ";

    if ( !opts.empty() )
        b << opts << " ";

    if (extra_opts)
        b << extra_opts << " ";
}

static int
run (std::stringstream& b) {
    std::cout << "# " << b.str() << std::endl;
#ifdef HAVE_SETENV
    setenv("CCLIVE_NO_CONFIG", "1", 1);
#endif
#if defined(HAVE_FORK) && defined(HAVE_WORKING_FORK)
    pid_t child = fork();

    if (child < 0)
        perror("fork");
    else if (child == 0)
        exit( system( b.str().c_str() ) >> 8 );

    int rc = 0;
    if (waitpid(child, &rc, 0) != child)
        perror("waitpid");

    if (WIFEXITED(rc)) {
        std::cerr << "# child: normal termination: "
                  << WEXITSTATUS(rc)
                  << std::endl;
    }
    else if (WIFSIGNALED(rc)) {
        std::cerr << "# child: abnormal termination: "
                  << WTERMSIG(rc)
                  << std::endl;
    }
    else if (WIFSTOPPED(rc)) {
        std::cerr << "# child: stopped: "
                  << WSTOPSIG(rc)
                  << std::endl;
    }
    return WEXITSTATUS(rc);
#else
    return system( b.str().c_str() ) >> 8;
#endif
}

int
runtest_multi (const std::string& url, const std::string& url2) {
    std::stringstream b;
    try {
        init(b);
        b << "-f 3gp -O /dev/null \""
          << url
          << "\" \""
          << url2
          << "\" ";
        return run(b);
    }
    catch (const std::runtime_error& x) {
        std::cerr << x.what() << std::endl;
    }
    return 0;
}

int
runtest_host (const std::string& url, const std::string& opts/*=""*/) {
    std::stringstream b;
    try {
        init(b, opts);
        b << "-n \"" << url << "\" ";
        return run(b);
    }
    catch (const std::runtime_error& x) {
        std::cerr << x.what() << std::endl;
    }
    return 0;
}
