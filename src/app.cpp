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
#include <vector>
#include <iterator>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <tr1/memory>

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#include <curl/curl.h>

#include "hosthandler.h"
#include "hostfactory.h"
#include "macros.h"
#include "opts.h"
#include "curl.h"
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
    logmgr.init(); // apply --quiet
    curlmgr.init();
}

static void
printVideo(const VideoProperties& props) {
    logmgr.cout()
        << "file: "
        << props.getFilename()
        << "  "
        << std::setprecision(1)
        << _TOMB(props.getLength())
        << "M  ["
        << props.getContentType()
        << "]"
        << std::endl;
}

static void
printCSV(const VideoProperties& props) {
    std::cout.setf(std::ios::fixed);
    std::cout.unsetf(std::ios::showpoint);
    std::cout
        << "csv:\""
        << props.getFilename()
        << "\",\""
        << std::setprecision(0)
        << props.getLength()
        << "\",\""
        << props.getLink()
        << "\""
        << std::endl;
}

typedef CurlMgr::FetchException FetchError;

static void
fetchPage(SHP<HostHandler> handler,
          const std::string& url,
          const bool& reset=false)
{
    if (reset)
        retrymgr.reset();
    try   { handler->parsePage(url); }
    catch (const FetchError& x) {
        retrymgr.handle(x);
        fetchPage(handler, url);
    }
    logmgr.resetReturnCode();
}

static void
fetchFile(VideoProperties& props, const bool& reset=false) {
    if (reset)
        retrymgr.reset();
    try   { curlmgr.fetchToFile(props); }
    catch (const FetchError& x) {
        retrymgr.setRetryUntilRetrievedFlag();
        retrymgr.handle(x);
        fetchFile(props);
    }
    logmgr.resetReturnCode();
}

static void
queryLength(VideoProperties& props, const bool& reset=false) {
    if (reset)
        retrymgr.reset();
    try   { curlmgr.queryFileLength(props); }
    catch (const FetchError& x) {
        retrymgr.handle(x);
        queryLength(props);
    }
    logmgr.resetReturnCode();
}

static void
processVideo(VideoProperties& props) {
    queryLength(props, true);

    const Options opts =
        optsmgr.getOptions();

    if (opts.no_extract_given)
        printVideo(props);
    else if (opts.emit_csv_given)
        printCSV(props);
    else if (opts.stream_pass_given)
        execmgr.passStream(props);
    else
    {
        if (opts.print_fname_given)
            printVideo(props);

        fetchFile(props, true);
    }
}

static void
reportNotice() {
    static const char report_notice[] =
    ":: A bug? If you think so, and you can reproduce the above,\n"
    ":: consider submitting it to the issue tracker:\n"
    "::     <http://code.google.com/p/cclive/issues/>\n";
    logmgr.cerr() << report_notice << std::endl;
}

typedef HostHandlerFactory::UnsupportedHostException NoSupport;
typedef HostHandler::ParseException                  ParseError;
typedef VideoProperties::NothingToDoException        NothingTodo;

static void
handleURL(const std::string& url) {
    try
    {
        SHP<HostHandler> handler = 
            HostHandlerFactory::createHandler(url);

        fetchPage(handler, url, true);

        VideoProperties props =
            handler->getVideoProperties();

        try   { processVideo(props); }
        catch (const NothingTodo& x) { logmgr.cerr(x, false); }

        if (optsmgr.getOptions().exec_run_given) 
            execmgr.append(props);
    }
    catch (const NoSupport& x)   { logmgr.cerr(x, false); }
    catch (const FetchError& x)  { /* printed by retrymgr.handle already */ }
    catch (const ParseError& x)  { logmgr.cerr(x, false); reportNotice(); }
}

typedef std::vector<std::string> STRV;

void
App::run() {
    const Options opts = optsmgr.getOptions();

    if (opts.version_given) {
        printVersion();
        return;
    }

    if (opts.hosts_given) {
        HostHandlerFactory::printHosts();
        return;
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
    std::for_each(tokens.begin(), tokens.end(), handleURL);

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
"Copyright (C) 2009 Toni Gundogdu. "
"License GPLv3+: GNU GPL version 3 or later\n"
"This is free software; see the  source for  copying conditions.  There is NO\n"
"warranty;  not even for MERCHANTABILITY or FITNESS FOR A  PARTICULAR PURPOSE.";

    const curl_version_info_data *c =
        curl_version_info(CURLVERSION_NOW);

    std::cout
        << CMDLINE_PARSER_PACKAGE       << " version "
        << CMDLINE_PARSER_VERSION       << " with libcurl version "
        << c->version                   << "  ["
#ifdef BUILD_DATE
        << BUILD_DATE << "-"
#endif
        << CANONICAL_TARGET             << "]\n"
        << copyr_notice                 << "\n"
        << "\n  Locale/codeset  : "     << optsmgr.getLocale()
        << "\n  Config          : "     << optsmgr.getPath()
        << "\n  Features        : pcre "
#ifdef HAVE_ICONV
        << "iconv "
#endif
#ifdef WITH_RESIZE
        << "sigwinch "
#endif
        << "\n  Home            : "     << "<http://cclive.googlecode.com/>"
        << std::endl;
}


