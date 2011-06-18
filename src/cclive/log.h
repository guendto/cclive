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

#ifndef cclive_log_h
#define cclive_log_h

#include <iostream>
#include <fstream>

#include <boost/iostreams/filtering_stream.hpp>

namespace cclive
{

extern boost::iostreams::filtering_ostream log;

struct omit_sink : public boost::iostreams::sink
{
  std::streamsize write(const char *s, std::streamsize n);
  explicit omit_sink(bool b=false);
private:
  bool _omit;
};

struct flushable_file_sink
{
  typedef char char_type;

  struct category :
    boost::iostreams::output_seekable,
    boost::iostreams::device_tag,
    boost::iostreams::closable_tag,
    boost::iostreams::flushable_tag {};

  flushable_file_sink(
    const std::string&,
    const std::ios_base::openmode mode = std::ios::trunc|std::ios::out);

  flushable_file_sink& operator=(const flushable_file_sink&);
  flushable_file_sink(const flushable_file_sink&);

  std::streampos seek(std::streamoff, std::ios_base::seekdir);
  std::streamsize write(const char *, std::streamsize);
  std::streamsize read(char_type *, std::streamsize);

  bool is_open() const;
  bool flush();
  void close();
private:
  void _swap(const flushable_file_sink&);
  void _open();
private:
  std::ios_base::openmode _mode;
  mutable std::fstream _f;
  std::string _fpath;
};

} // namespace cclive

#endif

// vim: set ts=2 sw=2 tw=72 expandtab:
