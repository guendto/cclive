/* cclive
 * Copyright (C) 2010-2013  Toni Gundogdu <legatvs@gmail.com>
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

#include <boost/exception/diagnostic_information.hpp>
#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/get_error_info.hpp>
#include <boost/exception/errinfo_errno.hpp>

#include <stdexcept>
#include <iostream>
#include <clocale>

#include <ccapplication>
#include <ccquvi>
#include <ccoptions>
#include <ccutil>

using namespace cc;

int main(int const argc, char const **argv)
{
  setlocale(LC_ALL, "");
  application::exit_status es = application::ok;
  try
    {
      application app;
      es = app.exec(argc, argv);
    }
  // --version, --help, etc.
  catch (const cc::error::exit_program&)
    {
      // Fall through.
    }
  catch (const cc::error::no_input&)
    {
      std::clog << "error: no input URL" << std::endl;
      es = application::error;
    }
  // Thrown by quvi::query constructor (e.g. quvi_init, quvi_new).
  catch (const quvi::error& e)
    {
      std::clog << "libquvi: error: " << e.what() << std::endl;
      es = application::error;
    }
  // Thrown by boost (e.g. cc::go_background failure).
  catch (const std::runtime_error& e)
    {
      std::clog << "error: " << e.what() << std::endl;
      es = application::error;
    }
  // Thrown by boost::program_options (cc::options).
  catch (const boost::exception& x)
    {
      es = static_cast<application::exit_status>(cc::error::print(x));
    }
  return es;
}

// vim: set ts=2 sw=2 tw=72 expandtab:
