/*
 * Copyright (C) 2009,2010 Toni Gundogdu.
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
#include <sstream>

#include <pcrecpp.h>

#include "opts.h"
#include "except.h"
#include "log.h"
#include "quvi.h"
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

static bool
verify_format (const std::string& url, const std::string& format) {

    pcrecpp::RE_Options re_opts;
    re_opts.set_caseless(true);

    std::stringstream pattern;
    pattern << "(?:\\||^)" << format << "(?:\\||$)";

    pcrecpp::RE re(pattern.str(), re_opts);
    const quvi_t quvi = quvimgr.handle();

    // Note: We need to go over the entire list, otherwise
    // the next time we use quvi_next_supported_website will
    // continue from where we left.

    bool done = false;
    bool m = false;

    while (!done) {

        char *d=NULL, *f=NULL;
        const QUVIcode rc = quvi_next_supported_website(quvi, &d, &f);

        switch (rc) {

        case QUVI_OK:

            if (url.find(d) != std::string::npos)
                m = re.PartialMatch(f);

            quvi_free(d);
            quvi_free(f);

            break;

        case QUVI_LAST:
            done = true;
            break;

        default:
            throw RuntimeException (CCLIVE_QUVI, quvi_strerror(quvi,rc));
        }
    }

    return m;
}

std::string
Util::parseFormatMap (const std::string& url) {

    static const char unsupported_m[] =
        "\n  >> Website does not support the specified format. "
        "See `--hosts' output.";

    const Options opts = optsmgr.getOptions();
    std::string fmt    = "default";

    if (opts.format_map_given) {

        // Match URL to --format-map=$domain:$format
        // NOTE: Use full domain names for better results.

        pcrecpp::StringPiece in(opts.format_map_arg);
        pcrecpp::RE re("(\\w+):([\\w-_]+)");
        std::string domain, format;

        while (re.FindAndConsume(&in, &domain, &format)) {

            if (url.find(domain) != std::string::npos) {

                if (verify_format(url, format)) {
                    fmt = format;
                    break;
                }

                logmgr.cerr()
                    << "  > Warning: ignoring `--format-map="
                    << "(" << domain << ":" << format << ")'"
                    << " for " << url
                    << unsupported_m
                    << std::endl;
            }
        }
    }

    if (opts.format_given) { // Overrides.

        if (!verify_format(url, opts.format_arg)) {
            logmgr.cerr()
                << "  > Warning: ignoring `--format="
                << opts.format_arg << "' for " << url
                << unsupported_m
                << std::endl;
        }
        else
            fmt = opts.format_arg;
    }

    return fmt;
}
 
const std::string&
Util::fromHtmlEntities(std::string& src) {

    typedef std::map<std::string,std::string> maps;

    maps m;
    m["&quot;"] = "\"";
    m["&amp;"]  = "&";
    m["&apos;"] = "'";
    m["&lt;"]   = "<";
    m["&gt;"]   = ">";

    for (maps::const_iterator iter = m.begin();
        iter != m.end();
        ++iter)
    {
        Util::subStrReplace(src,
            iter->first, iter->second);
    }

    return src;
}

const bool
Util::perlSubstitute(const std::string& re, std::string& src) {
    std::string pat, sub, flags;
    if (pcrecpp::RE("^s\\/(.*)\\/(.*)\\/(.*)$", pcrecpp::UTF8())
        .PartialMatch(re, &pat, &sub, &flags))
    {
        if (src.empty()) // test "re" only.
            return true;

        pcrecpp::RE_Options opts;

        opts.set_caseless(strstr(flags.c_str(), "i") != 0);
        opts.set_utf8(true);

        pcrecpp::RE subs(pat, opts);

        (strstr(flags.c_str(), "g"))
            ? subs.GlobalReplace(sub, &src)
            : subs.Replace(sub, &src);

        return true;
    }
    return false;
}
 
const bool
Util::perlMatch(const std::string& re, std::string& src) {
    std::string pat, flags;
    if (pcrecpp::RE("^\\/(.*)\\/(.*)$", pcrecpp::UTF8())
        .PartialMatch(re, &pat, &flags))
    {
        if (src.empty())
            return true;

        pcrecpp::RE_Options opts;

        opts.set_caseless(strstr(flags.c_str(), "i") != 0);
        opts.set_utf8(true);

        if (strstr(flags.c_str(), "g") != 0) {
            pcrecpp::StringPiece sp(src);
            pcrecpp::RE re(pat, opts);

            src.clear();

            std::string s;
            while (re.FindAndConsume(&sp, &s))
                src += s;
        }
        else {
            std::string tmp = src;
            src.clear();

            pcrecpp::RE(pat, opts).PartialMatch(tmp, &src);
        }
        return true;
    }
    return false;
}


