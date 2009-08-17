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

CctvHandler::CctvHandler()
    : HostHandler()
{
    props.setHost   ("cctv");
    props.setDomain ("cctv.com");
    props.setFormats("flv");
}

void
CctvHandler::parseId() {
    std::string id;
    partialMatch("(?i)videoid=(.*?)&", &id);
    props.setId(id);
}

void
CctvHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)<meta name=\"description\" content=\"(.*?)\"", &title);
    props.setTitle(title);
}

void
CctvHandler::parseLink() {

    std::string host;
    partialMatch("http:\\/\\/(.*?)\\/", &host, props.getPageLink());

    const std::string confpath =
        "http://" + host + "/playcfg/flv_info_new.jsp?videoId=" + props.getId();

    const std::string config =
        fetch(confpath, "config");

    std::string path;
    partialMatch("url\":\"(.*?)\"", &path, config);

    props.setLink("http://v.cctv.com/flash/" + path);
}


