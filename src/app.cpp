/*
 * Copyright (C) 2009,2010 Toni Gundogdu.
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

#include <pcrecpp.h>

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
static SHP<LogMgr>     __logmgr  (new LogMgr);

extern void handle_sigwinch(int); // src/progressbar.cpp
extern void fetch_page(QuviVideo&, const bool&); // src/retry.cpp
extern void fetch_file(QuviVideo&, const bool&); // src/retry.cpp

void
App::main(const int& argc, char * const *argv) {
    optsmgr.init(argc, argv);
    logmgr.init();  // apply --quiet
    quvimgr.init(); // creates also curl handle which we'll reuse
    curlmgr.init();
}

static void
print_fname(QuviVideo& qv) {
    logmgr.cout()
        << "file: "
        << qv.getFileName()
        << "  "
        << std::setprecision(1)
        << _TOMB(qv.getFileLength())
        << "M  ["
        << qv.getFileContentType()
        << "]"
        << std::endl;
}

static void
print_video(QuviVideo& qv) {
    try {
        while (1) {
            print_fname(qv);
            qv.nextVideoLink();
        }
    }
    catch (const QuviNoVideoLinkException&) { }
}

static void
print_csv(QuviVideo& qv) {
    std::cout.setf(std::ios::fixed);
    std::cout.unsetf(std::ios::showpoint);
    try {
        while (1) {
            std::cout
                << "csv:\""
                << qv.getFileName()
                << "\",\""
                << std::setprecision(0)
                << qv.getFileLength()
                << "\",\""
                << qv.getFileUrl()
                << "\""
                << std::endl;
            qv.nextVideoLink();
        }
    }
    catch (const QuviNoVideoLinkException&) { }
}

#define next_video_link(log_error, prepend_newline) \
    do { \
        if (log_error) \
            logmgr.cerr(x, prepend_newline); \
        try { \
            qv.nextVideoLink(); \
            handle_video(qv); \
        } \
        catch (const QuviNoVideoLinkException&) { } \
    } while(0)

static void
handle_video(QuviVideo& qv) {
    const Options opts = optsmgr.getOptions();
    try {
        if (opts.print_fname_given)
            print_fname(qv);

        fetch_file(qv, true);

        qv.nextVideoLink();
        handle_video(qv);
    }
    // This is actually a curl error: 
    // * We no longer rely on libquvi here
    // * Reuse the QuviException class
    catch (const QuviException& x)
        { next_video_link(true, true); }
    catch (const NothingToDoException& x)
        { next_video_link(true, false); }
    catch (const FileOpenException& x)
        { next_video_link(true, false); }
    catch (const NoMoreRetriesException& x)
        { logmgr.cerr() << "give up trying." << std::endl;
          next_video_link(false, false); }
    catch (const QuviNoVideoLinkException& x)
        { /* Triggered by qv.nextvideoLink() in the above try-block. */ }
}

static void
handle_url(const std::string& url) {
    try {
        QuviVideo qv(url);

        fetch_page(qv, true/*reset retry counter*/);

        const Options opts = optsmgr.getOptions();

        if (opts.no_extract_given)
            print_video(qv);
        else if (opts.emit_csv_given)
            print_csv(qv);
        else if (opts.stream_pass_given)
            execmgr.passStream(qv);
        else
            handle_video(qv);
    }
    catch (const QuviException& x)
        { logmgr.cerr(x, false); }
    catch (const NoSupportException& x)
        { logmgr.cerr(x, false); }
    catch (const ParseException& x)
        { logmgr.cerr(x, false); }
    catch (const NoMoreRetriesException& x)
        { logmgr.cerr() << "give up trying." << std::endl; }
    catch (const QuviNoVideoLinkException& x)
        { }
}

static void
verify_format_id (const Options& opts) {

    if (!opts.format_given)
        return;

    pcrecpp::RE_Options re_opts;
    re_opts.set_caseless(true);

    std::stringstream pattern;
    pattern
        << "(?:\\||^)"
        << opts.format_arg
        << "(?:\\||$)";

    pcrecpp::RE re(pattern.str(), re_opts);

    char *domain, *formats;
    while (quvi_next_host(&domain, &formats) == QUVI_OK) {
        if (re.PartialMatch(formats))
            return;
    }

    std::stringstream b;
    b << "--format=" << opts.format_arg;

    throw RuntimeException(CCLIVE_OPTARG, b.str());
}

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
        char *domain, *formats;

        while (quvi_next_host(&domain, &formats) == QUVI_OK) {
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

    verify_format_id(opts);
    execmgr.verifyExecArgument();

#if !defined(HAVE_FORK) || !defined(HAVE_WORKING_FORK)
    if (opts.stream_exec_given) {
        logmgr.cerr()
            << "warn: this system does not have a working fork.\n"
            << "warn: --stream-exec ignored."
            << std::endl;
    }
#endif

    quvi::StringVector tokens;

    typedef unsigned int _uint;

    if (!opts.inputs_num)
        tokens = parseInput();
    else {
        for (register _uint i=0; i<opts.inputs_num; ++i)
            tokens.push_back(opts.inputs[i]);
    }

    for (quvi::StringVector::iterator iter=tokens.begin();
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

quvi::StringVector
App::parseInput() {
    std::string input;

    char ch;
    while (std::cin.get(ch))
        input += ch;

    std::istringstream iss(input);
    quvi::StringVector tokens;

    std::copy(
        std::istream_iterator<std::string >(iss),
        std::istream_iterator<std::string >(),
        std::back_inserter<quvi::StringVector>(tokens)
    );

    return tokens;
}

void
App::printVersion() {
static const char copyr_notice[] =
"Copyright (C) 2009,2010 Toni Gundogdu. "
"License: GNU GPL version  3 or  later\n"
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


