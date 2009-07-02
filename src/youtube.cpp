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
        if (!strcmp(opts.format_arg, "fmt18"))
            b << "&fmt=18";
        else if (!strcmp(opts.format_arg, "fmt35"))
            b << "&fmt=35";
        else if (!strcmp(opts.format_arg, "fmt22"))
            b << "&fmt=22";
        else if (!strcmp(opts.format_arg, "fmt17"))
            b << "&fmt=17";
    }
    props.setLink( b.str() );
}
