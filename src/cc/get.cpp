/* cclive
 * Copyright (C) 2010-2013  Toni Gundogdu <legatvs@gmail.com>
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

#include <boost/program_options/variables_map.hpp>

#include <ccoptions>
#include <ccquvi>
#include <cclog>
#include <ccfile>
#include <ccutil>

namespace cc
{

namespace po = boost::program_options;

void get(const quvi::query& query, quvi::media& media)
{
  const po::variables_map map = cc::opts.map();

  const bool no_download = opts.flags.no_download;
  const int max_retries  = map["max-retries"].as<int>();
  const int retry_wait   = map["retry-wait"].as<int>();
  const bool exec        = map.count("exec");

  quvi::url url;
  int n = 0;

  while ((url = media.next_url()).ok())
    {
      int retry = 0;
      ++n;
      while (retry <= max_retries)
        {
          cc::file file(media, url, n);

          if (file.nothing_todo())
            {
              if (exec)
                cc::exec(file);

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

          cc::log << file.to_s(url) << std::endl;
          ++retry;

          if (!no_download)
            {
              if (!file.write(query, url));
                continue; // Retry.

              if (exec)
                cc::exec(file);
            }

          break; // Stop retrying.
        }
    }
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
