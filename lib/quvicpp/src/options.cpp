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

#include <cassert>
#include <iostream>

#include <quvicpp/quvicpp.h>

namespace quvicpp {

options::options ()
    : _statusfunc(NULL),
      _writefunc(NULL),
      _format("default"),
      _verify(true),
      _shortened(true)
    { }

// Copy constructor.

options::options (const options& opts)
    : _statusfunc(NULL),
      _writefunc(NULL),
      _format("default"),
      _verify(true),
      _shortened(true)
    { _swap(opts); }

// Copy assignment operator.

options&
options::operator=(const options& qo) {
    if (this != &qo)
        _swap(qo);
    return *this;
}

// Destructor.

options::~options () {
    _statusfunc = NULL;
    _writefunc  = NULL;
}

// Swap.

void
options::_swap (const options& qo) {
    _format     = qo._format;
    _verify     = qo._verify;
    _shortened  = qo._shortened;
    _statusfunc = qo._statusfunc;
    _writefunc  = qo._writefunc;
}

// Set.

void options::format (const std::string& fmt)      { _format     = fmt; }
void options::verify (const bool b)                { _verify     = b; }
void options::shortened (const bool b)             { _shortened  = b; }
void options::statusfunc (quvi_callback_status cb) { _statusfunc = cb; }
void options::writefunc (quvi_callback_write cb)   { _writefunc  = cb; }

} // End namespace.


