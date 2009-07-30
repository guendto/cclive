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

#include "hosthandler.h"
#include "opts.h"

GoogleHandler::GoogleHandler()
    : HostHandler()
{
    props.setHost   ("google");
    props.setDomain ("video.google.");
    props.setFormats("flv|mp4");
}

void
GoogleHandler::parseId() {
    props.setId( Util::subStr(pageContent, "docid:'", "'") );
}

void
GoogleHandler::parseLink() {
    props.setLink( Util::subStr(pageContent, "videoUrl\\x3d", "\\x26") );

    std::string fmt = optsmgr.getOptions().format_arg;

    if (fmt == "mp4" || fmt == "best") {
        const char *end_mp4 = "=ck1";
        try   {
            props.setLink(
                Util::rsubStr(pageContent, "href=", end_mp4) +end_mp4);
        }
        catch (ParseException x) { /* default to flv */ }
    }
}
