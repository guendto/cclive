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

BreakHandler::BreakHandler()
    : HostHandler()
{
    props.setHost   ("break");
    props.setDomain ("break.com");
    props.setFormats("flv");
}

void
BreakHandler::parseId() {
    props.setId( Util::subStr(pageContent, "ContentID='", "'") );
}

void
BreakHandler::parseTitle() {
    props.setTitle(
        Util::subStr(pageContent, "id=\"vid_title\" content=\"", "\""));
}

void
BreakHandler::parseLink() {
    std::string fpath =
        Util::subStr(pageContent, "ContentFilePath='", "'");

    std::string fname =
        Util::subStr(pageContent, "FileName='", "'");

    std::string lnk =
        "http://media1.break.com/dnet/media/" + fpath + "/" + fname + ".flv";

    props.setLink( lnk );
}
