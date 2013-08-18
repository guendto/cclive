/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
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

#ifndef cc__progressbar_h
#define cc__progressbar_h

#include <ccinternal>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include <ccsig>
#include <cclog>

namespace cc
{

static inline std::string ieee_1541_symbol(double& n)
{
  typedef enum {Ki=1024, Mi=1048576, Gi=1073741824} ieee_1541;
  int i = 0;
  if (n >=Gi)
    {
      n /= Gi;
      i = 2;
    }
  else if (n >=Mi)
    {
      n /= Mi;
      i = 1;
    }
  else
    n /= Ki;
#define _s(n) #n
  static const char *r[] = {_s(Ki), _s(Mi), _s(Gi), NULL};
#undef _s
  return r[i];
}

namespace po = boost::program_options;
namespace pt = boost::posix_time;

struct progressbar : boost::noncopyable
{
  progressbar(const po::variables_map& vm, const quvi::media& qm,
              const double initial_length)
  {
    _length_bytes.content = qm.content_length();
    _length_bytes.initial = initial_length;

    _cntrs.time_started = pt::microsec_clock::universal_time();
    _cntrs.bytes_received = 0;
    _cntrs.num_dots = 0;

    _flags.failed = false;
    _flags.done = false;

    _opts.update_interval =
      vm[OPT__UPDATE_INTERVAL].as<update_interval>().value();

    ifn_optsw_given(vm, OPT__BACKGROUND)
      _opts.mode = vm[OPT__PROGRESSBAR].as<cc::progressbar_mode>().value();
    else
      _opts.mode = cc::progressbar_mode::dotline;
  }

  inline virtual ~progressbar() { finish(); }

  inline int update(double dlnow)
  {
    if (dlnow ==0 || _flags.failed)
      return 0;

    const pt::time_duration& td =
      pt::microsec_clock::universal_time() - _cntrs.time_started;

    if (_flags.done)
      dlnow = _length_bytes.content;
    else
      {
        const double diff =
          td.total_microseconds() - _cntrs.last_update.total_microseconds();

        if (diff < (_opts.update_interval*1e+6))
          return 0;
      }

    double elapsed = td.total_seconds();
    if (elapsed ==0)
      elapsed = 1e+4;

    const bool inactive = (dlnow ==0);
    double rate = (elapsed >0) ? (dlnow/elapsed):0;

    std::string eta, rate_symbol;
    if (! inactive)
      {
        if (! _flags.done)
          {
            const double left =
              (_length_bytes.content - (dlnow + _length_bytes.initial)) /rate;
            eta = eta_from_seconds(left+.5);
          }
        else
          {
            rate = (_length_bytes.content - _length_bytes.initial) / elapsed;
            eta = eta_from_seconds(elapsed);
          }
        rate_symbol = ieee_1541_symbol(rate);
      }
    else
      {
        rate_symbol = "--.-";
        eta = "--:--";
      }

    double size = dlnow;
    if (! _flags.done)
      size += _length_bytes.initial;

    int percent = 0;
    if (_length_bytes.content >0)
      {
        percent = (100.0 * size / _length_bytes.content);
        if (percent >= 100)
          percent = 100;
      }

    _cntrs.bytes_received = dlnow;
    _cntrs.last_update = td;

    switch (_opts.mode)
      {
      case cc::progressbar_mode::dotline:
        return render_dotline(rate_symbol, eta, percent, rate, size);
      case cc::progressbar_mode::simple:
        return render_simple(percent, size);
      case cc::progressbar_mode::normal:
      default:
        return render_normal(rate_symbol, eta, percent, rate, size);
      }
  }

  inline bool print_error(const long resp_code, const CURLcode rc,
                          std::string& errmsg)
  {
    _flags.failed = true;

    // Do not retry if this is an "unrecoverable" error.
    if (resp_code >= 400 && resp_code <= 500)
      {
        clear_line(errmsg);
        BOOST_THROW_EXCEPTION(std::runtime_error(errmsg));
      }

    bool r = false; // Retry by default.
    if (rc == 42)   // 42=Operation aborted by callback (libcurl).
      {
        errmsg = "sigusr1 received: interrupt current download";
        r = true;   // Skip - do not attempt to retry.
      }

    cc::log << clear_line((boost::format("error: %s") % errmsg).str())
            << std::endl;

    return r;
  }

private:
  inline void finish()
  {
    if (_length_bytes.content >0
        && ((_cntrs.bytes_received + _length_bytes.initial)
              >_length_bytes.content))
      {
        _length_bytes.content = _length_bytes.initial + _cntrs.bytes_received;
      }

    if (! _flags.failed)
      {
        _flags.done = true;
        update(-1);
      }
  }

