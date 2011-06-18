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

#include <sstream>
#include <iomanip>

#include <quvicpp/quvicpp.h>

namespace quvicpp
{

// Constructors.

media::media()
  : _current_url( _urls.begin() ), _http_code(-1)
{ }

media::media(quvi_media_t qv)
  : _current_url( _urls.begin() ), _http_code(-1)
{
#define _wrap(id,dst,type) \
    do { \
        type tmp; \
        quvi_getprop(qv,id,&tmp); \
        dst = tmp; \
    } while (0)
  _wrap(QUVIPROP_HOSTID,      _host,  char*);
  _wrap(QUVIPROP_PAGEURL,     _url,   char*);
  _wrap(QUVIPROP_PAGETITLE,   _title, char*);
  _wrap(QUVIPROP_MEDIAID,     _id,    char*);
  _wrap(QUVIPROP_FORMAT,      _format, char*);
#undef _wrap

  do
    {
      _urls.push_back( quvicpp::url(qv) );
    }
  while (quvi_next_media_url(qv) == QUVI_OK);

  _current_url = _urls.begin();
}

// Copy constructor.

media::media(const media& v)
  : _current_url( _urls.begin() ), _http_code(-1)
{
  _swap(v);
}

// Copy assignment operator.

media& media::operator=(const media& v)
{
  if (this != &v)
    _swap(v);
  return *this;
}

// Destructor.

media::~media() { }

// Swap.

void media::_swap(const media& v)
{
  _urls        = v._urls;
  _title        = v._title;
  _host         = v._host;
  _url          = v._url;
  _id           = v._id;
  _format       = v._format;
  _http_code    = v._http_code;
  _current_url = _urls.begin();
}

// Get.

const std::string& media::title() const
{
  return _title;
}

const std::string& media::host() const
{
  return _host;
}

const std::string& media::url() const
{
  return _url;
}

const std::string& media::id() const
{
  return _id;
}

const std::string& media::format() const
{
  return _format;
}

long  media::http_code() const
{
  return _http_code;
}

// Next URL.

quvicpp::url media::next_url()
{
  if (_current_url == _urls.end())
    {
      _current_url = _urls.begin();
      return quvicpp::url();
    }
  return *(_current_url)++;
}

// To string. Mimic quvi(1) behaviour.

std::string media::to_s()
{
  std::stringstream b;

  b.setf(std::ios::fixed);

  b << "title\t: "    << _title    << "\n"
    << "host\t: "     << _host     << "\n"
    << "url\t: "      << _url      << "\n"
    << "id\t: "       << _id       << "\n"
    << "format\t: "   << _format   << "\n"
    << "httpcode: "   << _http_code << "\n";

  for (int i=0;; ++i)
    {
      const quvicpp::url u = next_url();

      if (!u.ok()) break;

      b << "url #"
        << i
        << "\t: "
        << u.media_url()
        << "\n:: length\t: "
        << std::setprecision(0)
        << u.content_length()
        << "\n:: content-type\t: "
        << u.content_type()
        << "\n:: suffix\t: "
        << u.suffix()
        << "\n";
    }

  return b.str();
}

std::ostream& operator<<(std::ostream& os, media& v)
{
  return os << v.to_s();
}

void media::print(std::ostream& os)
{
  os << to_s();
}

} // namespace quvicpp

// vim: set ts=2 sw=2 tw=72 expandtab:
