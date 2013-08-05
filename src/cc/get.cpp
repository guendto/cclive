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

#include <iostream>
#include <boost/program_options/variables_map.hpp>

#include <ccquvi>
#include <ccoptions>
#include <cclog>
#include <ccfile>
#include <ccutil>

namespace cc
{

namespace po = boost::program_options;

void get(quvi::media& media, void *curl, const po::variables_map& vm)
{
  const int max_retries  = vm[OPT__MAX_RETRIES].as<cc::max_retries>().value();
  const int retry_wait   = vm[OPT__RETRY_WAIT].as<cc::retry_wait>().value();

  const bool no_download = vm[OPT__NO_DOWNLOAD].as<bool>();
  const bool exec        = vm.count(OPT__EXEC);

  int retry = 0;

  while (retry <= max_retries)
    {
      cc::file file(media, vm);

      if (file.nothing_todo())
        {
          if (exec)
            cc::exec(file, vm);
#define E_NOTHING_TODO "media retrieved completely already"
          throw std::runtime_error(E_NOTHING_TODO);
#undef E_NOTHING_TODO
        }

      // Download media.

      if (retry > 0)
        {
          cc::log
              << "Retrying "
              << retry
              << " of "
              << max_retries
              << " ... "
              << std::flush;

          cc::wait(retry_wait);
        }

      cc::log << file.to_s(media) << std::endl;
      ++retry;

      if (!no_download)
        {
          if (!file.write(media, curl, vm))
            continue; // Retry.

          if (exec)
            cc::exec(file, vm);
        }

      break; // Stop retrying.
    }
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
