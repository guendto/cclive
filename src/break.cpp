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
#include <sstream>
#include <vector>

#include "except.h"
#include "video.h"
#include "util.h"
#include "hosthandler.h"

const bool
BreakHandler::isHost(std::string url) {
    props.setHost("break");
    return Util::toLower(url).find("break.com")
        != std::string::npos;
}

void
BreakHandler::parseId() {
    const char *begin = "ContentID='";
    const char *end   = "'";
    props.setId( Util::subStr(pageContent, begin, end) );
}

void
BreakHandler::parseLink() {
    const char *fpathBegin = "ContentFilePath='";
    const char *fpathEnd   = "'";

    std::string fpath = Util::subStr(pageContent, fpathBegin, fpathEnd);

    const char *fnBegin    = "FileName='";
    const char *fnEnd      = "'";

    std::string fname = Util::subStr(pageContent, fnBegin, fnEnd);

    std::stringstream b;
    b   << "http://media1.break.com/dnet/media/"
        << fpath
        << "/"
        << fname
        << ".flv";

    props.setLink( b.str() );
}
