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

LiveleakHandler::LiveleakHandler()
    : HostHandler()
{
    props.setHost   ("lleak");
    props.setDomain ("liveleak.com");
    props.setFormats("flv");
}

void
LiveleakHandler::parseId() {
    props.setId( Util::subStr(pageContent, "token=", "'") );
}

void
LiveleakHandler::parseTitle() {
    std::string title = Util::htmlTitle(pageContent);
    Util::subStrReplace(title, "LiveLeak.com - ", "");
    props.setTitle(title);
}

void
LiveleakHandler::parseLink() {
    std::string confPath =
        Util::subStr(pageContent, "'config','", "'");

    curlmgr.unescape(confPath);

    std::string config =
        curlmgr.fetchToMem(confPath, "config");

    std::string plPath =
        Util::subStr(config, "<file>", "</file>");

    curlmgr.unescape(plPath);

    std::string playlist =
        curlmgr.fetchToMem(plPath, "playlist");

    props.setLink( Util::subStr(playlist, "<location>", "</location>") );
}
