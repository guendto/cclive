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

#ifndef cclive_application_h
#define cclive_application_h

#include <ccoptions>

namespace quvi
{
class options;
class query;
}

namespace cc
{

class application
{
public:
  typedef enum {ok=EXIT_SUCCESS, error=EXIT_FAILURE} exit_status;
public:
  exit_status exec(int,char **);
private:
  void _tweak_curl_opts(const quvi::query&,
                        const boost::program_options::variables_map&);
  void _set_format_string(const std::string&,
                          quvi::options&,
                          const boost::program_options::variables_map&);
private:
  cc::options _opts;
};

} // namespace cc

#endif // cclive_application_h

// vim: set ts=2 sw=2 tw=72 expandtab:
