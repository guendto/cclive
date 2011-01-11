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

// Constructors.

link::link ()
  : _length(-1)
{ }

link::link (quvi_video_t qv)
  : _length(-1)
{
#define _wrap(id,dst,type) \
    do { \
        type tmp; \
        quvi_getprop(qv,id,&tmp); \
        dst = tmp; \
    } while (0)
  _wrap(QUVIPROP_VIDEOURL,              _url,         char*);
  _wrap(QUVIPROP_VIDEOFILECONTENTTYPE,  _contentType, char*);
  _wrap(QUVIPROP_VIDEOFILESUFFIX,       _suffix,      char*);
  _wrap(QUVIPROP_VIDEOFILELENGTH,       _length,      double);
#undef _wrap
}

// Copy constructor.

link::link (const link& l)
  : _length(-1)
{
  _swap(l);
}

// Copy assignment operator.

link&
link::operator=(const link& l)
{
  if (this != &l)
    _swap(l);
  return *this;
}

// Destructor.

link::~link () { }

// Swap.

void
link::_swap (const link& l)
{
  _contentType = l._contentType;
  _suffix      = l._suffix;
  _url         = l._url;
  _length      = l._length;
}

// Get.

const std::string&
link::content_type () const
{
  return _contentType;
}

const std::string&
link::suffix () const
{
  return _suffix;
}

const std::string&
link::url () const
{
  return _url;
}

double
link::length () const
{
  return _length;
}

bool
link::ok () const
{
  return _length > -1;
}

} // End namespace

// vim: set ts=2 sw=2 tw=72 expandtab:
