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

#include <cstring>
#include <string>
#include <vector>

#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "cmdline.h"
#include "opts.h"
#include "hosthandler.h"

const bool
GoogleHandler::isHost(std::string url) {
    props.setHost   ("google");
    props.setDomain ("video.google.");
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
GoogleHandler::parseId() {
    const char *begin = "docid:'";
    const char *end   = "'";
    props.setId( Util::subStr(pageContent, begin, end) );
}

void
GoogleHandler::parseLink() {
    const char *begin = "videoUrl\\x3d";
    const char *end   = "\\x26";
    props.setLink( Util::subStr(pageContent, begin, end) );

#ifdef _1_
    // See clive project for Google mp4 video link extraction.
    // Once the same can be done without regexp, update the
    // code below. Implement --format=best also.
    if (!strcmp(optsmgr.getOptions().format_arg, "mp4")) {
        const char *beginMp4 = "href=\"http://vp.";
        const char *endMp4   = "\"";
        try   {
            props.setLink("http://vp."
                + Util::subStr(pageContent, beginMp4, endMp4));
        }
        catch (ParseException x) { /* default to flv */ }
    }
#endif
}
