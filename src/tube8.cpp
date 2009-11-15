/*
 * Copyright (C) 2009 anonymous contributor.
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

Tube8Handler::Tube8Handler()
    : HostHandler()
{
    props.setHost   ("tube8");
    props.setDomain ("tube8.com");
    props.setFormats("flv");
}

void
Tube8Handler::parseId() {
    std::string id;
    partialMatch("addToFavourites\\(0, (\\d+)\\)", &id);
    props.setId(id);
}

void
Tube8Handler::parseTitle() {
    std::string title;
    partialMatch("<strong>Title</strong>: (.*?)</td>", &title);
    props.setTitle(title);
}

void
Tube8Handler::parseLink() {
    std::string lnk;
    partialMatch("videoUrl=(http://.*?)&", &lnk);
    props.setLink(lnk);
}

