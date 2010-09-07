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

#include <ctime>

#include <boost/foreach.hpp>
#include <boost/random.hpp>

#include <curl/curl.h>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include "cclive/application.h"
#include "cclive/get.h"
#include "cclive/error.h"
#include "cclive/log.h"
#include "cclive/wait.h"
#include "cclive/background.h"

namespace cclive {

static boost::mt19937 _rng;

static void
rand_decor () {

    boost::uniform_int<> r(2,5);

    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > v(_rng,r);

    const int n = v();

    for (int i=0; i<n; ++i) cclive::log << ".";
}

static void
handle_fetch (const quvi_word type, void*) {
    rand_decor();
    if (type == QUVISTATUSTYPE_DONE)
        cclive::log << " ";
}

static void
handle_verify (const quvi_word type) {
    rand_decor();
    if (type == QUVISTATUSTYPE_DONE)
        cclive::log << "done.\n";
}

static int
status_callback (long param, void *ptr) {

    const quvi_word status = quvi_loword(param);
    const quvi_word type   = quvi_hiword(param);

    switch (status) {
    case QUVISTATUS_FETCH : handle_fetch (type,ptr); break;
    case QUVISTATUS_VERIFY: handle_verify(type); break;
    }

    cclive::log << std::flush;

    return QUVI_OK;
}

template<class Iterator>
static Iterator
make_unique (Iterator first, Iterator last) {
    while (first != last) {
        Iterator next (first);
        last  = std::remove (++next, last, *first);
        first = next;
    }
    return last;
}

static void
print_retrying (
    const int retry,
    const int max_retries,
    const int retry_wait)
{
    if (retry > 0) {

        std::clog
            << "Retrying "
            << retry
            << " of "
            << max_retries
            << " ... "
            << std::flush;

        cclive::wait (retry_wait);
    }
}

static void
print_checking (const int i, const int n) {

    if (n > 1)
        cclive::log << "(" << i << " of " << n << ") ";

    cclive::log << "Checking ";
}

static void
print_quvi_error (const quvicpp::error& e)
    { std::clog << "libquvi: error: " << e.what() << std::endl; }

static void
print_error (const std::exception& e)
    { std::clog << "error: " << e.what() << std::endl; }

static void
check_quvi_error (const quvicpp::error& e) {

    const long resp_code = e.response_code ();

    if (resp_code >= 400 && resp_code <= 500)
        throw e;

    else {

        switch (e.quvi_code ()) {

        case QUVI_CURL:
            print_quvi_error (e);
            break; // Retry.

        default:
            throw e;
        }

    }

}

extern char LICENSE[]; // cclive/license.cpp

int
application::exec (int argc, char **argv) {

    try   { _opts.exec(argc,argv); }

    catch (const std::exception& e) {
        print_error (e);
        return invalid_option;
    }

    const boost::program_options::variables_map map = _opts.map();

    // Dump and terminate options.

    if (map.count("help")) {
        std::clog << _opts;
        return ok;
    }

    if (map.count("version")) {
        std::clog
            << "cclive version "
            << VERSION_LONG
            << "\n"
            << "libquvi version "
            << quvi_version (QUVI_VERSION_LONG)
            << std::endl;
        return ok;
    }

    if (map.count("license")) {
        std::clog << LICENSE << std::endl;
        return ok;
    }

    // Set up quvicpp.

    quvicpp::query query; // Throws quvicpp::error caught in main.cpp .

    if (map.count("support")) {
        std::clog << quvicpp::support_to_s (query.support ()) << std::flush;
        return ok;
    }

    // Parse input.

    std::vector<std::string> input;

    if (map.count("url"))
        input = map["url"].as< std::vector<std::string> >();
    else
        _read_stdin (input);

    // Remove duplicates.

    input.erase (make_unique (input.begin(), input.end()), input.end());

    // Turn on libcurl verbose output.

    if (map.count("verbose-libcurl"))
        curl_easy_setopt (query.curlHandle(), CURLOPT_VERBOSE, 1L);

    // Set up quvicpp.

    _tweak_curl_opts(query,map);

    quvicpp::options qopts;
    qopts.statusfunc (status_callback);
    qopts.format     (map["format"].as<std::string>());

    // Seed random generator.

    _rng.seed ( static_cast<unsigned int>(std::time(0)) );

    // Omit flag.

    bool omit = map.count ("quiet");

    // Go to background.

    if (map.count ("background")) {

        // Throws std::runtime_error if fails.

        cclive::go_background (map["log-file"].as<std::string>(), omit);
    }

    // Omit std output. Note that --background flips this above.

    cclive::log.push (cclive::omit_sink (omit));

    // For each input URL.

    const size_t n = input.size();
    size_t i = 0;

    const int max_retries  = map["max-retries"].as<int>();
    const int retry_wait   = map["retry-wait"].as<int>();

    foreach(std::string url, input) {

        ++i;

        try {

            int retry = 0;

            while (retry <= max_retries) {

                print_retrying (retry, max_retries, retry_wait);

                ++retry;

                print_checking (i, n);

                quvicpp::video v;

                try
                    { v = query.parse (url, qopts); }

                catch (const quvicpp::error& e)
                    { check_quvi_error (e); }

                cclive::get (query, v, _opts);

                break; // Stop retrying.
            }
        }

        catch (const quvicpp::error& e)
            { print_quvi_error (e); }

        catch (const std::runtime_error& e)
            { print_error (e); }
    }

    return ok;
}

void
application::_read_stdin (std::vector<std::string>& dst) {

    std::string s;
    char ch = 0;

    while (std::cin.get(ch))
        s += ch;

    std::istringstream iss(s);
    std::copy(
        std::istream_iterator<std::string >(iss),
        std::istream_iterator<std::string >(),
        std::back_inserter<std::vector<std::string> >(dst)
    );
}

void
application::_tweak_curl_opts (
    const quvicpp::query& query,
    const boost::program_options::variables_map& map)
{
    CURL *curl = query.curlHandle();

    curl_easy_setopt(curl, CURLOPT_USERAGENT,
        map["agent"].as<std::string>().c_str());

    if (map.count("verbose-curl"))
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    if (map.count("proxy")) {
        curl_easy_setopt(curl, CURLOPT_PROXY,
            map["proxy"].as<std::string>().c_str());
    }

    if (map.count("no-proxy"))
        curl_easy_setopt(curl, CURLOPT_PROXY, "");

    if (map.count("throttle")) {
        curl_off_t limit = map["throttle"].as<int>()*1024;
        curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, limit);
    }

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,
        map["connect-timeout"].as<int>());
}

} // End namespace.


