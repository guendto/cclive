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
    std::string id;
    partialMatch("(?i)docid:'(.*?)'", &id);
    props.setId(id);
}

void
GoogleHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)<div class=titlebar-title>(.*?)</", &title);
    props.setTitle(title);
}

void
GoogleHandler::parseLink() {

    std::string lnk;
    partialMatch("(?i)videourl\\W+x3d(.*?)\\W+x26", &lnk);

    std::string fmt = optsmgr.getOptions().format_arg;

    if (fmt == "mp4" || fmt == "best") {
        try   {
            partialMatch("(?i)href=(.*?)=ck1", &lnk);
            lnk += "=ck1";
        }
        catch (ParseException x) { /* default to flv */ }
    }
    props.setLink(lnk);
}


