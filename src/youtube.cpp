/*
 * cclive Copyright (C) 2009 Toni Gundogdu. This file is part of cclive.
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
#include <string>
#include <cstring>
#include <vector>

#include "error.h"
#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "cmdline.h"
#include "opts.h"
#include "hosthandler.h"

YoutubeHandler::YoutubeHandler()
    : HostHandler()
{
    props.setHost   ("youtube");
    props.setDomain ("youtube.com");
    props.setFormats("flv|3gp|mp4|hq|hd");
}

const bool
YoutubeHandler::isHost(std::string url) {
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
YoutubeHandler::parseId() {
    const char *begin = "\"video_id\": \"";
    const char *end   = "\"";
    props.setId( Util::subStr(pageContent, begin, end) );
}

void
YoutubeHandler::parseLink() {
    const char *begin = "\"t\": \"";
    const char *end   = "\"";
    std::string t     = Util::subStr(pageContent, begin, end);

    std::ostringstream b;

    b << "http://youtube.com/get_video?video_id="
      << props.getId()
      << "&t="
      << t;

    Options opts = optsmgr.getOptions();

    if (!strcmp(opts.format_arg, "best")) {
        b << "&fmt="
          <<  Util::subStr(pageContent, "\"fmt_map\": \"", "/");
    }
    else {

        /*
        fmt22 = HD[1280x720]
        fmt35 = HQ [640x380]
        fmt17 = 3gp[176x144]
        fmt18 = mp4[480x360]
        fmt34 = flv[320x180] */

        std::string fmt = opts.format_arg;
        if (fmt == "fmt18" || fmt == "mp4")
            b << "&fmt=18";
        else if (fmt == "fmt35" || fmt == "hq")
            b << "&fmt=35";
        else if (fmt == "fmt22" || fmt == "hd")
            b << "&fmt=22";
        else if (fmt == "fmt17" || fmt == "3gp")
            b << "&fmt=17";
    }
    props.setLink( b.str() );
}
