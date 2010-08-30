/* 
* Copyright (C) 2010 Toni Gundogdu.
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

#include "cclive/options.h"
#include "cclive/file.h"
#include "cclive/exec.h"
#include "cclive/get.h"

namespace cclive {

namespace po = boost::program_options;

void
get (
    const quvicpp::query& query,
    quvicpp::video& video,
    const options& opts)
{
    const po::variables_map map = opts.map ();

    const bool no_download = map.count ("no-download");
    const bool exec        = map.count ("exec");

    int n = 0;

    quvicpp::link link;

    while ( (link = video.next_link ()).ok ()) {

        cclive::file file (video, link, ++n, opts);

        if (!no_download) {

            file.write (query, link, opts);

            if (exec)
                cclive::exec (file, link, opts);
        }
        else
            std::clog << file.to_s (link) << std::endl;
    }
}

} // End namespace.


