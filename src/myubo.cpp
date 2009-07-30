/*
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

MyuboHandler::MyuboHandler()
    : HostHandler()
{
    props.setHost   ("myubo");
    props.setDomain ("myubo.com");
    props.setFormats("flv");
}

#include <iostream>
void
MyuboHandler::parseId() {
    const char *begin = "movieid=";
    const char *end   = "\"";
    props.setId( Util::subStr(pageContent, begin, end) );
}

void
MyuboHandler::parseLink() {

    // myubo uses a static <title>. Grab video title elsewhere.
    props.setTitle(
        Util::subStr(pageContent,
            "<div id=\"movieDetail\"><h1>",
            "</h1>")
    );

    const char *begin = "writeFlashPlayer(\'";
    const char *end   = "\'";

    props.setLink( Util::subStr(pageContent, begin, end) );
}
