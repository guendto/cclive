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

#include "hosthandler.h"
#include "curl.h"

SevenloadHandler::SevenloadHandler()
    : HostHandler()
{
    props.setHost   ("7load");
    props.setDomain ("sevenload.com");
    props.setFormats("flv");
}

void
SevenloadHandler::parseId() {
    // See parseLink below.
}

void
SevenloadHandler::parseLink() {
    std::string cpath =
        Util::subStr(pageContent, "configPath=", "\"");

    curlmgr.unescape(cpath);

    std::string config =
        curlmgr.fetchToMem(cpath, "config");

    props.setId( Util::subStr(config, "item id=\"", "\"") );

    props.setLink(
        Util::subStr(config, "<location seeking=\"yes\">", "</location>") );
}
