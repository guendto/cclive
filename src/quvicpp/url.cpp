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

#include "config.h"

#include <quvicpp/quvicpp.h>

namespace quvicpp
{

// Constructors.

url::url ()
  : _contentLength(-1)
{ }

url::url (quvi_media_t qv)
  : _contentLength(-1)
{
#define _wrap(id,dst,type) \
    do { \
        type tmp; \
        quvi_getprop(qv,id,&tmp); \
        dst = tmp; \
    } while (0)
  _wrap(QUVIPROP_MEDIAURL,           _url,           char*);
  _wrap(QUVIPROP_MEDIACONTENTTYPE,   _contentType,   char*);
  _wrap(QUVIPROP_MEDIACONTENTLENGTH, _contentLength, double);
  _wrap(QUVIPROP_FILESUFFIX,         _suffix,        char*);
#undef _wrap
}

// Copy constructor.

url::url (const url& u)
  : _contentLength(-1)
{
  _swap(u);
}

// Copy assignment operator.

url&
url::operator=(const url& u)
{
  if (this != &u)
    _swap(u);
  return *this;
}

// Destructor.

url::~url () { }

// Swap.

void
url::_swap (const url& u)
{
  _contentLength = u._contentLength;
  _contentType   = u._contentType;
  _suffix        = u._suffix;
  _url           = u._url;
}

// Get.

const  std::string& url::content_type () const
{
  return _contentType;
}
const  std::string& url::suffix () const
{
  return _suffix;
}
const  std::string& url::media_url () const
{
  return _url;
}
double              url::content_length () const
{
  return _contentLength;
}
bool                url::ok () const
{
  return _contentLength > -1;
}

} // End namespace

// vim: set ts=2 sw=2 tw=72 expandtab:
