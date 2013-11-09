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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include <boost/iostreams/tee.hpp>

#include <ccquvi>
#include <ccutil>
#include <ccapplication>
#include <cclog>

namespace cc
{

namespace io = boost::iostreams;

#if defined(HAVE_WORKING_FORK) || defined(HAVE_WORKING_VFORK)
void go_background(const std::string& log_file, bool& omit)
{
  const pid_t pid = fork();

  if (pid < 0)
    {
      cc::perror("fork");
      exit(application::error);
    }
  else if (pid != 0)
    {
      std::clog // Parent exits
          << "Run in background (pid: "
          << static_cast<long>(pid)
          << "). Redirect output to \""
          << log_file
          << "\"."
          << std::endl;
      exit(0);
    }

  // Child continues and becomes the session leader.
  setsid();

  // Clear file mode creation mask.
  umask(0);

  // Close unneeded file descriptors/streams.
  freopen("/dev/null", "w", stdout);
  freopen("/dev/null", "w", stderr);
  freopen("/dev/null", "r", stdin);

  // Redirect output to log file.
  cc::log.push(io::tee(cc::sink::flushable_file(log_file)));

  // Omit output to stderr (using cc::log).
  omit = true;
}
#endif // HAVE_WORKING_(V)FORK

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
