/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
 *
 * This file is part of cclive <http://cclive.sourceforge.net/>
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

#ifndef cc__fstream_h
#define cc__fstream_h

#include <ccinternal>

#include <boost/exception/errinfo_errno.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <algorithm>
#include <fstream>

namespace cc
{

struct fstream;
typedef boost::shared_ptr<fstream> fstream_ptr;

struct fstream : boost::noncopyable
{
  inline fstream(const std::string& fpath,
                 const std::ios_base::openmode& m=std::ofstream::binary)
  {
    _o = boost::shared_ptr<std::ofstream>(new std::ofstream(fpath.c_str(),m));
    if (_o->fail())
      {
        BOOST_THROW_EXCEPTION(error() << boost::errinfo_file_name(fpath)
                                      << boost::errinfo_errno(errno));
      }
    _fpath = fpath;
  }

  static inline fstream_ptr create(const std::string& fpath)
  {
    return fstream_ptr(new fstream(fpath, std::ofstream::trunc));
  }

  static inline fstream_ptr append(const std::string& fpath)
  {
    return fstream_ptr(new fstream(fpath, std::ofstream::app));
  }

  static inline fstream_ptr determine_mode(const std::string& fpath,
                                           uintmax_t& n)
  {
    boost::system::error_code ec; // Throw nothing.
    n = boost::filesystem::file_size(fpath, ec);
    return (n >0) ? append(fpath) : create(fpath);
  }

  static inline std::string read_ignore_errors(const std::string& fpath)
  {
    return read_contents(fpath, false);
  }

  static inline std::string read(const std::string& fpath)
  {
    return read_contents(fpath);
  }

  inline size_t write(const char *data, const size_t size)
  {
    _o->write(data, size);
    if (_o->fail())
      {
        BOOST_THROW_EXCEPTION(error() << boost::errinfo_file_name(_fpath)
                                      << boost::errinfo_errno(errno));
      }
    _o->flush();
    if (_o->fail())
      {
        BOOST_THROW_EXCEPTION(error() << boost::errinfo_file_name(_fpath)
                                      << boost::errinfo_errno(errno));
      }
    return size;
  }

  struct error : virtual std::exception, virtual boost::exception { };

private:
  static inline std::string read_contents(const std::string& fpath,
                                          const bool throws_if_fails=true)
  {
    const fs::path p = fs::system_complete(fpath);

    boost::system::error_code ec;
    if (fs::is_directory(p, ec))
      {
        BOOST_THROW_EXCEPTION(cc::error::tuple()
          << cc::error::errinfo_tuple(
            boost::make_tuple(p.string(), "is a directory")));
      }

    std::ifstream f(p.string().c_str());
    std::string r;
    if (!f)
      {
        if (throws_if_fails)
        {
          BOOST_THROW_EXCEPTION(error() << boost::errinfo_file_name(fpath)
                                        << boost::errinfo_errno(errno));
        }
        return r;
      }
    std::copy(std::istreambuf_iterator<char>(f),
              std::istreambuf_iterator<char>(),
              std::back_inserter(r));
    return r;
  }

private:
  boost::shared_ptr<std::ofstream> _o;
  std::string _fpath;
};

} // namespace cc

#endif // cc__fstream_h

// vim: set ts=2 sw=2 tw=72 expandtab:
