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

#include <sstream>
#include <iomanip>

#include <quvicpp/quvicpp.h>

namespace quvicpp {

// Constructors.

video::video ()
    : _current_link( _links.begin() ), _http_code(-1)
    { }

video::video (quvi_video_t qv)
    : _current_link( _links.begin() ), _http_code(-1)
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
    _wrap(QUVIPROP_VIDEOID,     _id,    char*);
    _wrap(QUVIPROP_VIDEOFORMAT, _format, char*);
    _wrap(QUVIPROP_HTTPCODE,    _http_code, long);
#undef _wrap

    do    { _links.push_back( link(qv) ); } 
    while (quvi_next_videolink(qv) == QUVI_OK);

    _current_link = _links.begin();
}

// Copy constructor.

video::video (const video& v)
    : _current_link( _links.begin() ), _http_code(-1)
    { _swap(v); }

// Copy assignment operator.

video&
video::operator=(const video& v) {
    if (this != &v)
        _swap(v);
    return *this;
}

// Destructor.

video::~video () { }

// Swap.

void
video::_swap (const video& v) {
    _links       = v._links;
    _title       = v._title;
    _host        = v._host;
    _url         = v._url;
    _id          = v._id;
    _format      = v._format;
    _http_code    = v._http_code;
    _current_link = _links.begin();
}

// Get.

const std::string& video::title () const { return _title; }
const std::string& video::host  () const { return _host; }
const std::string& video::url   () const { return _url; }
const std::string& video::id    () const { return _id; }
const std::string& video::format() const { return _format; }
long  video::http_code          () const { return _http_code; }

// Next link.

link
video::next_link () {
    if (_current_link == _links.end()) {
        _current_link = _links.begin();
        return link();
    }
    return *(_current_link)++;
}

// To string. Mimic quvi(1) behaviour.

std::string
video::to_s () {

    std::stringstream b;

    b.setf(std::ios::fixed);

    b << "title\t: "    << _title    << "\n"
      << "host\t: "     << _host     << "\n"
      << "url\t: "      << _url      << "\n"
      << "id\t: "       << _id       << "\n"
      << "format\t: "   << _format   << "\n"
      << "httpcode: "   << _http_code << "\n";

    for (int i=0;; ++i) {

        const link l = next_link();

        if (!l.ok()) break;

        b << "link #"
          << i
          << "\t: "
          << l.url()
          << "\n:: length\t: " 
          << std::setprecision(0)
          << l.length()
          << "\n:: content-type\t: "
          << l.content_type()
          << "\n:: suffix\t: "
          << l.suffix()
          << "\n";
    }

    return b.str();
}

std::ostream& operator<<(std::ostream& os, video& v)
    { return os << v.to_s(); }

void
video::print (std::ostream& os)
    { os << to_s(); }

} // End namespace.

// vim: set ts=4 sw=4 tw=72 expandtab:
