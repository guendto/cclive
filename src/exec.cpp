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

#include <boost/format.hpp>

#include "quvicpp/quvicpp.h"

#include "cclive/options.h"
#include "cclive/file.h"
#include "cclive/re.h"
#include "cclive/exec.h"

namespace cclive
{

namespace po = boost::program_options;

void
exec (const file& file, const quvicpp::url& url, const options& opts)
{
  const po::variables_map map = opts.map();

  std::string arg = map["exec"].as<std::string>();

  boost::format fmt;

  fmt = boost::format("s{%%f}{\"%1%\"}g") % file.path();
  cclive::re::subst (fmt.str(), arg);

  const int rc = system ( arg.c_str() );

  std::stringstream b;

  switch (rc)
    {
    case  0:
      break;
    case -1:
      b << "failed to execute: `" << arg << "'";
      break;
    default:
      b << "child exited with: " << (rc >> 8);
      break;
    }

  const std::string s = b.str();

  if ( !s.empty() )
    throw std::runtime_error (s);
}

} // End namespace.

// vim: set ts=2 sw=2 tw=72 expandtab:
