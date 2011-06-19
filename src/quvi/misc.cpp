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

#include <sstream>

#include <quvi/ccquvi.h>

namespace quvi
{

// Version.

std::string version()
{
  return quvi_version(QUVI_VERSION);
}

std::string version_long()
{
  return quvi_version(QUVI_VERSION_LONG);
}

// To string. Mimic quvi(1) behaviour.

std::string support_to_s(const std::map<std::string,std::string>& map)
{
  std::map<std::string,std::string>::const_iterator iter;
  std::stringstream b;

  for (iter = map.begin(); iter != map.end(); ++iter)
    b << (*iter).first << "\t" << (*iter).second << "\n";

  return b.str();
}

} // namespace quvi

// vim: set ts=2 sw=2 tw=72 expandtab:
