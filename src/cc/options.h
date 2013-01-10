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

#ifndef cclive_options_h
#define cclive_options_h

#include <boost/program_options.hpp>

namespace cc
{

struct flags_s
{
  bool verbose_libcurl;
  bool print_streams;
  bool query_formats;
  bool no_download;
  bool no_resolve;
  bool background;
  bool overwrite;
  bool no_proxy;
  bool version;
  bool license;
  bool support;
  bool quiet;
  bool cont/*inue*/;
  bool help;
};

class options
{
  inline friend std::ostream& operator<<(std::ostream& os, const options& o)
    {
      return os << o._visible;
    }
public:
  inline const boost::program_options::variables_map& map()const {return _map;}
  void exec(int argc, char **argv);
private:
  void _validate();
private:
  boost::program_options::options_description _visible;
  boost::program_options::variables_map _map;
public:
  struct flags_s flags;
};

extern options opts;

} // namespace cc

#endif // cclive_options_h

// vim: set ts=2 sw=2 tw=72 expandtab:
