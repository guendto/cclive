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

#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <vector>

#include "except.h"
#include "video.h"
#include "hosthandler.h"
#include "util.h"

const double
Util::fileExists(const std::string& path) {
    std::ifstream f;
    f.open(path.c_str(), std::ios::binary);

    double len = 0;

    if (f.is_open()) {
        f.seekg(0, std::ios::end);
        len = f.tellg();
        f.close();
    }
    return len;
}

const std::string
Util::subStr(const std::string& src,
             const std::string& begin,
             const std::string& end)
{
    std::string::size_type begin_pos = src.find(begin);
    if (begin_pos == std::string::npos)
        throw HostHandler::ParseException("not found: "+begin);

    std::string::size_type end_pos =
        src.find(end, begin_pos + begin.length());

    if (end_pos == std::string::npos)
        throw HostHandler::ParseException("not found: "+end);

    std::string::size_type from = begin_pos + begin.length();
    std::string::size_type len  = end_pos - begin_pos - begin.length();

    return src.substr(from, len);
}

std::string&
Util::subStrReplace(
    std::string& src,
    const std::string& what,
    const std::string& with)
{
    std::string::size_type pos;
    while ((pos = src.find(what)) != std::string::npos) {
        src.replace(pos, what.size(), with);
    }
    return src;
}

std::string&
Util::embedToPage(std::string& url) {
    typedef std::map<std::string, std::string> mapstr;
    mapstr m;
    m["/v/"]                = "/watch?v=";  // youtube
    m["googleplayer.swf"]   = "videoplay";  // google
    m["/pl/"]               = "/videos/";   // sevenload
    m["/e/"]                = "/view?i=";   // liveleak
    for (mapstr::iterator iter = m.begin();
        iter != m.end();
        ++iter)
    {
        Util::subStrReplace(url, iter->first, iter->second);
    }
    return url;
}

std::string&
Util::lastfmToYoutube(std::string& url) {
    std::string::size_type pos = url.find("+1-");
    if (pos != std::string::npos) {
        std::string id = url.substr(pos+3);
        url = "http://youtube.com/watch?v=" + id;
    }
    return url;
}

std::string&
Util::toLower(std::string& src) {
    std::transform(src.begin(), src.end(),
        src.begin(), tolower);
    return src;
}

std::vector<std::string>
Util::tokenize(const std::string& src, const std::string& delims) {
    std::string::size_type last = src.find_first_not_of(delims);
    std::string::size_type pos  = src.find_first_of(delims, last);

    std::vector<std::string> v;

    while (std::string::npos != pos || std::string::npos != last) {
        v.push_back( src.substr(last, pos-last) );
        last = src.find_first_not_of(delims, pos);
        pos  = src.find_first_of(delims, last);
    }
    return v;
}