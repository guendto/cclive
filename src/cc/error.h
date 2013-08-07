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

#ifndef cc__error_h
#define cc__error_h

#include <ccinternal>

#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/get_error_info.hpp>
#include <boost/exception/errinfo_errno.hpp>
#include <boost/tuple/tuple.hpp>
#include <cerrno>

namespace cc
{

namespace error
{

static inline std::string strerror(const int ec)
{
  char buf[256];
  return strerror_r(ec, buf, sizeof(buf));
}

static inline std::string strerror() { return strerror(errno); }

typedef boost::tuple<std::string,std::string> type_tuple;
typedef boost::error_info<struct tag_tuple,type_tuple> errinfo_tuple;

static inline int print(const boost::exception& x)
{
  std::clog << "error: ";

  if (std::string const *s =
      boost::get_error_info<boost::errinfo_file_name>(x))
    {
      std::clog << *s << ": ";
    }

  bool print_what = true;
  if (int const *n =
      boost::get_error_info<boost::errinfo_errno>(x))
    {
      std::clog << cc::error::strerror(*n) << " (" << *n << ")";
      print_what = false;
    }

  if (type_tuple const *t = boost::get_error_info<errinfo_tuple>(x))
    {
      std::clog << boost::get<0>(*t) << ": " << boost::get<1>(*t);
      print_what = false;
    }

  try
    {
      throw;
    }
  catch (const std::exception& x)
    {
      if (print_what)
        std::clog << x.what();
    }
  catch (...)
    {
      std::clog << boost::diagnostic_information(x);
    }
  std::clog << std::endl;
  return EXIT_FAILURE;
}

struct exit_program : virtual std::exception, virtual boost::exception { };
struct no_input : virtual std::exception, virtual boost::exception { };
struct config : virtual std::exception, virtual boost::exception { };
struct tuple : virtual std::exception, virtual boost::exception { };

} // namespace error

} // namespace cc
#endif // cc__error_h

/* vim: set ts=2 sw=2 tw=72 expandtab: */
