/*
 * cclive Copyright (C) 2009 Toni Gundogdu. This file is part of cclive.
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

#include "except.h"
#include "singleton.h"
#include "cmdline.h"
#include "opts.h"
#include "macros.h"
#include "video.h"
#include "hosthandler.h"
#include "hostfactory.h"
#include "curl.h"
#include "util.h"
#include "exec.h"
#include "app.h"

// singleton instances
static std::tr1::shared_ptr<OptionsMgr> __optsmgr(new OptionsMgr);
static std::tr1::shared_ptr<CurlMgr>    __curlmgr(new CurlMgr);
static std::tr1::shared_ptr<ExecMgr>    __execmgr(new ExecMgr);

extern void handle_sigwinch(int); // src/progress.cpp

App::~App() {
}

void
App::main(int argc, char **argv) {
    optsmgr.init(argc, argv);
    curlmgr.init();
}

static void
printVideo(const VideoProperties& props) {
    std::cout
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
    std::cout.unsetf(std::ios::showpoint);
    std::cout
        << "csv:\""
        << props.getFilename()
        << "\",\""
        << std::setprecision(0)
        << props.getLength()
        << "\",\""
        << std::setprecision(0)
        << props.getInitial()
        << "\",\""
        << props.getLink()
        << "\""
        << std::endl;
}

static void
handleURL(const std::string& url) {
    try
    {
        std::tr1::shared_ptr<HostHandler> handler = 
            HostHandlerFactory::createHandler(url);

        Options opts = optsmgr.getOptions();

        if (handler->isHost(url))
        {
            try
            {
                handler->parsePage( curlmgr.fetchToMem(url) );

                VideoProperties props =
                    handler->getVideoProperties();

                try {
                    curlmgr.queryFileLength(props);

                    if (opts.no_extract_given)
                        printVideo(props);
                    else if (opts.emit_csv_given)
                        printCSV(props);
                    else
                    {
                        if (opts.print_fname_given)
                            printVideo(props);

                        curlmgr.fetchToFile(props);
                    }
                }
                catch (const VideoProperties::NothingToDoException& x) {
                    std::cerr
                        << "\nerror: file is already fully retrieved; "
                        << "nothing to do" << std::endl;
                }

                if (opts.exec_given)
                    execmgr.append(props);
            }
            catch (const CurlMgr::FetchException& x) {
                std::cerr << "\nerror: " << x.getError() << std::endl;
            }
            catch (const HostHandler::ParseException& x) {
                std::cerr << "error: " << x.getError() << std::endl;
            }
        }
    }
    catch (const HostHandlerFactory::UnsupportedHostException& x) {
        std::cerr << "error: no support: " << url << std::endl;
    }
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
        std::cerr
            << "warn: built --without-perl; ignoring --title*"
            << std::endl;
    }
#endif

    if (opts.youtube_user_given) {
        throw RuntimeException("FIXME: youtube login: patches welcome");
        //curlmgr.logIntoYoutube();
    }

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
    {   // Convert any embed type URLs to video page links
        Util::embedToPage(*iter);
        if ((*iter).find("last.fm") != std::string::npos)
            Util::lastfmToYoutube(*iter);
    }

    std::cout.setf(std::ios::fixed);
#ifdef USE_SIGWINCH
    signal(SIGWINCH, handle_sigwinch);
#endif
    std::for_each(tokens.begin(), tokens.end(), handleURL);

    if (opts.exec_given)
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
      "Copyright (C) 2009 Toni Gundogdu <legatvs@gmail.com>.\n\n"
      "This program is free software: you can redistribute it and/or modify\n"
      "it under the terms of the GNU General Public License as published by\n"
      "the Free Software Foundation, either version 3 of the License, or\n"
      "(at your option) any later version <http://www.gnu.org/licenses/>.";

    curl_version_info_data *c =
        curl_version_info(CURLVERSION_NOW);

    std::cout
        << CMDLINE_PARSER_PACKAGE   << " version "
        << CMDLINE_PARSER_VERSION   << " with libcurl version "
        << c->version               << "  ["
        << CANONICAL_TARGET         << "]\n"
        << copyr_notice             << "\n\n\tFeatures: ";

#ifdef USE_SIGWINCH
    std::cout << "--enable-sigwinch ";
#endif
#ifdef WITH_PERL
    std::cout << "--with-perl ";
#endif
    std::cout
        << "\n\tReport bugs: <http://code.google.com/p/cclive/issues/>"
        << std::endl;
}
