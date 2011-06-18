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

#ifndef cclive_application_h
#define cclive_application_h

#include "cclive/options.h"
#include "quvicpp/quvicpp.h"

namespace cclive
{

class application
{
public:
  typedef enum { ok=0, invalid_option, system } exit_status;
public:
  exit_status exec(int,char **);
private:
  void _tweak_curl_opts(const quvicpp::query&,
                        const boost::program_options::variables_map&);
private:
  cclive::options   _opts;
};

} // namespace cclive

#endif

// vim: set ts=2 sw=2 tw=72 expandtab:
