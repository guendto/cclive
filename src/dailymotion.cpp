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

#include <string>
#include <vector>

#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "curl.h"
#include "cmdline.h"
#include "opts.h"
#include "hosthandler.h"

const bool
DailymotionHandler::isHost(std::string url) {
    props.setHost   ("dmotion");
    props.setDomain ("dailymotion.com");
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
DailymotionHandler::parseId() {
    const char *begin = "swf%2F";
    const char *end   = "\"";

    props.setId( Util::subStr(pageContent, begin, end) );
}

#include <iostream>
void
DailymotionHandler::parseLink() {
    const char *pathsBegin = "\"video\", \"";
    const char *pathsEnd   = "\"";

    std::string paths =
        Util::subStr(pageContent, pathsBegin, pathsEnd);

    curlmgr.unescape(paths);

    std::vector<std::string> tokens =
        Util::tokenize(paths, "||");

    if (tokens.size() == 0)
        throw ParseException("paths parsing failed for \"||\"");

    const char *prefix = "http://dailymotion.com";
    Options opts = optsmgr.getOptions();

    std::string link;

    for (std::vector<std::string>::iterator iter = tokens.begin();
        iter != tokens.end();
        ++iter)
    {
        std::vector<std::string> v = Util::tokenize(*iter, "@@");
        if (v.size() > 0) {
            if (v[1] == "spark") {
                link = prefix + v[0]; // This is "flv". Set it as the default.
                continue;
            }
            if (v[1] == opts.download_arg) {
                link = prefix + v[0];
                break;
            }
        }
    }

    if (link.empty())
        throw ParseException("failed to construct link from paths");

    props.setLink(link);
}
