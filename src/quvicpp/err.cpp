/*
* Copyright (C) 2010  Toni Gundogdu <legatvs@gmail.com>
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

namespace quvicpp
{

// Constructor.

error::error (quvi_t q, QUVIcode c)
  : _quvi_code (QUVI_OK), _resp_code (0)
{
  // Friend of quvicpp::error class -> clean API.
  _what = quvi_strerror (q, c);
  quvi_getinfo (q, QUVIINFO_HTTPCODE, &_resp_code);
}

// Copy constructor.

error::error (const error& e)
  : _quvi_code (QUVI_OK), _resp_code (0)
{
  _swap(e);
}

// Copy assignment operator.

error&
error::operator=(const error& e)
{
  if (this != &e)
    _swap(e);
  return *this;
}

// Destructor.
error::~error () { }

// Swap.

void
error::_swap (const error& e)
{
  _quvi_code = e._quvi_code;
  _resp_code = e._resp_code;
  _what      = e._what;
}

// Get.

const std::string&
error::what () const
{
  return _what;
}

long
error::response_code () const
{
  return _resp_code;
}

QUVIcode
error::quvi_code () const
{
  return _quvi_code;
}

} // End namespace.

// vim: set ts=2 sw=2 tw=72 expandtab:
