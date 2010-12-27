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

#ifndef quvicpp_h
#define quvicpp_h

#include <string>
#include <vector>
#include <map>

#include <quvi/quvi.h>

namespace quvicpp {

class options;
class query;
class video;
class link;
class error;

typedef std::string url;

// Version.

std::string version      ();
std::string version_long ();

// To string.

std::string support_to_s (const std::map<std::string,std::string>&);

// Options.

class options {
    friend class query;
    friend class error;
public:
    options();
    options(const options&);
    options& operator=(const options&);
    virtual ~options();
public:
    void format     (const std::string&);
    void verify     (const bool);
    void shortened  (const bool);
    void statusfunc (quvi_callback_status);
    void writefunc  (quvi_callback_write);
private:
    void _swap (const options&);
private:
    quvi_callback_status _statusfunc;
    quvi_callback_write  _writefunc;
    std::string          _format;
    bool                 _verify;
    bool                 _shortened;
};

// Query.

class query {
public:
    query();
    query(const query&);
    query& operator=(const query&);
    virtual ~query();
public:
    video parse (const url&, const options&) const;
    std::map<std::string,std::string> support () const;
    void* curlHandle () const;
private:
    void _init ();
    void _close();
private:
    quvi_t _quvi;
    void*  _curl;
};

// Link.

class link {
public:
    link();
    link(quvi_video_t);
    link(const link&);
    link& operator=(const link&);
    virtual ~link();
public:
    const   std::string& content_type   () const;
    const   std::string& suffix         () const;
    const   std::string& url            () const;
    double  length                      () const;
    bool    ok                          () const;
private:
    void _swap (const link&);
private:
    std::string _contentType;
    std::string _suffix;
    std::string _url;
    double _length;
};

// Video.

class video {
    friend std::ostream& operator<<(std::ostream&, const video&);
public:
    video();
    video(quvi_video_t);
    video(const video&);
    video& operator=(const video&);
    virtual ~video();
public:
    const std::string& title    () const;
    const std::string& host     () const;
    const std::string& url      () const;
    const std::string& id       () const;
    const std::string& format   () const;
    long  http_code             () const;
    link               next_link();
    std::string        to_s     ();
    void print                  (std::ostream&);
private:
    void _swap (const video&);
private:
    std::vector<link>::const_iterator _current_link;
    std::vector<link> _links;
    std::string _format;
    std::string _title;
    std::string _host;
    std::string _url;
    std::string _id;
    long _http_code;
};

// Error.

class error {
public:
    error (quvi_t, QUVIcode);
    error (const error&);
    error& operator=(const error&);
    virtual ~error();
public:
    const std::string& what() const;
    long response_code () const;
    QUVIcode quvi_code () const;
private:
    void _swap (const error&);
private:
    QUVIcode _quvi_code;
    std::string _what;
    long _resp_code;
};

} // End namespace.

#endif


