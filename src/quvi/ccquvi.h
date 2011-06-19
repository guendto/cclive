/* cclive
 * Copyright (C) 2010-2011  Toni Gundogdu <legatvs@gmail.com>
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

#ifndef cclive_ccquvi_h
#define cclive_ccquvi_h

#include <string>
#include <vector>
#include <map>

#include <quvi/quvi.h>

namespace quvi
{

class options;
class query;
class media;
class error;
class url;

// Version.

std::string version();
std::string version_long();

// To string.

std::string support_to_s(const std::map<std::string,std::string>&);

// Options.

class options
{
  friend class query;
  friend class error;
public:
  options();
  options(const options&);
  options& operator=(const options&);
  virtual ~options();
public:
  void statusfunc(quvi_callback_status);
  void format(const std::string&);
  void resolve(const bool);
  void verify(const bool);
private:
  void _swap(const options&);
private:
  quvi_callback_status _statusfunc;
  std::string _format;
  bool _resolve;
  bool _verify;
};

// Query.

class query
{
public:
  query();
  query(const query&);
  query& operator=(const query&);
  virtual ~query();
public:
  std::string formats(const std::string& url, const options&) const;
  media parse(const std::string& url, const options&) const;
  std::map<std::string,std::string> support() const;
  void* curlHandle() const;
private:
  void _set_opts(const options&) const;
  void _init();
  void _close();
private:
  quvi_t _quvi;
  void*_curl;
};

// Link.

class url
{
public:
  url();
  url(quvi_media_t);
  url(const url&);
  url& operator=(const url&);
  virtual ~url();
public:
  const std::string& content_type() const;
  const std::string& media_url() const;
  const std::string& suffix() const;
  double content_length() const;
  bool ok() const;
private:
  void _swap(const url&);
private:
  std::string _contentType;
  double _contentLength;
  std::string _suffix;
  std::string _url;
};

// Media.

class media
{
  friend std::ostream& operator<<(std::ostream&, const media&);
public:
  media();
  media(quvi_media_t);
  media(const media&);
  media& operator=(const media&);
  virtual ~media();
public:
  const std::string& title() const;
  const std::string& host() const;
  const std::string& url() const;
  const std::string& id() const;
  const std::string& format() const;
  void print(std::ostream&);
  long http_code() const;
  quvi::url next_url();
  std::string to_s();
private:
  void _swap(const media&);
private:
  std::vector<quvi::url>::const_iterator _current_url;
  std::vector<quvi::url> _urls;
  std::string _format;
  std::string _title;
  std::string _host;
  std::string _url;
  std::string _id;
  long _http_code;
};

// Error.

class error
{
public:
  error(quvi_t, QUVIcode);
  error(const error&);
  error& operator=(const error&);
  virtual ~error();
public:
  const std::string& what() const;
  long response_code() const;
  QUVIcode quvi_code() const;
private:
  void _swap(const error&);
private:
  QUVIcode _quvi_code;
  std::string _what;
  long _resp_code;
};

} // namespace quvi

#endif // cclive_quvi_h

// vim: set ts=2 sw=2 tw=72 expandtab:
