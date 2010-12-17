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

#include <fstream>
#include <cstring>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include "cclive/re.h"
#include "cclive/options.h"

namespace cclive {

namespace opts = boost::program_options;
namespace fs   = boost::filesystem;

void
options::exec (int argc, char **argv) {

    // Path to ccliverc.

    fs::path config_path (fs::initial_path<fs::path> ());

    const char *home = getenv ("HOME");

    if (home && strlen (home) > 0)
        config_path = fs::system_complete (fs::path (home));

    config_path /=
#ifndef _WIN32
    std::string (".") +
#endif
    std::string ("ccliverc");

    // Construct options.

    opts::options_description generic;

    generic.add_options()
        ("version",         "print version and exit")
        ("help",            "print help and exit")
        ("license",         "print license and exit")
        ("support",         "print supported websites and exit")
        ("verbose-libcurl", "turn on libcurl verbose output")
        ("quiet,q",         "turn off all output, excl. errors")
        ("background,b",    "go to background")
        ("format,f",
            opts::value<std::string>()->default_value("default"),
            "download video format")
        ("continue,c",      "resume partially downloaded video")
        ("overwrite,W",     "overwrite existing video")
        ("output-file,O",   opts::value<std::string>(),
            "write downloaded video to file")
        ("no-download,n",   "do not download video, print info only")
        ("no-proxy",        "disable use of http proxy")
        ("log-file",
            opts::value<std::string>()->default_value("cclive_log"),
            "write log output to arg")
        ("update-interval",
            opts::value<double>()->default_value(1.0),
            "update interval of progressbar")
        ("config-file",
            opts::value<std::string>(&_config_file)
                ->default_value(config_path.string()),
            "file to read cclive arguments from")
    ;

    // Config.

    opts::options_description config ("Configuration");

    config.add_options ()
        ("filename-format",
            opts::value<std::string>()->default_value("%t.%s"),
            "output video filename format")
        ("output-dir",
            opts::value<std::string>(),
            "output directory for downloaded videos")
        ("regexp",
            opts::value<std::string>()->default_value("/(\\w|\\pL|\\s)/g"),
            "regexp to clean up video title")
        ("subst", opts::value<std::string>(),
            "replace matched occurences in filename")
        ("exec", opts::value<std::string>(),
            "invoke arg after download finishes")
        ("agent",
            opts::value<std::string>()->default_value("Mozilla/5.0"),
            "identify as arg to http servers")
        ("proxy", opts::value<std::string>(),
            "use proxy for http connections")
        ("throttle", opts::value<int>()->default_value(0),
            "limit download transfer rate to KB/s")
        ("connect-timeout", opts::value<int>()->default_value(30),
            "seconds connecting allowed to take")
        ("transfer-timeout", opts::value<int>()->default_value(0),
            "seconds transfer allowed to take")
        ("dns-cache-timeout", opts::value<int>()->default_value(60),
            "seconds dns resolves kept in memory")
        ("max-retries", opts::value<int>()->default_value(5),
            "max download attempts before giving up")
        ("retry-wait", opts::value<int>()->default_value(5),
            "time to wait before retrying")
    ;

    // Hidden.

    opts::options_description hidden;

    hidden.add_options ()
        ("url", opts::value< std::vector<std::string> >(),
            "url")
    ;

    // Visible.

    _visible.add (generic).add (config);

    // Command line options.

    opts::options_description cmdline_options;

    cmdline_options.add (generic).add (config).add (hidden);

    // Config file options.

    opts::options_description config_file_options;

    config_file_options.add (config);

    // Positional.

    opts::positional_options_description p;
    p.add ("url", -1);

    // Parse.

    store(opts::command_line_parser (argc,argv)
        .options (cmdline_options).positional (p).run (), _map);

    notify (_map);

    // Read config.

    std::ifstream ifs (_config_file.c_str ());

    if (ifs) {
        store (parse_config_file (ifs, config_file_options), _map);
        notify (_map);
    }

    _verify ();
}

const opts::variables_map& options::map () const
    { return _map; }

std::ostream& operator<<(std::ostream& os, const options& o)
    { return os << o._visible; }

void
options::_verify () {

    std::string empty;

    if (_map.count ("regexp")) {

        std::string s = _map["regexp"].as<std::string>();

        if (!cclive::re::match (s, empty)) {

            std::stringstream b;

            b << "invalid syntax (`" << s << "'), "
              << "expected perl syntax with --regexp,\n"
              << "\te.g.: \"/(\\w|\\s)/g\"";

            throw std::runtime_error (b.str ());
        }

    }

    if (_map.count ("subst")) {

        std::istringstream iss( _map["subst"].as<std::string>());
        std::vector<std::string> v;

        std::copy(
            std::istream_iterator<std::string >(iss),
            std::istream_iterator<std::string >(),
            std::back_inserter<std::vector<std::string> >(v)
        );

        foreach (std::string s, v) {

            if (!cclive::re::subst (s,empty)) {

                std::stringstream b;

                b << "invalid syntax (`" << s << "'), "
                  << "expected perl like syntax with --subst, e.g.:\n"
                  << "\ts/old/{new}/g\n"
                  << "\ts{old}/new/\n"
                  << "\ts(old)<new>/i\n"
                  << "\ts<old>{new}/";

                throw std::runtime_error (b.str ());
            }
        }
    }
}

} // End namespace.


