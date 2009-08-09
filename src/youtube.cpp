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
#include "opts.h"

YoutubeHandler::YoutubeHandler()
    : HostHandler()
{
    props.setHost   ("youtube");
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
        optsmgr.getOptions().format_arg;

    if (fmt == "best") {
        std::string best;
        partialMatch("(?i)\"fmt_map\": \"(.*?)\\/", &best);
        lnk += "&fmt=" + best;
    }
    else {

        /*
        fmt22 = HD[1280x720]
        fmt35 = HQ [640x380]
        fmt17 = 3gp[176x144]
        fmt18 = mp4[480x360]
        fmt34 = flv[320x180] */

        if (fmt == "fmt18" || fmt == "mp4")
            lnk += "&fmt=18";
        else if (fmt == "fmt35" || fmt == "hq")
            lnk += "&fmt=35";
        else if (fmt == "fmt22" || fmt == "hd")
            lnk += "&fmt=22";
        else if (fmt == "fmt17" || fmt == "3gp")
            lnk += "&fmt=17";
    }

    props.setLink(lnk);
}
