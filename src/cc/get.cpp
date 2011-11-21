/* cclive
 * Copyright (C) 2010-2011  Toni Gundogdu <legatvs@gmail.com>
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

#include <ccquvi>
#include <cclog>
#include <ccfile>
#include <ccutil>

namespace cc
{

namespace po = boost::program_options;

void get(const quvi::query& query,
         quvi::media& media,
         const po::variables_map& map)
{
  const bool no_download = map.count("no-download");
  const bool exec        = map.count("exec");

  const int max_retries  = map["max-retries"].as<int>();
  const int retry_wait   = map["retry-wait"].as<int>();

  int n = 0;

  quvi::url url;

  while ((url = media.next_url()).ok())
    {
      ++n;

      int retry = 0;

      while (retry <= max_retries)
        {
          cc::file file(media, url, n, map);

          if (file.nothing_todo())
            {
              if (exec)
                cc::exec(file, map);

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

          ++retry;

          cc::log << file.to_s(url) << std::endl;

          if (!no_download)
            {
              if (!file.write(query, url, map))
                continue; // Retry.

              if (exec)
                cc::exec(file, map);
            }

          break; // Stop retrying.
        }
    }
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
