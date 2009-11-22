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

#include "config.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cerrno>

#ifdef HAVE_ICONV
#include <cerrno>
#include <iconv.h>
#endif

#include "hosthandler.h"
#include "opts.h"
#include "log.h"

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
             const std::string& end,
             const bool& croak_if_not_found /*=true*/)
{
    const std::string::size_type begin_pos = src.find(begin);
    if (begin_pos == std::string::npos && croak_if_not_found)
        throw HostHandler::ParseException("not found: "+begin);

    const std::string::size_type end_pos =
        src.find(end, begin_pos + begin.length());

    if (end_pos == std::string::npos && croak_if_not_found)
        throw HostHandler::ParseException("not found: "+end);

    const std::string::size_type from =
        begin_pos + begin.length();

    const std::string::size_type len =
        end_pos - begin_pos - begin.length();

    return src.substr(from, len);
}

const std::string
Util::rsubStr(const std::string& src,
        const std::string& begin,
        const std::string& end,
        const bool& croak_if_not_found /*=true*/)
{
    const std::string::size_type end_pos = src.rfind(end);
    if (end_pos == std::string::npos && croak_if_not_found)
        throw HostHandler::ParseException("not found: "+end);

    const std::string::size_type begin_pos =
        src.rfind(begin, end_pos - end.length());

    if (begin_pos == std::string::npos && croak_if_not_found)
        throw HostHandler::ParseException("not found: "+begin);

    const std::string::size_type from =
        begin_pos + begin.length();

    const std::string::size_type len =
        end_pos - begin_pos - begin.length();

    return src.substr(from, len);
}

std::string&
Util::subStrReplace(
    std::string& src,
    const std::string& what,
    const std::string& with)
{
    std::string::size_type pos;

    while ((pos = src.find(what)) != std::string::npos)
        src.replace(pos, what.size(), with);

    return src;
}

std::string&
Util::nocookieToYoutube(std::string& url) {
    // Convert alternate domain youtube-nocookie.com
    // to youtube.com domain.
    return Util::subStrReplace(url, "-nocookie", "");
}

std::string&
Util::embedToPage(std::string& url) {
    typedef std::map<std::string, std::string> mapstr;

    mapstr m;

    m["/v/"]                     = "/watch?v="; // youtube
    m["googleplayer.swf"]        = "videoplay"; // google
    m["/pl/"]                    = "/videos/";  // sevenload
    m["/e/"]                     = "/view?i=";  // liveleak
    m["/moogaloop.swf?clip_id="] = "/";         // vimeo
    m["/embed/"]                 = "/";         // redtube

    for (mapstr::const_iterator iter = m.begin();
        iter != m.end();
        ++iter)
    {
        Util::subStrReplace(url, iter->first, iter->second);
    }

    return url;
}

std::string&
Util::lastfmToYoutube(std::string& url) {
    const std::string::size_type pos = url.find("+1-");
    if (pos != std::string::npos) {
        const std::string id = url.substr(pos+3);
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

typedef std::vector<std::string> STRV;

STRV
Util::tokenize(const std::string& src, const std::string& delims) {
    std::string::size_type last = src.find_first_not_of(delims);
    std::string::size_type pos  = src.find_first_of(delims, last);

    STRV v;

    while (std::string::npos != pos || std::string::npos != last) {
        v.push_back( src.substr(last, pos-last) );
        last = src.find_first_not_of(delims, pos);
        pos  = src.find_first_of(delims, last);
    }
    return v;
}

std::string
Util::parseFormatMap(const std::string& host) {

    const Options opts =
        optsmgr.getOptions();

    std::string fmt;

    if (opts.format_map_given) {
        pcrecpp::RE re(host + ":(.*?)(\\||$)");
        re.PartialMatch(opts.format_map_arg, &fmt);
    }

    if (opts.format_given) // Override.
        fmt = opts.format_arg;

    if (fmt.empty())
        fmt = "flv";

    return fmt;
}

const std::string&
Util::toUnicode(std::string& src, const std::string& from) {
#ifdef HAVE_ICONV
    const std::string to = "UTF-8";

    // Try with TRANSLIT first.
    iconv_t cd =
        iconv_open( to.c_str(), std::string(from+"//TRANSLIT").c_str() );

    if (cd == (iconv_t)-1) // Then without TRANSLIT.
        cd = iconv_open( to.c_str(), from.c_str());

    if (cd == (iconv_t)-1) {
        if (errno == EINVAL) {
            logmgr.cerr()
                << "conversion from \""
                << from
                << "\" to \""
                << to
                << "\" unavailable"
                << std::endl;
        }
        else {
#ifdef HAVE_STRERROR
            logmgr.cerr()
                << "iconv_open: "
                << strerror(errno)
                << std::endl;
#else
            perror("iconv_open");
#endif
            return src;
        }
    }

    char inbuf[1024];
    ICONV_CONST char *inptr = inbuf;
    size_t insize = src.length();

    if (insize >= sizeof(inbuf))
        insize = sizeof(inbuf);

    snprintf(inbuf, sizeof(inbuf),
        "%s", src.c_str());

    char outbuf[1024];
    size_t avail = sizeof(outbuf);
    char *wptr   = (char *)outbuf;
    memset(wptr, 0, sizeof(outbuf));

    const size_t rc =
        iconv(cd, &inptr, &insize, &wptr, &avail);

    iconv_close(cd);
    cd = 0;

    if (rc == (size_t)-1) {
        logmgr.cerr()
            << "error: converting characters from \""
            << from
            << "\" to \""
            << to
            << "\" failed"
            << std::endl;
    }
    else
        src = outbuf;
#endif
    return src;
}

const std::string&
Util::fromHtmlEntities(std::string& src) {

    typedef std::map<std::string,std::string> maps;

    maps m;
    m["&quot;"] = "\"";
    m["&#34;"]  = "\"";

    m["&amp;"]  = "&";
    m["&#38;"]  = "&";

    m["&apos;"] = "'";
    m["&#39;"]  = "'";

    m["&lt;"]   = "<";
    m["&#60;"]  = "<";

    m["&gt;"]   = ">";
    m["&#62;"]  = ">";

    for (maps::const_iterator iter = m.begin();
        iter != m.end();
        ++iter)
    {
        Util::subStrReplace(src,
            iter->first, iter->second);
    }

    return src;
}


