/* cclive
 * Copyright (C) 2010-2011  Toni Gundogdu <legatvs@gmail.com>
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

#include <ccinternal>

#include <iostream>
#include <climits>
#include <cstring>
#include <cerrno>

#include <ccquvi>
#include <ccutil>

namespace cc
{

std::string perror(const std::string& p/*=""*/)
{
#if defined (HAVE_STRERROR) || defined (HAVE_STRERROR_R)
  std::string s;
#ifdef HAVE_STRERROR_R
  char buf[256];
  s = strerror_r(errno, buf, sizeof(buf));
#else // HAVE_STRERROR_R
  s = strerror(errno);
#endif // HAVE_STRERROR_R
  return s;
#else // HAVE_STRERROR || HAVE_STRERROR_R
  perror(p.c_str()); // No strerror or strerror_r
#endif // HAVE_STRERROR || HAVE_STRERROR_R
}

} // namspace cclive

// vim: set ts=2 sw=2 tw=72 expandtab:
