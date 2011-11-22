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

#include <ccinternal>

#include <iostream>
#include <fstream>
#include <cstring>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include <ccre>
#include <ccoptions>

namespace cc
{

namespace opts = boost::program_options;
namespace fs   = boost::filesystem;

typedef std::vector<std::string> vst;

void options::exec(int argc, char **argv)
{
  // Path to ccliverc.

#if BOOST_FILESYSTEM_VERSION > 2
  fs::path conf_path(fs::current_path());
#else
  fs::path conf_path(fs::current_path<fs::path>());
#endif

  const char *home = getenv("HOME");

  if (home && strlen(home) > 0)
    conf_path = fs::system_complete(fs::path(home));

  conf_path /=
#ifndef _WIN32
    std::string(".") +
#endif
    std::string("ccliverc");

  // Construct options.

  opts::options_description generic;
  std::string conf_file;

  generic.add_options()
  ("version",
   "Print version and exit")
  ("help",
   "Print help and exit")
  ("license",
   "Print license and exit")
  ("support",
   "Print supported websites and exit")
  ("verbose-libcurl",
   "Turn on libcurl verbose output")
  ("quiet,q",
   "Turn off all output, excl. errors")
#ifdef HAVE_FORK
  ("background,b",
   "Go to background")
#endif
  ("query-formats,F",
   "Query available formats to URL")
  ("format,f",
   opts::value<std::string>(),
   "Download media format")
  ("continue,c",
   "Resume partially downloaded media")
  ("overwrite,W",
   "Overwrite existing media")
  ("output-file,O",
   opts::value<std::string>(),
   "Write media to arg")
  ("no-download,n",
   "Do not download media, print details")
  ("no-resolve,r",
   "Do not resolve URL redirections")
  ("no-proxy",
   "Do not use HTTP proxy")
  ("log-file",
   opts::value<std::string>()->default_value("cclive_log"),
   "Write log output to arg")
  ("config-file",
   opts::value<std::string>(&conf_file)->default_value(conf_path.string()),
   "Read args from arg")
  ;

  // Config.

  opts::options_description config("Configuration");

  config.add_options()
  ("prefer-format,p",
   opts::value<std::vector<std::string> >()->composing(),
   "Preferred format [domain:format[,...]]")
  ("progressbar",
   opts::value<std::string>()->default_value("normal"),
   "Use progressbar arg")
  ("update-interval",
   opts::value<double>()->default_value(1.0),
   "Update interval of progressbar")
  ("filename-format",
   opts::value<std::string>()->default_value("%t.%s"),
   "Downloaded media filename format")
  ("output-dir",
   opts::value<std::string>(),
   "Write downloaded media to arg directory")
  ("tr,t",
   opts::value<vst>()->composing(),
   "Translate characters in media title")
  ("regexp",
   opts::value<std::string>(),
   "Regexp to cleanup media title (depr.)")
  ("subst", opts::value<std::string>(),
   "Replace matched occurences in filename (depr.)")
  ("exec", opts::value<vst>()->composing(),
   "Invoke arg after each finished download")
  ("agent",
   opts::value<std::string>()->default_value("Mozilla/5.0"),
   "Identify as arg to HTTP servers")
  ("proxy", opts::value<std::string>(),
   "Use proxy for HTTP connections")
  ("throttle", opts::value<int>()->default_value(0),
   "Do not exceed transfer rate arg KB/s")
  ("connect-timeout", opts::value<int>()->default_value(30),
   "Seconds connecting allowed to take")
  ("transfer-timeout", opts::value<int>()->default_value(0),
   "Seconds transfer allowed to take")
  ("dns-cache-timeout", opts::value<int>()->default_value(60),
   "Seconds DNS resolves kept in memory")
  ("max-retries", opts::value<int>()->default_value(5),
   "Max download attempts before giving up")
  ("retry-wait", opts::value<int>()->default_value(5),
   "Time to wait before retrying")
  ;

  // Hidden.

  opts::options_description hidden;

  hidden.add_options()
  ("url", opts::value<vst>(), "url");

  // Visible.

  _visible.add(generic).add(config);

  // Command line options.

  opts::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  // Config file options.

  opts::options_description config_file_options;
  config_file_options.add(config);

  // Positional.

  opts::positional_options_description p;
  p.add("url", -1);

  // Parse.

  store(opts::command_line_parser(argc,argv)
        .options(cmdline_options).positional(p).run(), _map);
  notify(_map);

  // Read config.

  std::ifstream ifs(conf_file.c_str());

  if (ifs)
    {
      store(parse_config_file(ifs, config_file_options), _map);
      notify(_map);
    }

  _validate();
}

const opts::variables_map& options::map() const
{
  return _map;
}

std::ostream& operator<<(std::ostream& os, const options& o)
{
  return os << o._visible;
}

void options::_validate()
{
  std::string empty;

  if (_map.count("tr"))
    {
      vst v = _map["tr"].as<vst>();
      foreach (const std::string s, v)
      {
        re::tr(s, empty);
      }
    }

  if (_map.count("regexp")) // Deprecated.
    {
      std::clog
          << "WARNING --regexp is deprecated and will be removed "
          << "in later versions.\nWARNING Use --tr instead."
          << std::endl;

      std::string s = _map["regexp"].as<std::string>();
      if (!cc::re::capture(s, empty))
        {
          std::stringstream b;
          b << "--regexp: expects "
            << "`/pattern/flags', for example: \"/(\\w|\\s)/g\"";
          throw std::runtime_error(b.str());
        }
    }

  if (_map.count("subst")) // Deprecated.
    {
      std::clog
          << "WARNING --subst is deprecated and will be removed "
          << "in later versions.\nWARNING Use --tr instead."
          << std::endl;

      std::istringstream iss( _map["subst"].as<std::string>());
      vst v;

      std::copy(
        std::istream_iterator<std::string >(iss),
        std::istream_iterator<std::string >(),
        std::back_inserter<vst>(v)
      );

      foreach (const std::string s, v)
      {
        if (!cc::re::subst(s,empty))
          {
            std::stringstream b;
            b << "--subst: expects " << "`s{old}{new}flags'";
            throw std::runtime_error(b.str());
          }
      }
    }
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
