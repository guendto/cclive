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

#include <stdexcept>
#include <sstream>
#include <cstdio>
#include <cerrno>

#include <sys/wait.h>

#include <boost/program_options/variables_map.hpp>
#include <boost/foreach.hpp>
#include <pcrecpp.h>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include <ccquvi>
#include <ccfile>
#include <ccre>
#include <ccutil>
#include <cclog>

namespace cc
{

typedef std::vector<std::string> vst;

static void tokenize(const std::string& r,
                     const std::string& s,
                     vst& dst)
{
  pcrecpp::StringPiece sp(s);
  pcrecpp::RE rx(r);

  std::string t;
  while (rx.FindAndConsume(&sp, &t))
    dst.push_back(t);
}

namespace po = boost::program_options;

int exec(const file& file,
         const quvi::url& url,
         const po::variables_map& map)
{
  std::stringstream b;
  b << "\"" << file.path() << "\"";

  std::string e = map["exec"].as<std::string>();
  pcrecpp::RE("%f").GlobalReplace(b.str(), &e);

  vst tokens;
  tokenize("(\"(.*?)\"|\\S+)", e, tokens);

  std::string t;
  vst args;

  foreach (const std::string s, tokens)
  {
    t = s;
    pcrecpp::RE("\"").GlobalReplace("", &t);
    args.push_back(t);
  }

  const size_t sz = args.size();
  const char **argv = new const char* [sz+2];
  if (!argv)
    throw std::runtime_error("memory allocation error");

  argv[0] = args[0].c_str();

  for (size_t i=1; i<sz; ++i)
    argv[i] = args[i].c_str();

  argv[sz] = NULL;

  fflush(stdout);
  fflush(stderr);

  pid_t child_pid = fork();
  if (child_pid == -1)
    {
      delete [] argv;
      throw std::runtime_error(cc::perror("fork"));
    }

  if (child_pid == 0)
    {
      execvp(argv[0], (char **)argv);
      exit(1);
    }

  delete [] argv;

  int wait_status = 0;
  while (waitpid(child_pid, &wait_status, 0) == (pid_t)-1)
    {
      if (errno != EINTR)
        {
          cc::log << "error waiting for " << args[0] << std::endl;
          break;
        }
    }

  if (WIFSIGNALED(wait_status))
    {
      cc::log << args[0]
              << " terminated by signal "
              << WTERMSIG(wait_status)
              << std::endl;
    }

  if (WEXITSTATUS(wait_status) == 0)
    return 0; // OK.

  return 1;
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
