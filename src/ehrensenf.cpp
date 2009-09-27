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

#include <sstream>
#include <iomanip>

#include "hosthandler.h"
#include "macros.h"

EhrensenfHandler::EhrensenfHandler()
    : HostHandler()
{
    props.setHost   ("ehrensenf");
    props.setDomain ("ehrensenf.de");
    props.setFormats("flv");
}

void
EhrensenfHandler::parseId() {
    std::string data;
    partialMatch("(?i)<h2 class=\"gradient\">(.*?)</", &data);

    const char re[] = "(\\d\\d)\\.(\\d\\d)\\.(\\d\\d\\d\\d)";

    int d,m,y;
    if (!pcrecpp::RE(re).PartialMatch(pageContent, &d, &m, &y))
        throw HostHandler::ParseException("no match: " +std::string(re));

    std::stringstream b;
    b << y << "-" << zeropad(2,m) << "-" << zeropad(2,d);

    props.setId( b.str() );
}

void
EhrensenfHandler::parseTitle() {
    props.setTitle( props.getId() );
}

void
EhrensenfHandler::parseLink() {
    std::string lnk = 
        "http://www.ehrensenf.de/misc/load-balancing/lb.php?file="
            + props.getId() + ".flv";
    curlmgr.escape(lnk);
    props.setLink(lnk);
}


