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

#include <iostream>

#include "cclive/application.h"

int main(int argc, char *argv[])
{
  cclive::application app;
  int rc = 0; // OK.

  try
    {
      rc = app.exec(argc,argv);
    }

  // Thrown by quvicpp::query constructor (e.g. quvi_init failure).
  catch (const quvicpp::error& e)
    {
      std::clog << "libquvi: error: " << e.what() << std::endl;
    }

  // Thrown by boost (e.g. cclive::go_background failure).
  catch (const std::runtime_error& e)
    {
      std::clog << "error: " << e.what() << std::endl;
    }

  return rc;
}

// vim: set ts=2 sw=2 tw=72 expandtab:
