/* cclive
 * Copyright (C) 2011-2013  Toni Gundogdu <legatvs@gmail.com>
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

#ifndef cclive_util_h
#define cclive_util_h

namespace boost
{
  namespace program_options
  {
    class variables_map;
  }
}

namespace cc
{

class nothing_todo_error {
public:
  nothing_todo_error() { }
};

namespace po = boost::program_options;
class file;

void get(quvi::media&, void*, const po::variables_map&);
void exec(const file&, const po::variables_map&);
void curl_setup(void*, const po::variables_map&);

void go_background(const std::string&, bool&);
std::string perror(const std::string& p="");
void wait(const int);

} // namespace cc

#endif // cclive_util_h

/* vim: set ts=2 sw=2 tw=72 expandtab: */
