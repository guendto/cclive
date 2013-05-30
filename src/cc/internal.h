/* cclive
 * Copyright (C) 2011  Toni Gundogdu <legatvs@gmail.com>
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

// Include this header instead of 'config.h'.

#ifndef cclive_internal_h
#define cclive_internal_h

#include <boost/version.hpp>

#include "config.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

// Boost 1.44+
#if (BOOST_VERSION >= 104400)
#ifndef BOOST_FILESYSTEM_VERSION
#define BOOST_FILESYSTEM_VERSION 3 // Default in 1.46.0
#endif
#endif

#endif // cclive_internal_h

/* vim: set ts=2 sw=2 tw=72 expandtab: */
