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

ClipfishHandler::ClipfishHandler()
    : HostHandler()
{
    props.setHost   ("clipfish");
    props.setDomain ("clipfish.de");
    props.setFormats("flv");
}

void
ClipfishHandler::parseId() {
    std::string id;
    partialMatch("(?i)\\/video\\/(.*?)\\/", &id, props.getPageLink());
    props.setId(id);
}

void
ClipfishHandler::parseTitle() {

    std::string title;

    partialMatch("(?i)<title>(.*?)</title>", &title);
    Util::subStrReplace(title, "Video: ", "");
    Util::subStrReplace(title, " - Clipfish", "");

    props.setTitle(title);
}

void
ClipfishHandler::parseLink() {

    std::string config_url =
        "http://www.clipfish.de/video_n.php?p=0|DE&vid=" + props.getId();

    const std::string config =
        fetch(config_url, "config", true);

    std::string lnk;
    partialMatch("(?i)&url=(.*?)&", &lnk, config);
    curlmgr.escape(lnk);
    props.setLink(lnk);
}


