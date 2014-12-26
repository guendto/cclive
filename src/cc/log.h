/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
 *
 * This file is part of cclive <http://cclive.sourceforge.net/>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef cc__log_h
#define cc__log_h

#include <ccinternal>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>

#include <ccerror>

namespace cc
{

extern boost::iostreams::filtering_ostream log;

namespace sink
{

struct omit : boost::iostreams::sink
{
  inline std::streamsize write(const char *s, std::streamsize n)
  {
    if (!_omit) std::clog.write(s, n);
    return n;
  }

  inline explicit omit(bool omit=false): _omit(omit) { }

private:
  bool _omit;
};

namespace fs = boost::filesystem;

struct flushable_file : boost::noncopyable
{
  typedef char char_type;

  struct category : boost::noncopyable,
    boost::iostreams::output_seekable,
    boost::iostreams::flushable_tag,
    boost::iostreams::closable_tag,
    boost::iostreams::device_tag { };

#define DEFAULT_MODE  std::ios::trunc|std::ios::out
  inline flushable_file(const std::string& fpath,
                        const std::ios_base::openmode m = DEFAULT_MODE)
    : _mode(m), _fpath(fpath)
  {
    _open();
  }
#undef DEFAULT_MODE

  inline flushable_file(const flushable_file& o) { _copy(o); }

  inline flushable_file& operator=(const flushable_file& o)
  {
    if (this != &o) _copy(o);
    return *this;
  }

  inline std::streampos seek(std::streamoff n, std::ios_base::seekdir s)
  {
    _s.seekp(n, s);
    _s.seekg(n, s);
    return n;
  }

  inline std::streamsize write(const char *s, std::streamsize n)
  {
    _s.write(s, n);
    return n;
  }

  inline std::streamsize read(char_type *t, std::streamsize n)
  {
    _s.read(t, n);
    return n;
  }

  inline bool is_open() const { return _s.is_open(); }

  inline bool flush()
  {
    _s.flush();
    return true;
  }

  inline void close()
  {
    flush();
    _s.close();
  }

private:
  inline void _copy(const flushable_file& o)
  {
    close();
    _fpath = o._fpath;
    _mode = o._mode;
    _open();
  }

  inline void _open()
  {
    _fpath = fs::system_complete(fs::path(_fpath)).string();
    _s.open(_fpath.c_str(), _mode);
    if (_s.fail())
      {
        BOOST_THROW_EXCEPTION(cc::error::fstream()
          << boost::errinfo_file_name(_fpath)
          << boost::errinfo_errno(errno));
      }
  }

private:
  std::ios_base::openmode _mode;
  std::string _fpath;
  std::fstream _s;
};

} // namespace log

} // namespace cc

#endif

// vim: set ts=2 sw=2 tw=72 expandtab:
