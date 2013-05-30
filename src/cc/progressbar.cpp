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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem.hpp>

#include <ccoptions>
#include <ccquvi>
#include <ccfile>
#include <cclog>
#include <ccprogressbar>

#if defined(SIGWINCH) && defined(TIOCGWINSZ)
#define WITH_RESIZE
#endif

namespace cc
{
#ifdef WITH_RESIZE
static volatile sig_atomic_t recv_sigwinch;

static void handle_sigwinch(int s)
{
  recv_sigwinch = 1;
}

static size_t get_term_width()
{
  const int fd = fileno(stderr);

  winsize wsz;

  if (ioctl(fd, TIOCGWINSZ, &wsz) < 0)
    return 0;

  return wsz.ws_col;
}
#endif // WITH_RESIZE

namespace po = boost::program_options;

progressbar::progressbar(const file& f, const quvi::media& m)
  : _update_interval(1),
    _expected_bytes(m.content_length()),
    _initial_bytes(f.initial_length()),
    _time_started(0),
    _last_update(0),
    _term_width(0),
    _dot_count(0),
    _count(0),
    _width(0),
    _file(f),
    _done(false),
    _mode(normal)
{
  if (_initial_bytes > _expected_bytes)
    _expected_bytes = _initial_bytes;

#ifdef WITH_RESIZE
  signal(SIGWINCH, handle_sigwinch);

  if (!_term_width || recv_sigwinch)
    {
      _term_width = get_term_width();

      if (!_term_width)
        _term_width = default_term_width;
    }
#else
  _term_width = default_term_width;
#endif

  _width = _term_width;

  const po::variables_map map = cc::opts.map();
  time(&_time_started);

  if (opts.flags.background)
    _mode = dotline;
  else
    {
      const std::string s = map["progressbar"].as<std::string>();
      if (s == "simple")
        _mode = simple;
      else if (s == "dotline")
        _mode = dotline;
    }

  _update_interval = fabs(map["update-interval"].as<double>());
}

static double to_mb(const double bytes)
{
  return bytes/(1024*1024);
}

namespace pt = boost::posix_time;

static std::string to_s(const int secs)
{
  pt::time_duration td = pt::seconds(secs);
  return pt::to_simple_string(td);
}

static std::string to_unit(double& rate)
{
  std::string units = "K/s";
  if (rate >= 1024.0*1024.0*1024.0)
    {
      rate /= 1024.0*1024.0*1024.0;
      units = "G/s";
    }
  else if (rate >= 1024.0*1024.0)
    {
      rate /= 1024.0*1024.0;
      units = "M/s";
    }
  else
    rate /= 1024.0;
  return units;
}

namespace fs = boost::filesystem;

int progressbar::update(double now)
{
  time_t tnow;

  time(&tnow);

  const time_t elapsed = tnow - _time_started;

  bool force_update = false;

#ifdef WITH_RESIZE
  if (recv_sigwinch && _mode == normal)
    {
      const size_t old_term_width = _term_width;

      _term_width = get_term_width();

      if (!_term_width)
        _term_width = default_term_width;

      if (_term_width != old_term_width)
        {
          _width = _term_width;
          force_update = true;
        }

      recv_sigwinch = 0;
    }
#endif // WITH_RESIZE

  const bool inactive = now == 0;

  if (!_done)
    {
      if ((elapsed - _last_update) < _update_interval
          && !force_update)
        {
          return 0;
        }
    }
  else
    now = _expected_bytes;

  // Current size.

  const double size =
    (!_done)
    ? _initial_bytes + now
    : now;

  std::stringstream size_s;

  size_s.setf(std::ios::fixed);

  size_s
      << std::setprecision(1)
      << to_mb(size)
      << "M";

  // Rate.

  double rate = elapsed ? (now/elapsed):0;

  std::stringstream rate_s, eta_s;

  rate_s.setf(std::ios::fixed);
  eta_s.setf(std::ios::fixed);

  if (!inactive)
    {
      // ETA.

      std::string eta;

      if (!_done)
        {
          const double left =
            (_expected_bytes - (now + _initial_bytes)) / rate;

          eta = to_s(static_cast<int>(left+0.5));
        }
      else
        {
          rate = (_expected_bytes - _initial_bytes) / elapsed;
          eta  = to_s(elapsed);
        }

      std::string unit = to_unit(rate);

      rate_s
          << std::setw(4)
          << std::setprecision(1)
          << rate
          << unit;

      eta_s
          << std::setw(6)
          << eta;
    }
  else   // ETA: inactive (default).
    {
      rate_s << "--.-K/s";
      eta_s << "--:--:--";
    }

  // Percent.

  std::stringstream percent_s;
  int percent = 0;

  if (_expected_bytes > 0)
    {
      percent = static_cast<int>(100.0*size/_expected_bytes);

      if (percent < 100)
        percent_s << std::setw(2) << percent << "%";
      else
        percent_s << "100%";
    }

  // Filename.

  fs::path p = fs::system_complete(_file.path());

#if BOOST_FILESYSTEM_VERSION > 2
  std::string fname = p.filename().string();
#else
  std::string fname = p.filename();
#endif

  switch (_mode)
    {
    default:
    case  normal:
      _normal(size_s, rate_s, eta_s, percent, percent_s);
      break;
    case dotline:
      _dotline(size_s, rate_s, eta_s, percent_s);
      break;
    case simple:
      _simple(size_s, percent_s);
      break;
    }

  _last_update = elapsed;
  _count       = now;

  return 0;
}

void progressbar::_normal(const std::stringstream& size_s,
                          const std::stringstream& rate_s,
                          const std::stringstream& eta_s,
                          const int percent,
                          const std::stringstream& percent_s)
{
  std::stringstream info;

  info.setf(std::ios::fixed);

  info
      << "  "
      << percent_s.str()
      << "  "
      << std::setw(4)
      << size_s.str()
      << "  "
      << rate_s.str()
      << "  "
      << eta_s.str();

  const size_t space_left = _width - info.str().length() - 1;

  if (_width <= space_left)
    return;

  std::stringstream bar;

  _render_meter(bar, percent, space_left);

  bar << info.str();

  cc::log << bar.str() << "\r" << std::flush;
}

void progressbar::_dotline(const std::stringstream& size_s,
                           const std::stringstream& rate_s,
                           const std::stringstream& eta_s,
                           const std::stringstream& percent_s)
{
#define details \
    "  " \
    << std::setw(6) \
    << size_s.str() \
    << "  " \
    << rate_s.str() \
    << "  " \
    << eta_s.str() \
    << "  " \
    << percent_s.str()

#define dot \
    do { \
        cc::log \
            << "." \
            << (_dot_count % 3 == 0 ? " ":"") \
            << std::flush; \
    } while (0)

  ++_dot_count;

  if (_done)
    {
      for (; _dot_count < 31; ++_dot_count) dot;
      cc::log << details << std::flush;
      return;
    }
  if (_dot_count >= 31)
    {
      cc::log << details << std::endl;
      _dot_count = 0;
    }
#undef details
  else
    dot;
#undef dot
}

void progressbar::_simple(const std::stringstream& size_s,
                          const std::stringstream& percent_s) const
{
  cc::log << percent_s.str()
          << " - "
          << size_s.str()
          << " received\r"
          << std::flush;
}

void progressbar::_render_meter(std::stringstream& bar,
                                const int percent,
                                const size_t space_left)
{
  const int m = static_cast<int>(space_left*percent/100.0);
  bar << "[";
  int i = 0;
  while (bar.str().length() < space_left)
    {
      bar << (i<m ? "#":"-");
      ++i;
    }
  bar << "]";
}

void progressbar::finish()
{
  if (_expected_bytes > 0
      && _count + _initial_bytes > _expected_bytes)
    {
      _expected_bytes = _initial_bytes + _count;
    }

  _done = true;
  update(-1);
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
