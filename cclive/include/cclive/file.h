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

#ifndef cclive_file_h
#define cclive_file_h

namespace cclive
{

class file
{
public:
  file ();
  file (const quvicpp::video&, const quvicpp::link&,
        const int, const options&);
  file (const file&);
  file& operator=(const file&);
public:
  bool write (const quvicpp::query&, const quvicpp::link&,
              const options&) const;
public:
  double             initial_length   () const;
  const std::string& path             () const;
  std::string        to_s             (const quvicpp::link&) const;
public:
  static double exists (const std::string&);
private:
  void _swap              (const file&);
  void _init              (const quvicpp::video&, const quvicpp::link&,
                           const int, const options&);
  bool _should_continue   () const;
private:
  double _initial_length;
  std::string _name;
  std::string _path;
};

} // End namespace.

#endif

// vim: set ts=2 sw=2 tw=72 expandtab:
