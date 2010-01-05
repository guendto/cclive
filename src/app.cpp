/*
 * Copyright (C) 2009 Toni Gundogdu.
 *
 * This file is part of cclive.
 * 
 * cclive is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * cclive is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#ifndef HAVE_PTRDIFF_T
#error Cannot compile without ptrdiff_t support
#endif

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cerrno>

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include "macros.h"
#include "opts.h"
#include "except.h"
#include "quvi.h"
#include "curl.h"
#include "util.h"
#include "exec.h"
#include "retry.h"
#include "log.h"
#include "app.h"

#if defined (SIGWINCH) && defined (TIOCGWINSZ)
#define WITH_RESIZE
#endif

#define SHP std::tr1::shared_ptr

// singleton instances
static SHP<OptionsMgr> __optsmgr (new OptionsMgr);
static SHP<QuviMgr>    __quvimgr (new QuviMgr);
static SHP<CurlMgr>    __curlmgr (new CurlMgr);
static SHP<ExecMgr>    __execmgr (new ExecMgr);
static SHP<RetryMgr>   __retrymgr(new RetryMgr);
static SHP<LogMgr>     __logmgr  (new LogMgr);

extern void handle_sigwinch(int); // src/progress.cpp

App::~App() {
}

void
App::main(const int& argc, char * const *argv) {
    optsmgr.init(argc, argv);
    logmgr.init();  // apply --quiet
    quvimgr.init(); // creates also curl handle which we'll reuse
    curlmgr.init();
}

static void
print_video(QuviVideo& props) {
    try {
        while (1) {
            logmgr.cout()
                << "file: "
                << props.getFileName()
                << "  "
                << std::setprecision(1)
                << _TOMB(props.getFileLength())
                << "M  ["
                << props.getFileContentType()
                << "]"
                << std::endl;
            props.nextVideoLink();
        }
    }
    catch (const QuviNoVideoLinkException&) {
    }
}

static void
print_csv(QuviVideo& props) {
    std::cout.setf(std::ios::fixed);
    std::cout.unsetf(std::ios::showpoint);
    try {
        while (1) {
            std::cout
                << "csv:\""
                << props.getFileName()
                << "\",\""
                << std::setprecision(0)
                << props.getFileLength()
                << "\",\""
                << props.getFileUrl()
                << "\""
                << std::endl;
            props.nextVideoLink();
        }
    }
    catch (const QuviNoVideoLinkException&) {
    }
}

static void
fetch_page(QuviVideo& props,
          const bool& reset=false)
{
    if (reset)
        retrymgr.reset();
    try { props.parse(); }
    catch (const QuviException& x) {
        retrymgr.handle(x);
        fetch_page(props);
    }
    logmgr.resetReturnCode();
}

static void
fetch_file(QuviVideo& props, const bool& reset=false) {
    if (reset)
        retrymgr.reset();

    try   { curlmgr.fetchToFile(props); }
    catch (const QuviException& x) {
        retrymgr.setRetryUntilRetrievedFlag();
        retrymgr.handle(x);
        fetch_file(props);
    }
    catch (const NothingToDoException& x) { }

    logmgr.resetReturnCode();

    try {
        props.nextVideoLink();
        fetch_file(props, true);
    }
    catch (const QuviNoVideoLinkException&) { }
}

static void
report_notice() {
    static const char report_notice[] =
    ":: Consider filing a bug report bug if you can reproduce the above\n"
    ":: results with the steps to repeat it.\n"
    "::   <http://code.google.com/p/cclive/issues/>\n";
    logmgr.cerr() << report_notice << std::endl;
}

static void
handle_error(QuviVideo& props, const RuntimeException& x) {
    logmgr.cerr(x, false);
    try {
        props.nextVideoLink();
        fetch_file(props, true);
    }
    catch (const QuviNoVideoLinkException&) { }
}

static void
handle_url(const std::string& url) {
    try
    {
        QuviVideo props(url);

        try
        {
            fetch_page(props);

            const Options opts =
                optsmgr.getOptions();

            if (opts.no_extract_given)
                print_video(props);
            else if (opts.emit_csv_given)
                print_csv(props);
            else if (opts.stream_pass_given)
                execmgr.passStream(props);
            else
            {
                if (opts.print_fname_given)
                    print_video(props);

                fetch_file(props, true);
            }

            if (optsmgr.getOptions().exec_run_given) 
                execmgr.append(props);
        }
        catch (const FileOpenException& x)
            { handle_error(props, x); }
        catch (const NothingToDoException& x)
            { handle_error(props, x); }
    }
    catch (const NoSupportException& x)
        { logmgr.cerr(x, false); }
    catch (const ParseException& x)
        { logmgr.cerr(x, false); report_notice(); }
    catch (const QuviException& x)
        { /* printed by retrymgr.handle already */ }
}

typedef std::vector<std::string> STRV;

