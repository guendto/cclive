/*
 * Copyright (C) 2009 Toni Gundogdu.
 * Copyright (C) 2009 Patrick Hoffmann <patrick@sobran.de>.
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

FunnyhubHandler::FunnyhubHandler()
    : HostHandler()
{
    props.setHost   ("funnyhub");
    props.setDomain ("funnyhub.com");
    props.setFormats("flv");
}

void
FunnyhubHandler::parseId() {
    std::string id;
    partialMatch("(?i)\\/videofiles\\/(.*?)\\_", &id);
    props.setId(id);
}

void
FunnyhubHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)yahoobuzzarticleheadline = \"(.*?)\"", &title);
    props.setTitle(title);
}

void
FunnyhubHandler::parseLink() {
    std::string lnk;
    partialMatch("(?i)\"flv\", \"(.*?)\"", &lnk);
    curlmgr.escape(lnk);
    props.setLink(lnk);
}


