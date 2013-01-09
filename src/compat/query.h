/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
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

#ifndef compat_query_h
#define compat_query_h

namespace quvi {

class query_base
{
  virtual std::string streams(const std::string&, const quvi::options&) const = 0;
  virtual media parse(const std::string&, const quvi::options&) const = 0;
  virtual std::map<std::string,std::string> support() const = 0;
  virtual void _configure(const quvi::options&) const = 0;
  virtual void setup_curl() const = 0;
  virtual void _close() = 0;
  virtual void _init() = 0;
protected:
  inline virtual ~query_base() { }
};

class query_impl : query_base
{
protected:
  inline query_impl(const query_impl&): _quvi(0)  {}
  inline query_impl(): _quvi(0)                   {}
  inline query_impl& operator=(const query_impl&) { return *this; }
  inline virtual ~query_impl()                    {}
protected:
  quvi_t _quvi;
};

class query_pt4 : public query_impl
{
  void _configure(const quvi::options&) const;
  void _close();
  void _init();
public:
  inline query_pt4(const query_pt4& a): query_impl(a) { _init(); }
  inline query_pt4(): query_impl()                    { _init(); }
  inline virtual ~query_pt4()                         { _close(); }
  inline query_pt4& operator=(const query_pt4& a)
  {
    if (this != &a)
      {
        _close();
        _init();
      }
    return *this;
  }
public:
  std::string streams(const std::string&, const quvi::options&) const;
  media parse(const std::string&, const quvi::options&) const;
  std::map<std::string,std::string> support() const;
  void setup_curl() const;
};

typedef class query_pt4 query;

} // namespace quvi

#endif // compat_query_h

// vim: set ts=2 sw=2 tw=72 expandtab:
