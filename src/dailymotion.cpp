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

#define HOST "dailymotion"

DailymotionHandler::DailymotionHandler()
    : HostHandler()
{
    props.setHost   (HOST);
    props.setDomain ("dailymotion.");
    props.setFormats("flv|spark-mini|vp6-hq|vp6-hd|vp6|h264");
}

void
DailymotionHandler::parseId() {
    std::string id;
    partialMatch("(?i)video\\/(.*?)_", &id);
    props.setId(id);
}

void
DailymotionHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)<h1 class=\"dmco_title\">(.*?)</", &title);
    props.setTitle(title);
}

typedef std::vector<std::string> STRV;

void
DailymotionHandler::parseLink() {

    std::string paths;
    partialMatch("(?i)\"video\", \"(.*?)\"", &paths);
    curlmgr.unescape(paths);

    const STRV tokens =
        Util::tokenize(paths, "||");

    if (tokens.size() == 0)
        throw ParseException("unable to tokenize (\"||\")");

    std::string format =
        Util::parseFormatMap(HOST);

    if (format == "flv")
        format = "spark";

    std::string lnk;
    std::map<int, std::string, std::greater<int> > width;

    for (STRV::const_iterator iter = tokens.begin();
        iter != tokens.end();
        ++iter)
    {
        const STRV v =
            Util::tokenize(*iter, "@@");

        if (v.size() == 0)
            continue;

        const int w =
            atoi(Util::subStr(v[0], "-", "x").c_str());

        width[w] = v[0];

        if (v[1] == format && format != "best") {
            lnk = v[0];
            break;
        }
    }

    // std::map sorts by key (width) in descending order
    // automatically. Assume first element to be the best.
    if (format == "best")
        lnk = (width.begin())->second;

    if (lnk.empty())
        throw ParseException("failed to construct link from paths");

    props.setLink(lnk);
}


