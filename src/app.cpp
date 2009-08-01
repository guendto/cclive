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

#ifdef HOST_W32
// A peculiar thing this one. If commented out or included *after* "config.h",
// mingw32-g++ returns: error: '::malloc' has not been declared
#include <cstdlib>
#endif

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

#ifdef USE_SIGWINCH
#include <signal.h>
#endif

#include <curl/curl.h>

#include "hosthandler.h"
#include "hostfactory.h"
#include "macros.h"
#include "opts.h"
#include "curl.h"
#include "exec.h"
#include "log.h"
#include "app.h"

// singleton instances
static std::tr1::shared_ptr<OptionsMgr> __optsmgr(new OptionsMgr);
static std::tr1::shared_ptr<CurlMgr>    __curlmgr(new CurlMgr);
static std::tr1::shared_ptr<ExecMgr>    __execmgr(new ExecMgr);
static std::tr1::shared_ptr<LogMgr>     __logmgr (new LogMgr);

extern void handle_sigwinch(int); // src/progress.cpp

App::~App() {
}

void
App::main(int argc, char **argv) {
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

typedef VideoProperties::NothingToDoException        NothingTodo;
typedef CurlMgr::FetchException                      FetchError;
typedef HostHandler::ParseException                  ParseError;
typedef HostHandlerFactory::UnsupportedHostException NoSupport;

static void
handleURL(const std::string& url) {
    try {
        std::tr1::shared_ptr<HostHandler> handler = 
            HostHandlerFactory::createHandler(url);

        Options opts = optsmgr.getOptions();

        try {
            handler->parsePage(url);

            VideoProperties props =
                handler->getVideoProperties();

            try {
                curlmgr.queryFileLength(props);

                // Must be called here since the above parses the file
                // content-type and figures out the file suffix.
                props.formatOutputFilename();

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

                    curlmgr.fetchToFile(props);
                }
            }
            catch (const NothingTodo& x) { logmgr.cerr(x, false); }

            if (opts.exec_run_given) 
                execmgr.append(props);
        }
        catch (const FetchError& x) { logmgr.cerr(x); }
        catch (const ParseError& x) { logmgr.cerr(x, false); }
    }
    catch (const NoSupport& x) { logmgr.cerr(x, false); }
}

void
App::run() {
    Options opts = optsmgr.getOptions();

    if (opts.version_given) {
        printVersion();
        return;
    }

    if (opts.hosts_given) {
        HostHandlerFactory::printHosts();
        return;
    }

    execmgr.verifyExecArgument();

#ifndef WITH_PERL
    if (opts.title_given) {
        logmgr.cerr()
            << "warn: built without perl: ignoring --title"
            << std::endl;
    }
#endif

#ifdef HOST_W32
    if (opts.stream_exec_given) {
        logmgr.cerr()
            << "warn: system does not support fork/waitpid: "
            << "ignoring --stream-exec"
            << std::endl;
    }
#endif

    std::vector<std::string> tokens;

    if (!opts.inputs_num) {
        tokens = parseInput();
    }
    else {
        for (register unsigned int i=0; i<opts.inputs_num; ++i)
            tokens.push_back(opts.inputs[i]);
    }

    for (std::vector<std::string>::iterator iter=tokens.begin();
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
#ifdef USE_SIGWINCH
    signal(SIGWINCH, handle_sigwinch);
#endif
    std::for_each(tokens.begin(), tokens.end(), handleURL);

    if (opts.exec_run_given)
        execmgr.playQueue();
}

std::vector<std::string>
App::parseInput() {
    std::string input;

    char ch;
    while (std::cin.get(ch))
        input += ch;

    std::istringstream iss(input);
    std::vector<std::string>tokens;

    std::copy(
        std::istream_iterator<std::string >(iss),
        std::istream_iterator<std::string >(),
        std::back_inserter<std::vector<std::string> >(tokens)
    );
    return tokens;
}

void
App::printVersion() {
static const char copyr_notice[] =
"Copyright (C) 2009 Toni Gundogdu.\n\n"
"License GPLv3+: GNU GPL version 3 or later\n"
"  <http://www.gnu.org/licenses/>\n\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.";

    curl_version_info_data *c =
        curl_version_info(CURLVERSION_NOW);

    char *locale = getenv("LANG");
    if (!locale)
        locale = const_cast<char *>("undefined");

    std::cout
        << CMDLINE_PARSER_PACKAGE   << " version "
        << CMDLINE_PARSER_VERSION   << " with libcurl version "
        << c->version               << "  ["
        << CANONICAL_TARGET         << "]\n"
        << copyr_notice             << "\n"
        << "\n  Config  : "         << optsmgr.getPath()
        << "\n  Features: "
#ifdef USE_SIGWINCH
        << "--enable-sigwinch "
#endif
#ifdef WITH_PERL
        << "--with-perl "
#endif
        << "\n  Locale  : "           << locale
        << "\n  Home    : "           << "<http://cclive.googlecode.com/>"
        << std::endl;
}
