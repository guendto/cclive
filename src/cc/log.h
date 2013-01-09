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

#ifndef cclive_log_h
#define cclive_log_h

#include <iostream>
#include <fstream>

#include <boost/iostreams/filtering_stream.hpp>

#define cc_debug(...)\
  do { cc::_debug(__BASE_FILE__, __func__, __LINE__, __VA_ARGS__); } while (0)

namespace cc
{

void _debug(const std::string&, const std::string&,
            const int, const char*, ...);

extern boost::iostreams::filtering_ostream log;

struct omit_sink : public boost::iostreams::sink
{
  inline std::streamsize write(const char *s, std::streamsize n)
    {
      if (!_omit) std::clog.write(s,n);
      return n;
    }
  inline explicit omit_sink(bool b=false): _omit(b) { }
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

  inline flushable_file_sink(
    const std::string& fpath,
    const std::ios_base::openmode mode = std::ios::trunc|std::ios::out)
      : _mode(mode), _fpath(fpath)
  {
    _open();
  }

  inline flushable_file_sink(const flushable_file_sink& f) { _swap(f); }
  
  inline flushable_file_sink& operator=(const flushable_file_sink& f)
    {
      if (this != &f) _swap(f);
      return *this;
    }

  inline std::streampos seek(std::streamoff o, std::ios_base::seekdir d)
    {
      _f.seekp(o,d);
      _f.seekg(o,d);
      return o;
    }

  inline std::streamsize write(const char *s, std::streamsize n)
    {
      _f.write(s,n);
      return n;
    }

  inline std::streamsize read(char_type *t, std::streamsize n)
    {
      _f.read(t,n);
      return n;
    }

  inline bool is_open() const { return _f.is_open(); }

  inline bool flush()
    {
      _f.flush();
      return true;
    }

  inline void close()
    {
      flush();
      _f.close();
    }
private:
  inline void _swap(const flushable_file_sink& f)
    {
      close();
      _fpath = f._fpath;
      _mode  = f._mode;
      _open();
    }
  void _open();
private:
  std::ios_base::openmode _mode;
  mutable std::fstream _f;
  std::string _fpath;
};

} // namespace cc

#endif // cclive_log_h

// vim: set ts=2 sw=2 tw=72 expandtab:
