/*
 * Copyright (C) 2009 i.am.a.real.coward@gmail.com
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

YoujizzHandler::YoujizzHandler()
    : HostHandler()
{
    props.setHost   ("youjizz");
    props.setDomain ("youjizz.com");
    props.setFormats("flv");
}

void
YoujizzHandler::parseId() {
    std::string id;
    partialMatch("(?i)\\?id=(\\d+)", &id);
    props.setId(id);
}

void
YoujizzHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)<title>(.*?)</title>", &title);
    props.setTitle(title);
}

void
YoujizzHandler::parseLink() {
    std::string lnk;
    partialMatch("(?i)['\"]content_video['\"],.*['\"](.*?)['\"]", &lnk);
    props.setLink(lnk);
}


