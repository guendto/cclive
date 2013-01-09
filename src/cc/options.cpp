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

options opts;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

typedef std::vector<std::string> vst;

void options::exec(int argc, char **argv)
{
  memset(&flags, 0, sizeof(struct flags_s));

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

  po::options_description generic;
  std::string conf_file;

  generic.add_options()
  ("version",
   po::value(&flags.version)->zero_tokens(),
   "Print version and exit")
  ("help",
   po::value(&flags.help)->zero_tokens(),
   "Print help and exit")
  ("license",
   po::value(&flags.license)->zero_tokens(),
   "Print license and exit")
  ("support",
   po::value(&flags.support)->zero_tokens(),
   "Print supported websites and exit")
  ("verbose-libcurl",
   po::value(&flags.verbose_libcurl)->zero_tokens(),
   "Turn on libcurl verbose output")
  ("quiet,q",
   po::value(&flags.quiet)->zero_tokens(),
   "Turn off all output, excl. errors")
#ifdef HAVE_FORK
  ("background,b",
   po::value(&flags.background)->zero_tokens(),
   "Go to background")
#endif
  ("query-formats,F",
   po::value(&flags.query_formats)->zero_tokens(),
   "Query available formats to URL")
  ("format,f",
   po::value<std::string>(),
   "Download media format")
  ("continue,c",
   po::value(&flags.cont)->zero_tokens(),
   "Resume partially downloaded media")
  ("overwrite,W",
   po::value(&flags.overwrite)->zero_tokens(),
   "Overwrite existing media")
  ("output-file,O",
   po::value<std::string>(),
   "Write media to arg")
  ("no-download,n",
   po::value(&flags.no_download)->zero_tokens(),
   "Do not download media, print details")
  ("no-resolve,r",
   po::value(&flags.no_resolve)->zero_tokens(),
   "Do not resolve URL redirections")
  ("no-proxy",
   po::value(&flags.no_proxy)->zero_tokens(),
   "Do not use HTTP proxy")
  ("log-file",
   po::value<std::string>()->default_value("cclive_log"),
   "Write log output to arg")
  ("config-file",
   po::value<std::string>(&conf_file)->default_value(conf_path.string()),
   "Read args from arg")
  ;

  // Config.

  po::options_description config("Configuration");

  config.add_options()
  ("prefer-format,p",
   po::value<std::vector<std::string> >()->composing(),
   "Preferred format [domain:format[,...]]")
  ("progressbar",
   po::value<std::string>()->default_value("normal"),
   "Use progressbar arg")
  ("update-interval",
   po::value<double>()->default_value(1.0),
   "Update interval of progressbar")
  ("filename-format",
   po::value<std::string>()->default_value("%t.%s"),
   "Downloaded media filename format")
  ("output-dir",
   po::value<std::string>(),
   "Write downloaded media to arg directory")
  ("tr,t",
   po::value<vst>()->composing(),
   "Translate characters in media title")
  ("regexp",
   po::value<std::string>(),
   "Regexp to cleanup media title (depr.)")
  ("subst", po::value<std::string>(),
   "Replace matched occurences in filename (depr.)")
  ("exec", po::value<vst>()->composing(),
   "Invoke arg after each finished download")
  ("agent",
   po::value<std::string>()->default_value("Mozilla/5.0"),
   "Identify as arg to HTTP servers")
  ("proxy", po::value<std::string>(),
   "Use proxy for HTTP connections")
  ("throttle", po::value<int>()->default_value(0),
   "Do not exceed transfer rate arg KB/s")
  ("connect-timeout", po::value<int>()->default_value(30),
   "Seconds connecting allowed to take")
  ("transfer-timeout", po::value<int>()->default_value(0),
   "Seconds transfer allowed to take")
  ("dns-cache-timeout", po::value<int>()->default_value(60),
   "Seconds DNS resolves kept in memory")
  ("max-retries", po::value<int>()->default_value(5),
   "Max download attempts before giving up")
  ("retry-wait", po::value<int>()->default_value(5),
   "Time to wait before retrying")
  ;

  // Hidden.

  po::options_description hidden;

  hidden.add_options()
  ("url", po::value<vst>(), "url");

  // Visible.

  _visible.add(generic).add(config);

  // Command line options.

  po::options_description cmdline_options;
  cmdline_options.add(generic).add(config).add(hidden);

  // Config file options.

  po::options_description config_file_options;
  config_file_options.add(config);

  // Positional.

  po::positional_options_description p;
  p.add("url", -1);

  // Parse.

  store(po::command_line_parser(argc,argv)
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
