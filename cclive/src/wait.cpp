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

#include "config.h"

#include <iostream>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#define sleep(n) Sleep(n*1000)
#endif

#include "cclive/log.h"
#include "cclive/wait.h"

namespace cclive
{

void
wait (const int retry_wait)
{

  for (int i=1; i<=retry_wait; ++i)
    {

      if (i % 5 == 0)
        cclive::log << i;
      else
        cclive::log << ".";

      cclive::log << std::flush;

      sleep(1);
    }

  cclive::log << std::endl;
}

} // End namespace.

// vim: set ts=2 sw=2 tw=72 expandtab:
