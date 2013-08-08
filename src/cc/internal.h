/* cclive
 * Copyright (C) 2011,2013  Toni Gundogdu <legatvs@gmail.com>
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

// NOTE: Include this header instead of 'config.h'

#ifndef cc__internal_h
#define cc__internal_h

#include <boost/version.hpp>
#include "config.h"

// Boost.Program_Options

#if (BOOST_VERSION <= 104900)
  // There is a bug in Boost.Program_Options (1.49 at least, possibly
  // earlier) that causes the library to provide an incomplete error
  // message when an option is given multiple times (and this isn't
  // acceptable). 1.53+ appears to work OK, 1.50-1.52 unconfirmed.
  #ifndef HAVE_BUG__BOOST_PO__MULTIPLE_OCCURRENCES
    #define HAVE_BUG__BOOST_PO__MULTIPLE_OCCURRENCES
  #endif
#endif

// Boost.Filesystem

#define BOOST_FILESYSTEM_NO_DEPRECATED

#if (BOOST_VERSION >= 104400)           // Boost 1.44+
  #ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 3  // Default in 1.46.0
  #endif
#endif

#ifndef if_optsw_given
  #define if_optsw_given(__varmap,__varname)\
    if (__varmap[__varname].as<bool>())
#endif

#ifndef ifn_optsw_given
  #define ifn_optsw_given(__varmap,__varname)\
    if (! __varmap[__varname].as<bool>())
#endif

#if defined(HAVE_WORKING_FORK) || defined(HAVE_WORKING_VFORK)
  #ifdef HAVE_VFORK
    #define vfork fork
  #endif
#endif

#endif // cc__internal_h

/* vim: set ts=2 sw=2 tw=72 expandtab: */
