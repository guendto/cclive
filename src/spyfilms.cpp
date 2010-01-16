/*
 * Copyright (C) 2010 Toni Gundogdu.
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

#include <sstream>

#include "hosthandler.h"

#define HOST "spyfilms"

SpyfilmsHandler::SpyfilmsHandler()
    : HostHandler()
{
    props.setHost   (HOST);
    props.setDomain ("spyfilms.com");
    props.setFormats("flv|hd");
}

void
SpyfilmsHandler::parseId() {
    // Done in further in parseTitle+parseLink.
}

void
SpyfilmsHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)fv_title:\\s+\"(.*?)\"", &title);
    props.setId(title); // NOTE: tweaked in parseLink.
    title = Util::subStrReplace(title, "+", " ");
    props.setTitle(title);
}

void
SpyfilmsHandler::parseLink() {
    std::string lnk;
    partialMatch("(?i)fv_flv:\\s+\"(.*?)\"", &lnk);

    std::string fmt = Util::parseFormatMap(HOST);

    if (fmt == "hd" || fmt == "best") {
        try   { partialMatch("(?i)fv_hd:\\s+\"(.*?)\"",&lnk); }
        catch (const HostHandler::ParseException&) { }
    }

    std::string reel_path;
    partialMatch("(?i)fv_reel_path:\\s+\"(.*?)\"", &reel_path);

    std::string dir_path;
    partialMatch("(?i)fv_director_path:\\s+\"(.*?)\"", &dir_path);

    std::stringstream b;
    b << reel_path << dir_path << "/" << lnk;

    props.setLink(b.str());

    // TODO: find a better video id scheme.
    std::string dir_id;
    partialMatch("(?i)fv_director_id:\\s+\"(.*?)\"", &dir_id);

    std::string id = props.getId();

    std::stringstream tmp;
    tmp << dir_id << Util::subStrReplace(id, "+", "_");

    props.setId(tmp.str());
}


