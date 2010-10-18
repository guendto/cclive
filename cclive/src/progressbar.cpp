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

#include <iomanip>
#include <cstdio>
#include <ctime>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "quvicpp/quvicpp.h"

#include "cclive/options.h"
#include "cclive/file.h"
#include "cclive/log.h"
#include "cclive/progressbar.h"

#if defined(SIGWINCH) && defined(TIOCGWINSZ)
#define WITH_RESIZE
#endif

namespace cclive {

#ifdef WITH_RESIZE

static volatile sig_atomic_t recv_sigwinch;

static void
handle_sigwinch (int s) {
    recv_sigwinch = 1;
    signal(SIGWINCH, handle_sigwinch);
}

static int
get_term_width () {

    const int fd = fileno (stderr);

    winsize wsz;

    if (ioctl (fd, TIOCGWINSZ, &wsz) < 0)
        return 0;

    return wsz.ws_col;
}
#endif // WITH_RESIZE

namespace po = boost::program_options;

progressbar::progressbar (
    const file& f,
    const quvicpp::link& l,
    const options& opts)
    : _update_interval (.2),
      _expected_bytes (l.length ()),
      _initial_bytes  (f.initial_length ()),
      _time_started (0),
      _last_update (0),
      _term_width (0),
      _dot_count  (0),
      _count (0),
      _width (0),
      _file (f),
      _done (false),
      _mode (normal)
{
    if (_initial_bytes > _expected_bytes)
        _expected_bytes = _initial_bytes;

#ifdef WITH_RESIZE
    signal (SIGWINCH, handle_sigwinch);

    if (!_term_width || recv_sigwinch) {

        _term_width = get_term_width ();

        if (!_term_width)
            _term_width = default_term_width;
    }
#else
    _term_width = default_term_width;
#endif

    _width = _term_width-1; // Don't use the last column.

    time (&_time_started);

    const po::variables_map map  = opts.map ();

    if (map.count ("background")) {
        cclive::log << _file.to_s (l) << std::endl;
        _mode = dotline;
    }

    _update_interval = map["update-interval"].as<double>();
}

static double
to_mb (const double bytes) { return bytes/(1024*1024); }

namespace pt = boost::posix_time;

static std::string
to_s (const int secs) {
    pt::time_duration td = pt::seconds (secs);
    return pt::to_simple_string (td);
}

static std::string
to_unit (double& rate) {
    int i = 0;
    if (rate < 1024*1024) {
        rate /= 1024;
    }
    else if (rate < 1024*1024) {
        rate /= 1024*1024;
        i = 1;
    }
    else if (rate < 1024*1024*1024) {
        rate /= 1024*1024*1024;
        i = 2;
    }
    static const char *units[] = {"K/s", "M/s", "G/s"};
    return units[i];
}

namespace fs = boost::filesystem;

void
progressbar::update (double now) {

    time_t tnow;

    time (&tnow);

    const time_t elapsed = tnow - _time_started;

    bool force_update = false;

#ifdef WITH_RESIZE
    if (recv_sigwinch && _mode == normal) {

        const int old_width = _term_width;

        _term_width = get_term_width ();

        if (!_term_width)
            _term_width = default_term_width;

        if (_term_width != old_width) {
            _width = _term_width-1;
            force_update = true;
        }

        recv_sigwinch = 0;
    }
#endif // WITH_RESIZE

    if (!_done) {
        if ((elapsed - _last_update) < _update_interval
            && !force_update)
        {
            return;
        }
    }
    else
        now = _expected_bytes;

    // Current size.

    const double size = _initial_bytes + now;

    std::stringstream size_s;

    size_s.setf (std::ios::fixed);

    size_s
        << std::setprecision (1)
        << to_mb (!_done ? size:now)
        << "M";

    // Rate.

    double rate = elapsed ? (now/elapsed):0;

    std::stringstream rate_s, eta_s;

    rate_s.setf (std::ios::fixed);
    eta_s.setf (std::ios::fixed);

    if (rate > 0) {

        // ETA.

        std::string eta;

        if (!_done) {

            const double left =
                (_expected_bytes - (now + _initial_bytes)) / rate;

            eta = to_s (static_cast<int>(left+0.5));
        }
        else {
            rate = (_expected_bytes - _initial_bytes) / elapsed;
            eta  = to_s (elapsed);
        }

        std::string unit = to_unit (rate);

        rate_s
            << std::setw (4)
            << std::setprecision (1)
            << rate
            << unit;

        eta_s
            << std::setw (6)
            << eta;
    }
    else { // ETA: inactive (default).
        rate_s << "--.-K/s";
        eta_s << "--:--:--";
    }

    // Percent.

    std::stringstream percent_s;

    if (_expected_bytes > 0) {

        const int percent =
            static_cast<int>(100.0*size/_expected_bytes);

        if (percent < 100)
            percent_s << std::setw(2) << percent << "%";
        else
            percent_s << "100%";
    }

    // Filename.

    fs::path p = fs::system_complete (_file.path ());

    std::string fname = p.filename ();

    switch (_mode) {
    default:
    case  normal:  _normal  (size_s, rate_s, eta_s, percent_s, fname); break;
    case dotline:  _dotline (size_s, rate_s, eta_s, percent_s, fname); break;
    }

    _last_update = elapsed;
    _count       = now;
}

void
progressbar::_normal (
    const std::stringstream& size_s,
    const std::stringstream& rate_s,
    const std::stringstream& eta_s,
    const std::stringstream& percent_s,
    const std::string& fname)
{
    std::stringstream tmp;

    tmp.setf (std::ios::fixed);

    // Size.

    tmp << "  "
        << std::setw (4)
        << size_s.str ();

    // Rate, ETA.

    tmp << "  "
        << rate_s.str ()
        << "  "
        << eta_s.str ();

    // Percent.

    tmp << "  "
        << percent_s.str ();

    // Filename. Slice and dice.

    int l = _width - tmp.str ().length () - 2;

    if (_width > default_term_width)
        l += _width - default_term_width;

    std::stringstream b;

    b << fname.substr (0,l);

    // Pad to max. terminal width (filename <-> other details).

    l = _width - tmp.str ().length ();

    for (int i = b.str ().length (); i<l; ++i)
        b << " ";

    b << tmp.str ();

    // Print.

    cclive::log << b.str () << "\r" << std::flush;
}

void
progressbar::_dotline (
    const std::stringstream& size_s,
    const std::stringstream& rate_s,
    const std::stringstream& eta_s,
    const std::stringstream& percent_s,
    const std::string& fname)
{
    if (++_dot_count >= 31) {
        cclive::log
            << "  "
            << std::setw (6)
            << size_s.str ()
            << "  "
            << rate_s.str ()
            << "  "
            << eta_s.str ()
            << "  "
            << percent_s.str ()
            << std::endl;
        _dot_count = 0;
    }
    else {
        cclive::log << "." << (_dot_count % 3 == 0 ? " ":"") << std::flush;
    }
}

void
progressbar::finish () {

    if (_expected_bytes > 0
        && _count + _initial_bytes > _expected_bytes)
    {
        _expected_bytes = _initial_bytes + _count;
    }

    _done = true;
    update (-1);
}

} // End namespace.


