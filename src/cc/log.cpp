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

#include <stdexcept>
#include <cerrno>

#include <boost/filesystem.hpp>

#include <ccutil>
#include <cclog>

namespace cc
{

namespace io = boost::iostreams;

io::filtering_ostream log;

omit_sink::omit_sink(bool b/*=false*/) : _omit(b) { }

std::streamsize omit_sink::write(const char *s, std::streamsize n)
{
  if (!_omit) std::clog.write(s,n);
  return n;
}

// Constructor.

flushable_file_sink::flushable_file_sink(
  const std::string& fpath,
  const std::ios_base::openmode mode/*=std::ios::trunc|std::ios::out*/)
  : _mode(mode), _fpath(fpath)
{
  _open();
}

// Copy constructor.

flushable_file_sink::flushable_file_sink(const flushable_file_sink& f)
{
  _swap(f);
}

// Copy assignment operator.

flushable_file_sink&
flushable_file_sink::operator=(const flushable_file_sink& f)
{
  if (this != &f) _swap(f);
  return *this;
}

void flushable_file_sink::_swap(const flushable_file_sink& f)
{
  close();
  _fpath = f._fpath;
  _mode  = f._mode;
  _open();
}

bool flushable_file_sink::is_open() const
{
  return _f.is_open();
}

std::streamsize
flushable_file_sink::write(const char *s, std::streamsize n)
{
  _f.write(s,n);
  return n;
}

std::streampos flushable_file_sink::seek(
  std::streamoff o,
  std::ios_base::seekdir d)
{
  _f.seekp(o,d);
  _f.seekg(o,d);
  return o;
}

std::streamsize flushable_file_sink::read(char_type *t, std::streamsize n)
{
  _f.read(t,n);
  return n;
}

bool flushable_file_sink::flush()
{
  _f.flush();
  return true;
}

void flushable_file_sink::close()
{
  flush();
  _f.close();
}

namespace fs = boost::filesystem;

void flushable_file_sink::_open()
{
  _fpath = fs::system_complete(fs::path(_fpath)).string();
  _f.open(_fpath.c_str(), _mode);
  if (_f.fail())
    {
      std::string s = _fpath + ": ";

      if (errno)
        s += cc::perror();
      else
        s += "unknown file open error";

      throw std::runtime_error(s);
    }
}

} // namspace cclive

// vim: set ts=2 sw=2 tw=72 expandtab:
