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

#include "error.h"
#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "curl.h"
#include "hosthandler.h"

SevenloadHandler::SevenloadHandler()
    : HostHandler()
{
    props.setHost   ("7load");
    props.setDomain ("sevenload.com");
    props.setFormats("flv");
}

const bool
SevenloadHandler::isHost(std::string url) {
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
SevenloadHandler::parseId() {
    // See parseLink below.
}

void
SevenloadHandler::parseLink() {
    const char *cpathBegin = "configPath=";
    const char *cpathEnd   = "\"";

    std::string cpath =
        Util::subStr(pageContent, cpathBegin, cpathEnd);

    curlmgr.unescape(cpath);

    std::string config =
        curlmgr.fetchToMem(cpath, "config");

    const char *idBegin    = "item id=\"";
    const char *idEnd      = "\"";

    props.setId( Util::subStr(config, idBegin, idEnd) );

    const char *linkBegin  = "<location seeking=\"yes\">";
    const char *linkEnd    = "</location>";

    props.setLink( Util::subStr(config, linkBegin, linkEnd) );
}
