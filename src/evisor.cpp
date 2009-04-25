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

#include <string>
#include <vector>

#include "except.h"
#include "video.h"
#include "util.h"
#include "hosthandler.h"

const bool
EvisorHandler::isHost(std::string url) {
    props.setHost   ("evisor");
    props.setDomain ("evisor.tv");
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
EvisorHandler::parseId() {
    const char *begin = "unit_long";
    const char *end   = "\"";
    props.setId( Util::subStr(pageContent, begin, end) );
}

void
EvisorHandler::parseLink() {
    const char *begin = "file=";
    const char *end   = "\"";
    props.setLink( Util::subStr(pageContent, begin, end) );
}
