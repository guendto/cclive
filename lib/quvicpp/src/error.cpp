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

#include <quvicpp/quvicpp.h>

namespace quvicpp {

// Constructor.

error::error (quvi_t q, QUVIcode c)
    // Friend of quvicpp::error class -> clean API.
    { _what = quvi_strerror(q,c); }

// Copy constructor.

error::error (const error& e)
    { _swap(e); }

// Copy assignment operator.

error&
error::operator=(const error& e) {
    if (this != &e)
        _swap(e);
    return *this;
}

// Destructor.
error::~error () { }

// Swap.

void
error::_swap (const error& e)
    { _what = e._what; }

// Get.

const std::string&
error::what () const
    { return _what; }

} // End namespace.

