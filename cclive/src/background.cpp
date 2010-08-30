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

#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <boost/iostreams/tee.hpp>

#include "cclive/error.h"
#include "cclive/log.h"
#include "cclive/application.h"
#include "cclive/background.h"

namespace cclive {

namespace io = boost::iostreams;

void
go_background (const std::string& log_file, bool& omit) {

#ifdef HAVE_FORK

    // Has to be done here before calling fork.

    cclive::log.push (io::tee (cclive::flushable_file_sink (log_file)));

    omit = true;

    const pid_t pid = fork();

    if (pid < 0) {

        cclive::perror ("fork");

        exit (application::system);
    }
    else if (pid != 0) {

        std::clog
            << "Run in background (pid: "
            << static_cast<long>(pid)
            << "). Redirect output to \""
            << log_file
            << "\"."
            << std::endl;

        exit (0);
    }

    setsid ();

#ifdef HAVE_GETCWD
    char buf[PATH_MAX];
    chdir (getcwd (buf, sizeof(buf) ) );
#endif

    umask (0);

#else // HAVE_FORK

    std::clog << "warning: ignoring --background, no fork(3)." << std::endl;

#endif
}

} // End namespace.


