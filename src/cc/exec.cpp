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
#include <sstream>

#include <boost/program_options/variables_map.hpp>
#include <pcrecpp.h>

#include <ccquvi>
#include <ccfile>
#include <ccre>
#include <ccutil>

namespace cc
{

namespace po = boost::program_options;

void exec(const file& file,
          const quvi::url& url,
          const po::variables_map& map)
{
  std::string arg = map["exec"].as<std::string>();
  pcrecpp::RE("%f").GlobalReplace(file.path(), &arg);

  const int rc = system(arg.c_str());
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

  if (!s.empty())
    throw std::runtime_error(s);
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
