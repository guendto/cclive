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

#include <cstring>

#include "hosthandler.h"
#include "curl.h"

#define HOST "vimeo"

VimeoHandler::VimeoHandler()
    : HostHandler()
{
    props.setHost   (HOST);
    props.setDomain ("vimeo.com");
    props.setFormats("flv|hd");
}

void
VimeoHandler::parseId() {
    std::string id;
    partialMatch("(?i)clip_id=(.*?)\"", &id);
    props.setId(id);
}

void
VimeoHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)<meta name=\"title\" content=\"(.*?)\"", &title);
    props.setTitle(title);
}

void
VimeoHandler::parseLink() {

    std::string config_path =
        "http://vimeo.com/moogaloop/load/clip:" + props.getId();

    const std::string config = 
        fetch(config_path, "config", true);

    std::string sign;
    partialMatch("(?i)<request_signature>(.*?)</", &sign, config);

    std::string exp;
    partialMatch("(?i)<request_signature_expires>(.*?)</", &exp, config);

    std::string hd;
    try   { partialMatch("(?i)<hd_button>(.*?)</", &hd, config); }
    catch (const HostHandler::ParseException& x) { }

    std::string lnk =
        "http://vimeo.com/moogaloop/play/clip:" +props.getId()+
        "/" +sign+ "/" +exp;

    const std::string format =
        Util::parseFormatMap(HOST);

    if (format == "best" || format == "hd") {
        if (hd == "1")
            lnk += "/?q=hd";
    }

    props.setLink(lnk);
}


