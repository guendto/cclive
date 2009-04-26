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

#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "cmdline.h"
#include "opts.h"
#include "hosthandler.h"

const bool
YoutubeHandler::isHost(std::string url) {
    props.setHost   ("youtube");
    props.setDomain ("youtube.com");
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

    if (!strcmp(opts.download_arg, "mp4"))
        b << "&fmt=18";
    else if (!strcmp(opts.download_arg, "mp4_hd"))
        b << "&fmt=22";
    else if (!strcmp(opts.download_arg, "3gpp"))
        b << "&fmt=17";
    else if (!strcmp(opts.download_arg, "xflv"))
        b << "&fmt=6";

    props.setLink( b.str() );
}