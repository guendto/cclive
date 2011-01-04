/* 
* Copyright (C) 2010 Toni Gundogdu.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"

#include <cerrno>
#include <climits>
#include <cstring>
#include <iostream>

#include "cclive/error.h"

namespace cclive {

std::string
perror (const std::string& p/*=""*/) {

#if defined (HAVE_STRERROR) || defined (HAVE_STRERROR_R)
    std::string s;
 #ifdef HAVE_STRERROR_R
    char buf[256];
    s = strerror_r (errno, buf, sizeof(buf));
 #else
    s = strerror (errno);
 #endif
    return s;
#else // No strerror or strerror_r.
    perror (p.c_str ());
#endif

}

} // End namespace.

// vim: set ts=4 sw=4 tw=72 expandtab:
