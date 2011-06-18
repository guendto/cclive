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

#include "quvicpp/quvicpp.h"

#include "cclive/log.h"
#include "cclive/options.h"
#include "cclive/file.h"
#include "cclive/exec.h"
#include "cclive/wait.h"
#include "cclive/get.h"

namespace cclive
{

namespace po = boost::program_options;

void get(
  const quvicpp::query& query,
  quvicpp::media& media,
  const options& opts)
{
  const po::variables_map map = opts.map();

  const bool no_download = map.count("no-download");
  const bool exec        = map.count("exec");

  const int max_retries  = map["max-retries"].as<int>();
  const int retry_wait   = map["retry-wait"].as<int>();

  int n = 0;

  quvicpp::url url;

  while ((url = media.next_url()).ok())
    {
      ++n;

      int retry = 0;

      while (retry <= max_retries)
        {
          cclive::file file(media, url, n, opts);

          if (retry > 0)
            {
              cclive::log
                  << "Retrying "
                  << retry
                  << " of "
                  << max_retries
                  << " ... "
                  << std::flush;

              cclive::wait(retry_wait);
            }

          ++retry;

          cclive::log << file.to_s(url) << std::endl;

          if (!no_download)
            {
              if (!file.write(query, url, opts))
                continue; // Retry.

              if (exec)
                cclive::exec(file, url, opts);
            }

          break; // Stop retrying.
        }
    }
}

} // namespace cclive

// vim: set ts=2 sw=2 tw=72 expandtab:
