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

#ifndef cc__var_h
#define cc__var_h

#include <boost/iostreams/filtering_stream.hpp>
#include <csignal>

namespace cc
{
  boost::iostreams::filtering_ostream log;
  namespace var
  {
    volatile sig_atomic_t recv_sigwinch = 0;
    volatile sig_atomic_t recv_sigusr1 = 0;
    size_t max_width = 0;
  } // namespace sig
} // namespace cc

#endif // cc__var_h

// vim: set ts=2 sw=2 tw=72 expandtab:
