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

#include "error.h"
#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "cmdline.h"
#include "opts.h"
#include "hosthandler.h"

GoogleHandler::GoogleHandler()
    : HostHandler()
{
    props.setHost   ("google");
    props.setDomain ("video.google.");
    props.setFormats("flv|mp4");
}

const bool
GoogleHandler::isHost(std::string url) {
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

    if (!strcmp(optsmgr.getOptions().format_arg, "mp4")
        || !strcmp(optsmgr.getOptions().format_arg, "best"))
    {
        const char *begin_mp4 = "href=";
        const char *end_mp4   = "=ck1";
        try   {
            props.setLink(
                Util::rsubStr(pageContent, begin_mp4, end_mp4) +end_mp4);
        }
        catch (ParseException x) { /* default to flv */ }
    }
}
