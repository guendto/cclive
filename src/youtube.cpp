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

#define HOST "youtube"

YoutubeHandler::YoutubeHandler()
    : HostHandler()
{
    props.setHost   (HOST);
    props.setDomain ("youtube.com");
    props.setFormats("flv|3gp|mp4|hq|hd");
}

void
YoutubeHandler::parseId() {
    std::string id;
    partialMatch("(?i)\"video_id\": \"(.*?)\"", &id);
    props.setId(id);
}

void
YoutubeHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)<meta name=\"title\" content=\"(.*?)\"", &title);
    props.setTitle(title);
}

void
YoutubeHandler::parseLink() {

    std::string t;
    partialMatch("(?i)\"t\": \"(.*?)\"", &t);

    std::string lnk =
        "http://youtube.com/get_video?video_id=" +props.getId()+ "&t=" +t;

    const std::string fmt =
        Util::parseFormatMap(HOST);

    if (fmt == "best") {
        try {
            std::string best;
            partialMatch("(?i)\"fmt_map\": \"(\\d+)", &best);
            lnk += "&fmt=" + best;
        }
        catch (const HostHandler::ParseException& x) {
            // Ignore. Go with the default format instead.
        }
    }
    else {

        /*
        * Youtube/Google likes to rehash these from time to time.
        *
        * It's not uncommon that some formats are available only
        * for some videos. The following lists aliases for the
        * supported "fmt" strings, e.g. fmt22=hd.
        *
        * fmt22 = HD [1280x720]
        * fmt35 = HQ  [640x380]
        * fmt18 = mp4 [480x360]
        * fmt34 = -   [320x180] quality reportedly varies
        * fmt17 = 3gp [176x144]
        *
        * If --format is unused, we default to whatever youtube
        * defaults to by leaving "&fmt=" from the video link.
        */

        if (fmt == "fmt18" || fmt == "mp4")
            lnk += "&fmt=18";
        else if (fmt == "fmt34")
            lnk += "&fmt=34";
        else if (fmt == "fmt35" || fmt == "hq")
            lnk += "&fmt=35";
        else if (fmt == "fmt22" || fmt == "hd")
            lnk += "&fmt=22";
        else if (fmt == "fmt17" || fmt == "3gp")
            lnk += "&fmt=17";
    }

    curlmgr.escape(lnk);
    props.setLink(lnk);
}


