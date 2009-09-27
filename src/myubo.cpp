/*
 * Copyright (C) 2009 Toni Gundogdu.
 * Copyright (C) 2009 Patrick Hoffmann <patrick@sobran.de>.
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

MyuboHandler::MyuboHandler()
    : HostHandler()
{
    props.setHost   ("myubo");
    props.setDomain ("myubo.com");
    props.setFormats("flv");
}

void
MyuboHandler::parseId() {
    std::string id;
    partialMatch("(?i)movieid=(.*?)\"", &id);
    props.setId(id);
}

void
MyuboHandler::parseTitle() {
    // myubo uses a static <title>. Grab video title elsewhere.
    std::string title;
    partialMatch("(?i)<div id=\"moviedetail\"><h1>(.*?)</", &title);
    props.setTitle(title);
}

void
MyuboHandler::parseLink() {
    std::string lnk;
    partialMatch("(?i)writeflashplayer\\('(.*?)'", &lnk);
    curlmgr.escape(lnk);
    props.setLink(lnk);
}


