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

#ifndef compat_options_h
#define compat_options_h

namespace quvi {

class options
{
  inline void _copy(const options& a)
  {
    statusfunc = a.statusfunc;
    useragent  = a.useragent;
    resolve    = a.resolve;
    stream     = a.stream;
  }
public:
  inline options(const options& a):statusfunc(NULL), resolve(true) { _copy(a); }
  inline options():statusfunc(NULL), resolve(true)                 { }
  inline options& operator=(const options& a)
  {
    if (this != &a)
      _copy(a);
    return *this;
  }
  inline virtual ~options() { statusfunc=NULL; }
public:
  quvi_callback_status statusfunc;
  std::string useragent;
  std::string stream;
  bool resolve;
};

} // namespace quvi

#endif // compat_options_h

// vim: set ts=2 sw=2 tw=72 expandtab:
