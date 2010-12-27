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

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iomanip>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include <curl/curl.h>

#include "quvicpp/quvicpp.h"

#include "cclive/options.h"
#include "cclive/progressbar.h"
#include "cclive/re.h"
#include "cclive/error.h"
#include "cclive/log.h"
#include "cclive/file.h"

namespace cclive {

file::file ()
    : _initial_length(0)
    { }

file::file (
    const quvicpp::video& video,
    const quvicpp::link& link,
    const int n,
    const options& opts)
    : _initial_length(0)
    { _init (video, link, n, opts); }

file::file (const file& f)
    : _initial_length(0)
    { _swap(f); }

file&
file::operator=(const file& f) {
    if (this != &f) _swap(f);
    return *this;
}

void
file::_swap (const file& f) {
    _name           = f._name;
    _path           = f._path;
    _initial_length = f._initial_length;
}

#define E "server response code %ld, expected 200 or 206 (conn_code=%ld)"

static std::string
format_unexpected_http_error (const long resp_code, const long conn_code)
    { return (boost::format (E) % resp_code % conn_code).str (); }

#undef E

#define E "%s (curl_code=%ld, resp_code=%ld, conn_code=%ld)"

static std::string
format_error (
    const CURLcode curl_code,
    const long resp_code,
    const long conn_code)
{
    const std::string e = curl_easy_strerror (curl_code);
    return (boost::format (E) % e % curl_code % resp_code % conn_code).str ();
}

#undef E

static size_t
write_cb (void *data, size_t size, size_t nmemb, void *ptr) {
    std::ofstream *o   = reinterpret_cast<std::ofstream*>(ptr);
    const size_t rsize = size*nmemb;
    o->write (static_cast<char*>(data), rsize);
    o->flush ();
    return rsize;
}

static int
progress_cb (void *ptr, double, double now, double, double) {
    reinterpret_cast<progressbar*>(ptr)->update (now);
    return 0;
}

namespace po = boost::program_options;

bool
file::write (
    const quvicpp::query& q,
    const quvicpp::link& l,
    const options& opts) const
{
    CURL *curl = q.curlHandle();

    curl_easy_setopt (curl, CURLOPT_URL, l.url().c_str());
    curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, write_cb);

    const po::variables_map map  = opts.map();
    std::ios_base::openmode mode = std::ofstream::binary;

    if ( map.count ("overwrite") )
        mode |= std::ofstream::trunc;
    else {
        if ( _should_continue() )
            mode |= std::ofstream::app;
    }

    std::ofstream out( _path.c_str(), mode );

    if ( out.fail() ) {

        std::string s = _path + ": ";

        if (errno)
            s += cclive::perror ();
        else
            s += "unknown file open error";

        throw std::runtime_error (s);
    }

    curl_easy_setopt (curl, CURLOPT_WRITEDATA, &out);

    curl_easy_setopt (curl, CURLOPT_ENCODING, "identity");
    curl_easy_setopt (curl, CURLOPT_HEADER, 0L);

    progressbar pb (*this, l, opts);
    curl_easy_setopt (curl, CURLOPT_PROGRESSDATA, &pb);
    curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt (curl, CURLOPT_PROGRESSFUNCTION, progress_cb);

    curl_off_t resume_from = static_cast<curl_off_t>(_initial_length);
    curl_easy_setopt (curl, CURLOPT_RESUME_FROM_LARGE, resume_from);

    curl_easy_setopt (curl, CURLOPT_CONNECTTIMEOUT,
        map["connect-timeout"].as<int>());

    curl_easy_setopt (curl, CURLOPT_TIMEOUT,
        map["transfer-timeout"].as<int>());

    curl_easy_setopt (curl, CURLOPT_DNS_CACHE_TIMEOUT,
        map["dns-cache-timeout"].as<int>());

    curl_off_t throttle = map["throttle"].as<int>()*1024;
    curl_easy_setopt (curl, CURLOPT_MAX_RECV_SPEED_LARGE, throttle);

    const CURLcode rc = curl_easy_perform (curl);

    // Restore curl settings.

