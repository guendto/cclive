/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
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

#include <pcrecpp.h>
#include <sstream>

#include <ccquvi>

namespace quvi
{

// Version.

std::string version()
{
  return quvi_version(QUVI_VERSION);
}

typedef std::map<std::string,std::string>::const_iterator smci;

static const std::string _domain_patt_to_s(smci i)
{
  std::string d = (*i).first;
#ifndef HAVE_LIBQUVI_0_9 /* Only libquvi 0.4 needs this. */
  pcrecpp::RE("%w\\+").GlobalReplace("com", &d);
  pcrecpp::RE("%").GlobalReplace("", &d);
#endif
  return d;
}

std::string support_to_s(const std::map<std::string,std::string>& map)
{
  std::stringstream b;

  for (smci iter = map.begin(); iter != map.end(); ++iter)
    b << _domain_patt_to_s(iter) << "\n";

  return b.str();
}

} // namespace quvi

// vim: set ts=2 sw=2 tw=72 expandtab:
