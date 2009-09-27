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

#define HOST "golem"

GolemHandler::GolemHandler()
    : HostHandler()
{
    props.setHost   (HOST);
    props.setDomain ("golem.de");
    props.setFormats("flv|ipod|high");
}

void
GolemHandler::parseId() {
    std::string id;
    partialMatch("(?i)\"id\", \"(.*?)\"", &id);
    props.setId(id);
}

void
GolemHandler::parseTitle() {
    // See below.
}

void
GolemHandler::parseLink() {

    std::string config_url =
        "http://video.golem.de/xml/" + props.getId();

    const std::string config =
        fetch(config_url, "config", true);

    std::string title;
    partialMatch("(?i)<title>(.*?)</title>", &title, config);
    props.setTitle(title);

    std::string lnk =
        "http://video.golem.de/download/" + props.getId();

    std::string format =
        Util::parseFormatMap(HOST);

    if (format == "best") {
        // One should not simply assume "high" (or "hd")
        // is necessarily the best format available.
        // See clive (Golem.pm) for an example.
        format = "high";
    }
    else {
        // host uses "medium" for default ("flv" in clive terms)
        if (format == "flv")
            format = ""; 
    }

    if (!format.empty())
        lnk += "?q=" + format;

    curlmgr.escape(lnk);
    props.setLink(lnk);
}


