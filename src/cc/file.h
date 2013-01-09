/* cclive
 * Copyright (C) 2010-2013  Toni Gundogdu <legatvs@gmail.com>
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

#ifndef cclive_file_h
#define cclive_file_h

namespace cc
{

class file
{
public:
  file(const quvi::media&, const quvi::url&, const int);

  inline file(): _initial_length(0), _nothing_todo(false) { }

  inline file(const file& f): _initial_length(0), _nothing_todo(false)
    {
      _swap(f);
    }

  inline file& operator=(const file& f)
    {
      if (this != &f) _swap(f);
      return *this;
    }
public:
  bool write(const quvi::url&, void*) const;
public:
  std::string to_s(const quvi::url&) const;
  inline const std::string& title() const { return _title; }
  inline const std::string& path() const  { return _path; }
  inline const std::string& name() const  { return _name; }
  inline const bool nothing_todo() const  { return _nothing_todo; }
  inline double initial_length() const    { return _initial_length; }
public:
  static double exists(const std::string&);
private:
  void _init(const quvi::media&, const quvi::url&, const int);

  inline bool _should_continue() const { return _initial_length >0; }

  inline void _swap(const file& f)
    {
      _title          = f._title;
      _name           = f._name;
      _path           = f._path;
      _initial_length = f._initial_length;
      _nothing_todo   = f._nothing_todo;
    }
private:
  double _initial_length;
  bool _nothing_todo;
  std::string _title;
  std::string _name;
  std::string _path;
};

} // namespace cc

#endif // cclive_file_h

// vim: set ts=2 sw=2 tw=72 expandtab:
