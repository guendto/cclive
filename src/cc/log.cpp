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

#include <stdexcept>
#include <cstdarg>
#include <cerrno>

#include <boost/filesystem.hpp>

#include <ccutil>
#include <cclog>

namespace cc
{

namespace fs = boost::filesystem;
namespace io = boost::iostreams;

io::filtering_ostream log;

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

void _debug(const std::string& fn, const std::string& func,
            const int ln, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[%s:%d] ", fn.c_str(), ln);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

} // namspace cclive

// vim: set ts=2 sw=2 tw=72 expandtab:
