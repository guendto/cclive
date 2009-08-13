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
#include <cstdlib>
#include <iomanip>

#include "hosthandler.h"
#include "macros.h"

// For well documented comments, see clive project and Redtube.pm file.

RedtubeHandler::RedtubeHandler()
    : HostHandler()
{
    props.setHost   ("redtube");
    props.setDomain ("redtube.com");
    props.setFormats("flv");
}

void
RedtubeHandler::parseId() {
    std::string id;
    partialMatch("(?i)videoid=(.*?)['\"]", &id);
    props.setId(id);
}

void
RedtubeHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)videotitle['\"]>(.*?)</", &title);
    props.setTitle(title);
}

static long
digit(const long& num, const int& digit, const int& len) {
    std::stringstream b;
    b << zeropad(len,num);
    return atol(b.str().substr(digit,1).c_str());
}

typedef std::vector<int> INTV;
typedef unsigned int _uint;

static INTV
unpack_casterisk(const char *str) { // perl: unpack('C*', '...')
    INTV v;
    for (register _uint i=0; str[i]!='\0'; ++i) {
        std::stringstream b;
        b << zeropad(2, static_cast<int>(str[i]));
        v.push_back( atoi(b.str().c_str()) );
    }
    return v;
}

static std::string
pack_casterisk(const INTV& v) { // perl: pack('C', ...)
    std::stringstream b;
    for (INTV::const_iterator iter = v.begin();
        iter != v.end(); ++iter)
    {
        b << static_cast<char>(*iter);
    }
    return b.str();
}

void
RedtubeHandler::parseLink() {

    const long id = atol(props.getId().c_str());

    long var1 = 0;
    register _uint i=0;

    for (; i<7; ++i)
        var1 += (digit(id,i,7) * (i+1));

    std::stringstream b;
    b << var1;

    const size_t n = b.str().length();
    long var2 = 0;

    for (i=0; i<n; ++i)
        var2 += digit(var1, i, n);

    INTV map =
        unpack_casterisk("R15342O7K9HBCDXFGAIJ8LMZ6PQ0STUVWEYN");

    INTV mapping;
    mapping.push_back( map[digit(id,3,7) + var2 + 3] );
    mapping.push_back( 48 + var2 % 10 );
    mapping.push_back( map[digit(id,0,7) + var2 + 2] );
    mapping.push_back( map[digit(id,2,7) + var2 + 1] );
    mapping.push_back( map[digit(id,5,7) + var2 + 6] );
    mapping.push_back( map[digit(id,1,7) + var2 + 5] );
    mapping.push_back( 48 + var2 / 10 );
    mapping.push_back( map[digit(id,4,7) + var2 + 7] );
    mapping.push_back( map[digit(id,6,7) + var2 + 4] );

    std::string tmp = pack_casterisk(mapping);

    std::stringstream lnk;
    lnk << "http://dl.redtube.com/_videos_t4vn23s9jc5498tgj49icfj4678/"
        << zeropad(7,id/1000)
        << "/"
        << tmp
        << ".flv";

    props.setLink( lnk.str() );
}