    curl_easy_setopt (curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt (curl, CURLOPT_RESUME_FROM_LARGE, 0L);
    curl_easy_setopt (curl, CURLOPT_MAX_RECV_SPEED_LARGE,
        static_cast<curl_off_t>(0L));

    out.flush();
    out.close();

    long resp_code = 0;
    long conn_code = 0;

    curl_easy_getinfo (curl,
        CURLINFO_RESPONSE_CODE, &resp_code);

    curl_easy_getinfo (curl,
        CURLINFO_HTTP_CONNECTCODE, &conn_code);

    std::string error;

    if (CURLE_OK == rc) {
        if (resp_code != 200 && resp_code != 206)
            error = format_unexpected_http_error (resp_code, conn_code);
    }
    else {
        error = format_error (rc, resp_code, conn_code);
    }

    if ( !error.empty() ) {

        cclive::log << std::endl;

        if (resp_code >= 400 && resp_code <= 500)
            throw std::runtime_error (error);
        else
            cclive::log << "error: " << error << std::endl;

        return false; // Retry.
    }

    pb.finish();

    cclive::log << std::endl;

    return true;
}

double file::initial_length   () const { return _initial_length; }
const std::string& file::path () const { return _path; }
bool file::_should_continue   () const { return _initial_length > 0; }

static double
to_mb (const double bytes) { return bytes/(1024*1024); }

std::string
file::to_s (const quvicpp::link& link) const {

    const double length = to_mb (link.length ());

    boost::format fmt = boost::format("%s  %.2fM  [%s]")
        % _name % length % link.content_type ();

    return fmt.str();
}

#define E_NOTHINGTODO "video retrieved completely already"

namespace fs = boost::filesystem;

static fs::path
output_dir (const po::variables_map& map) {

    fs::path dir;

    if (map.count ("output-dir"))
        dir = map["output-dir"].as<std::string>();

    else {
#ifdef HAVE_GETCWD
        char tmp[PATH_MAX];
        getcwd (tmp, sizeof (tmp));
        dir = tmp;
#endif
    }

    return fs::system_complete (dir);
}

void
file::_init (
    const quvicpp::video& video,
    const quvicpp::link& link,
    const int n,
    const options& opts)
{
    const po::variables_map map = opts.map();

    if (map.count("output-file")) {

        // Overrides --filename-format.

        fs::path p = output_dir (map);

        p /= map["output-file"].as<std::string>();

        _name           = p.filename();
        _path           = p.string();
        _initial_length = file::exists (_path);

        if ( _initial_length >= link.length() && !map.count("overwrite") )
            throw std::runtime_error (E_NOTHINGTODO);
    }

    else {

        std::string title  = video.title();

        // Apply --regexp to title.

        cclive::re::match (map["regexp"].as<std::string>(), title);

        cclive::re::trim (title);

        // --filename-format

        std::string fname_format =
            map["filename-format"].as<std::string>();

        boost::format fmt;

        fmt = boost::format("s{%%i}{%1%}g") % video.id();
        cclive::re::subst (fmt.str(), fname_format);

        fmt = boost::format("s{%%t}{%1%}g") % title;
        cclive::re::subst (fmt.str(), fname_format);

        fmt = boost::format("s{%%s}{%1%}g") % link.suffix();
        cclive::re::subst (fmt.str(), fname_format);

        fmt = boost::format("s{%%h}{%1%}g") % video.host();
        cclive::re::subst (fmt.str(), fname_format);

        if (map.count("subst")) {

            std::istringstream iss(map["subst"].as<std::string>());
            std::vector<std::string> v;

            std::copy(
                std::istream_iterator<std::string >(iss),
                std::istream_iterator<std::string >(),
                std::back_inserter<std::vector<std::string> >(v)
            );

            foreach (std::string s, v)
                cclive::re::subst (s, fname_format);
        }

        std::stringstream b;

        b << fname_format;

        // A multi-segment video.

        if (n > 1) b << "_" << n;

        // Output dir.

        const fs::path out_dir = output_dir (map);
        fs::path output_file   = out_dir;

        output_file /= b.str ();

        // Path, name.

        fs::path p = fs::system_complete (output_file);

        _name = p.filename();
        _path = p.string();

        if ( !map.count ("overwrite") ) {

            for (int i=0; i<INT_MAX; ++i) {

                _initial_length = file::exists (_path);

                if (!_initial_length)
                    break;

                else if (_initial_length >= link.length())
                    throw std::runtime_error (E_NOTHINGTODO);

                else {
                    if (map.count("continue"))
                        break;

                }

                boost::format fmt =
                    boost::format ("%1%.%2%") % b.str() % i;

                p     = out_dir;
                p    /= fs::system_complete (fmt.str ());
                _name = p.filename();
                _path = p.string();
            }
        }
    }

    if ( map.count("overwrite") )
        _initial_length = 0;
}

#undef E_NOTHINGTODO

double
file::exists (const std::string& path) {

    fs::path p( fs::system_complete(path) );

    double size = 0;

    if (fs::exists(p))
        size = static_cast<double>(fs::file_size(p));

    return size;
}

} // End namespace.