  inline void dotline_render_dot() const
    { cc::log << "." << ( (_cntrs.num_dots%3 ==0) ? " ":"") << std::flush; }

  inline void dotline_render_info(const std::string& rate_symbol,
                                  const std::string& eta,
                                  const double percent,
                                  const double rate,
                                  double size,
                                  const bool nl=false) const
  {
    const std::string& size_symbol = ieee_1541_symbol(size);

    const std::string& s =
      (boost::format(" %6.1f%s  %6.1f%s/s  %4s  %3.0f%%%s")
        % size
        % size_symbol
        % rate
        % rate_symbol
        % eta
        % percent
        % (nl ? "\n":"")
      ).str();

    cc::log << s << std::flush;
  }

  inline int render_dotline(const std::string& rate_symbol,
                            const std::string& eta,
                            const double percent,
                            const double rate,
                            double size)
  {
    ++_cntrs.num_dots;

    if (_flags.done)
      {
        while (_cntrs.num_dots < 31)
          {
            dotline_render_dot();
            ++_cntrs.num_dots;
          }
        dotline_render_info(rate_symbol, eta, percent, rate, size);
        return 0;
      }

    if (_cntrs.num_dots >= 31)
      {
        dotline_render_info(rate_symbol, eta, percent, rate, size, true);
        _cntrs.num_dots = 0;
      }
    else
      dotline_render_dot();

    return 0;
  }

  inline int render_simple(const double percent, double size) const
  {
    const std::string& size_symbol = ieee_1541_symbol(size);

    const std::string& s =
      (boost::format(" %3.0f%%  %6.1f%s received\r")
        % percent
        % size
        % size_symbol
      ).str();

    cc::log << s << std::flush;
    return 0;
  }

  inline int render_normal(const std::string& rate_symbol,
                           const std::string& eta,
                           const double percent,
                           const double rate,
                           double size) const
  {
    const std::string& size_symbol = ieee_1541_symbol(size);

    const std::string& s =
      (boost::format(" %3.0f%%  %6.1f%s  %6.1f%s/s  %4s")
        % percent
        % size
        % size_symbol
        % rate
        % rate_symbol
        % eta
      ).str();

    const long l = s.length();
    size_t term_width;

    const size_t space_left = cc::sig::calc_term_spaceleft(l, term_width);
    if (term_width > space_left)
      {
        cc::log << render_meter(percent, space_left)
                << s << (_flags.done ? "\n":"\r")
                << std::flush;
      }
    return 0;
  }

  static inline std::string render_meter(const double percent,
                                         const long space_left)
  {
    std::ostringstream o;
    o << ("[");
    const int m = static_cast<int>(space_left * percent / 100.0);
    for (int i=0; (o.tellp() < space_left); ++i)
      o << (i<m ? "#" : "-");
    o << "]";
    return o.str();
  }

  static inline std::string eta_from_seconds(const double s)
  {
    const pt::time_duration& td = pt::seconds(s);
    return pt::to_simple_string(td);
  }

  static inline const std::string& clear_line(const std::string& errmsg)
  {
    const long term_width = cc::sig::query_term_width();
    std::ostringstream o;
    for (int i=0; (o.tellp() < term_width); ++i)
      o << " ";
    cc::log << o.str() << "\r" << std::flush;
    return errmsg;
  }

private:
  struct
  {
    double content; // total media content length
    double initial; // >0 if a resumed transfer
  } _length_bytes;
  struct
  {
    cc::progressbar_mode::mode_type mode;
    int update_interval;
  } _opts;
  struct
  {
    pt::time_duration last_update;
    pt::ptime time_started;
    double bytes_received;
    int num_dots;
  } _cntrs;
  struct
  {
    bool failed;
    bool done;
  } _flags;
};

} // namespace cc

#endif // cc__progressbar_h

// vim: set ts=2 sw=2 tw=72 expandtab:
