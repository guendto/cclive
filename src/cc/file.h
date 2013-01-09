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
  file();
  file(const quvi::media&, const quvi::url&, const int);
  file(const file&);
  file& operator=(const file&);
public:
  bool write(const quvi::query&, const quvi::url&) const;
public:
  std::string to_s(const quvi::url&) const;
  const std::string& title() const;
  const std::string& path() const;
  const std::string& name() const;
  const bool nothing_todo() const;
  double initial_length() const;
public:
  static double exists(const std::string&);
private:
  void _init(const quvi::media&,
             const quvi::url&,
             const int);
  bool _should_continue() const;
  void _swap(const file&);
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
