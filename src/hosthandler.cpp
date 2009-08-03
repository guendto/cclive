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

#include <string>
#include <vector>

#include "config.h"

#include "hosthandler.h"
#include "opts.h"
#include "curl.h"

#ifdef WITH_PERL
#include "pl.h"
#endif

HostHandler::HostHandler()
    : pageContent(""), props(VideoProperties())
{
}

HostHandler::~HostHandler() {
}

const bool
HostHandler::isHost(std::string url) {
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
HostHandler::parsePage(const std::string& url) {

    props.setPageLink(url);
    pageContent = curlmgr.fetchToMem(url);

#ifdef WITH_PERL
    Options opts = optsmgr.getOptions();
    if (opts.title_given)
        perlmgr.parseTitle(pageContent, props);
#endif

    // Call overridden functions
    parseId   ();
    parseLink ();

#ifdef WITH_PERL
    // parseLink above may have changed the title (video property):
    //  apply --cclass now.
    if (opts.title_given)
        perlmgr.filterTitle(props);
#endif

    this->pageContent.clear();
}

const VideoProperties&
HostHandler::getVideoProperties() const {
    return props;
}

HostHandler::
    ParseException::ParseException(const std::string& error)
        : RuntimeException(CCLIVE_PARSE, error)
{
}
