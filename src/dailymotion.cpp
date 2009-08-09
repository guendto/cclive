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

#include <map>

#include "hosthandler.h"
#include "curl.h"
#include "opts.h"

DailymotionHandler::DailymotionHandler()
    : HostHandler()
{
    props.setHost   ("dmotion");
    props.setDomain ("dailymotion.com");
    props.setFormats("flv|spak-mini|vp6-hq|vp6-hd|vp6|h264");
}

void
DailymotionHandler::parseId() {
    std::string id;
    partialMatch("(?i)swf\\/(.*?)_", &id);
    props.setId(id);
}

void
DailymotionHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)<h1 class=\"dmco_title\">(.*?)</", &title);
    props.setTitle(title);
}

void
DailymotionHandler::parseLink() {

    std::string paths;
    partialMatch("(?i)\"video\", \"(.*?)\"", &paths);
    curlmgr.unescape(paths);

    std::vector<std::string> tokens =
        Util::tokenize(paths, "||");

    if (tokens.size() == 0)
        throw ParseException("unable to tokenize (\"||\")");

    std::string format =
        optsmgr.getOptions().format_arg;

    if (format == "flv")
        format = "spark";

    std::string link;
    std::map<std::string, std::string> width;

    for (std::vector<std::string>::iterator iter = tokens.begin();
        iter != tokens.end();
        ++iter)
    {
        std::vector<std::string> v =
            Util::tokenize(*iter, "@@");

        if (v.size() == 0)
            continue;

        std::string w = Util::subStr(v[0], "-", "x");
        width[w] = v[0];

        if (v[1] == format && format != "best") {
            link = v[0];
            break;
        }
    }

    // std::map sorts by key (width) in descending order
    // automatically. Assume first element to be the best.
    if (format == "best")
        link = (width.begin())->second;

    if (link.empty())
        throw ParseException("failed to construct link from paths");

    props.setLink(link);
}


