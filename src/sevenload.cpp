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

SevenloadHandler::SevenloadHandler()
    : HostHandler()
{
    props.setHost   ("sevenload");
    props.setDomain ("sevenload.com");
    props.setFormats("flv");
}

void
SevenloadHandler::parseId() {
    // See parseLink below.
}

void
SevenloadHandler::parseTitle() {
    std::string title;
    try {
        partialMatch("(?i)<h1 id=\"itemtitle\">(.*?)</", &title);
    }
    catch(const HostHandler::ParseException& x) {
        partialMatch("(?i)class=\"l title\">(.*?)</", &title);
    }
    props.setTitle(title);
}

void
SevenloadHandler::parseLink() {

    std::string cpath;
    partialMatch("(?i)configpath=(.*?)\"", &cpath);
    curlmgr.unescape(cpath);

    const std::string config =
        fetch(cpath, "config", true);

    std::string id;
    partialMatch("(?i)item id=\"(.*?)\"", &id, config);
    props.setId(id);

    std::string lnk;
    partialMatch("(?i)<location seeking=\"yes\">(.*?)</", &lnk, config);
    curlmgr.escape(lnk);
    props.setLink(lnk);
}