void
App::run() {

    const Options opts =
        optsmgr.getOptions();

    if (opts.version_given) {
        printVersion();
        return;
    }

    if (opts.hosts_given) {
        std::vector<std::string> hosts;
        char *domain=0, *formats=0;

        while (quvi_iter_host(&domain, &formats) != QUVI_LASTHOST) {
            hosts.push_back(
                std::string(domain)
                + "\t"
                + std::string(formats)
                + "\n"
            );
        }

        std::sort(hosts.begin(), hosts.end());

        std::copy(hosts.begin(), hosts.end(),
            std::ostream_iterator<std::string>(std::cout));

        std::cout
            << "\nNote: Some videos may have limited number "
            << "of formats available." << std::endl;

        return;
    }
 
    if (opts.regexp_given) {
        std::string empty;
        if (!Util::perlMatch(opts.regexp_arg, empty)) {
            throw RuntimeException(CCLIVE_OPTARG,
                "--regexp: expected perl-like\n"
                "error: /pattern/(gi) regular expression");
        }
    }

    if (opts.substitute_given) {
        std::string empty; // Validate regexp only (empty string).
        if (!Util::perlSubstitute(opts.substitute_arg, empty)) {
            throw RuntimeException(CCLIVE_OPTARG,
                "--substitute: expected perl-like\n"
                "error: s/old/new/(gi) substitution");
        }
    }

    execmgr.verifyExecArgument();

#if !defined(HAVE_FORK) || !defined(HAVE_WORKING_FORK)
    if (opts.stream_exec_given) {
        logmgr.cerr()
            << "warn: this system does not have a working fork.\n"
            << "warn: --stream-exec ignored."
            << std::endl;
    }
#endif

    STRV tokens;

    typedef unsigned int _uint;

    if (!opts.inputs_num)
        tokens = parseInput();
    else {
        for (register _uint i=0; i<opts.inputs_num; ++i)
            tokens.push_back(opts.inputs[i]);
    }

    for (STRV::iterator iter=tokens.begin();
        iter != tokens.end();
        ++iter)
    {
        // Convert alternate domain link to youtube.com page link.
        Util::nocookieToYoutube(*iter);

        // Convert any embed type URLs to video page links.
        Util::embedToPage(*iter);

        // Convert last.fm video link to Youtube page link.
        if ((*iter).find("last.fm") != std::string::npos)
            Util::lastfmToYoutube(*iter);
    }

    logmgr.cout().setf(std::ios::fixed);
#ifdef WITH_RESIZE
    signal(SIGWINCH, handle_sigwinch);
#endif
    if (opts.background_given)
        daemonize();

    std::for_each(tokens.begin(), tokens.end(), handle_url);

    if (opts.exec_run_given)
        execmgr.playQueue();
}

STRV
App::parseInput() {
    std::string input;

    char ch;
    while (std::cin.get(ch))
        input += ch;

    std::istringstream iss(input);
    STRV tokens;

    std::copy(
        std::istream_iterator<std::string >(iss),
        std::istream_iterator<std::string >(),
        std::back_inserter<STRV>(tokens)
    );

    return tokens;
}

void
App::printVersion() {
static const char copyr_notice[] =
"Copyright (C) 2009,2010 Toni Gundogdu. "
"License GPLv3+: GNU GPL version 3 or later\n"
"This is free software; see the  source for  copying conditions.  There is NO\n"
"warranty;  not even for MERCHANTABILITY or FITNESS FOR A  PARTICULAR PURPOSE.";

    std::cout
        << CMDLINE_PARSER_PACKAGE       << " version "
        << CMDLINE_PARSER_VERSION       << " with libquvi version "
        << quvi_version(QUVI_VERSION)   << "  ["
#ifdef BUILD_DATE
        << BUILD_DATE << "-"
#endif
        << CANONICAL_TARGET             << "]\n"
        << copyr_notice                 << "\n"
        << "\n  Locale/codeset  : "     << optsmgr.getLocale()
        << "\n  Config          : "     << optsmgr.getPath()
        << "\n  Home            : "     << "<http://cclive.googlecode.com/>"
        << std::endl;
}

void
App::daemonize() {
#if defined(HAVE_FORK) && defined(HAVE_WORKING_FORK)
#ifdef HAVE_GETCWD
    char path[PATH_MAX];
    path[0] = '\0';
    getcwd(path, sizeof(path));
#endif

    pid_t pid = fork();

    if (pid < 0) {
#ifdef HAVE_STRERROR
        std::cerr
            << "error: fork: "
            << strerror(errno)
            << std::endl;
#else
        perror("fork");
#endif
        exit (CCLIVE_SYSTEM);
    }
    else if (pid != 0) {
        const Options opts = optsmgr.getOptions();
        std::cout
            << "Continuing in background, pid "
            << static_cast<long>(pid)
            << ".\nOutput will be written to \""
            << logmgr.getFilename()
            << "\"."
            << std::endl;
        exit (CCLIVE_OK);
    }
    setsid();
#ifdef HAVE_GETCWD
    chdir(path);
#endif
    umask(0);
#else // ifndef HAVE_FORK ...
    logmgr.cerr()
        << "warning: --background ignored: system does not support fork(2)"
        << std::endl;
#endif
}


