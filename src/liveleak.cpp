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

#include <string>
#include <vector>

#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "curl.h"
#include "hosthandler.h"

const bool
LiveleakHandler::isHost(std::string url) {
    props.setHost   ("lleak");
    props.setDomain ("liveleak.com");
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
LiveleakHandler::parseId() {
    const char *begin = "token=";
    const char *end   = "'";

    props.setId( Util::subStr(pageContent, begin, end) );
}

void
LiveleakHandler::parseLink() {
    const char *cpathBegin = "'config','";
    const char *cpathEnd   = "'";

    std::string confPath =
        Util::subStr(pageContent, cpathBegin, cpathEnd);

    curlmgr.unescape(confPath);

    std::string config =
        curlmgr.fetchToMem(confPath, "config");

    const char *plBegin = "<file>";
    const char *plEnd   = "</file>";

    std::string plPath =
        Util::subStr(config, plBegin, plEnd);

    curlmgr.unescape(plPath);

    std::string playlist =
        curlmgr.fetchToMem(plPath, "playlist");

    const char *linkBegin = "<location>";
    const char *linkEnd   = "</location>";

    props.setLink( Util::subStr(playlist, linkBegin, linkEnd) );
}
