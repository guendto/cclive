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
#include <vector>
#include <ctime>

#include <boost/algorithm/string/classification.hpp> // is_any_of
#include <boost/algorithm/string/split.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <ccquvi>
#include <ccapplication>
#include <ccoptions>
#include <ccinput>
#include <ccutil>
#include <cclog>
#include <ccre>

namespace cc
{

static void handle_fetch(const quvi_word type, void*)
{
#ifdef HAVE_LIBQUVI_0_9
  if (type == QUVI_CALLBACK_STATUS_DONE)
#else
  if (type == QUVISTATUSTYPE_DONE)
#endif
    cc::log << " ";
}

static void print_done()
{
  cc::log << "done.\n";
}

static void handle_verify(const quvi_word type)
{
#ifdef HAVE_LIBQUVI_0_9
  if (type == QUVI_CALLBACK_STATUS_DONE)
#else
  if (type == QUVISTATUSTYPE_DONE)
#endif
    print_done();
}

static void handle_resolve(const quvi_word type)
{
#ifdef HAVE_LIBQUVI_0_9
  if (type == QUVI_CALLBACK_STATUS_DONE)
#else
  if (type == QUVISTATUSTYPE_DONE)
#endif
    cc::log << " ";
}

#ifdef HAVE_LIBQUVI_0_9
static void status_callback_pt9(const quvi_word status, const quvi_word type,
                                void *data, void *userdata)
{
  cc::log << ".";
  switch (status)
    {
    case QUVI_CALLBACK_STATUS_FETCH:
      handle_fetch(type, data);
      break;
    case QUVI_CALLBACK_STATUS_HTTP_QUERY_METAINFO:
      handle_verify(type);
      break;
    case QUVI_CALLBACK_STATUS_RESOLVE:
      handle_resolve(type);
      break;
    }
}
#else
static void status_callback_pt4(const quvi_word status, const quvi_word type,
                                void *data)
{
  cc::log << ".";
  switch (status)
    {
    case QUVISTATUS_FETCH:
      handle_fetch(type, data);
      break;
    case QUVISTATUS_VERIFY:
      handle_verify(type);
      break;
    case QUVISTATUS_RESOLVE:
      handle_resolve(type);
      break;
    }
}
#endif

#ifdef HAVE_LIBQUVI_0_9
static int status_callback(long status_type, void *data, void *userdata)
#else
static int status_callback(long status_type, void *data)
#endif
{
  const quvi_word status = quvi_loword(status_type);
  const quvi_word type   = quvi_hiword(status_type);

#ifdef HAVE_LIBQUVI_0_9
  status_callback_pt9(status, type, data, userdata);
#else
  status_callback_pt4(status, type, data);
#endif
  cc::log << std::flush;

  return QUVI_OK;
}

static void print_retrying(const int retry,
                           const int max_retries,
                           const int retry_wait)
{
  if (retry > 0)
    {
      cc::log
          << "Retrying "
          << retry
          << " of "
          << max_retries
          << " ... "
          << std::flush;

      cc::wait(retry_wait);
    }
}

static void print_checking(const int i, const int n)
{
  if (n > 1)  cc::log << "(" << i << " of " << n << ") ";
  cc::log << "Checking ... " << std::flush;
}

static void print_quvi_error(const quvi::error& e)
{
  cc::log << "libquvi: error: " << e.what() << std::endl;
}

namespace po = boost::program_options;
typedef std::vector<std::string> vst;

static std::string format_streams(const std::string& s)
{
  vst v;
  boost::split(v, s, boost::is_any_of("|,"));
  const size_t m = v.size();

  if (m ==0)
    v.push_back("default");

  std::stringstream r;
  r << "\n";

  size_t i = 0, c = 0;
  BOOST_FOREACH(const std::string& a, v)
  {
    r << boost::format("%|22s|") % a;
    ++c;
    if (++i ==3)
      {
        if (c <m)
          r << "\n";
        i = 0;
      }
  }
  return r.str();
}

static application::exit_status
print_streams(const quvi::query& query, const quvi::options &qopts,
              const vst& input_urls, const po::variables_map& vm)
{
  const size_t n = input_urls.size();
  size_t i = 0;

  BOOST_FOREACH(const std::string& url, input_urls)
  {
    try
      {
        print_checking(++i,n);
        query.setup_curl(vm);

        const std::string r = query.streams(url, qopts);
        print_done();

        cc::log << "streams (found):" << format_streams(r) << std::endl;
      }
    catch(const quvi::error& e)
      {
        print_quvi_error(e);
        return application::error;
      }
  }
  return application::ok;
}

static void parse_prefer_format(const std::string& url, std::string& fmt,
                                const po::variables_map& vm)
{
  vst vb, va = vm[OPT__PREFER_FORMAT].as<vst>();
  BOOST_FOREACH(const std::string& s, va)
  {
    boost::split(vb, s, boost::is_any_of(":"));
    if (vb.size() == 2)
      {
        // vb[0] = pattern
        // vb[1] = format
        if (cc::re::grep(vb[0], url))
          {
            fmt = vb[1];
            return;
          }
      }
    vb.clear();
  }
}

static void set_stream(const std::string& url, quvi::options& qopts,
                       const po::variables_map& vm)
{
  std::string r = vm[OPT__STREAM].as<std::string>();
  if (r == "default" && vm.count(OPT__PREFER_FORMAT))
    parse_prefer_format(url, r, vm);
  qopts.stream = r;
}

application::exit_status application::exec(int const argc, char const **argv)
{
  const boost::scoped_ptr<cc::options> o(new cc::options(argc, argv));
  const po::variables_map vm = o->values();

  // Parse input.

  const vst& input_urls = cc::input::parse(vm);
  const size_t n = input_urls.size();

  // Set up quvi.

  quvi::query query; // Throws quvi::error caught in main.cpp

  quvi::options qopts;
  qopts.useragent = vm[OPT__AGENT].as<std::string>(); /* libquvi 0.9+ */
  qopts.resolve = ! vm[OPT__NO_RESOLVE].as<bool>();
  qopts.statusfunc = status_callback;

  // Omit flag.

  bool omit = vm[OPT__QUIET].as<bool>();

  // Go to background.

#if defined(HAVE_WORKING_FORK) || defined(HAVE_WORKING_VFORK)
  const bool background_given = vm[OPT__BACKGROUND].as<bool>();
  if (background_given)
    cc::go_background(vm[OPT__LOG_FILE].as<std::string>(), omit);
#endif

  // Omit std output. Note that --background flips this above.

  cc::log.push(cc::omit_sink(omit));
  cc::log.setf(std::ios::fixed);

  // Print streams.

  if_optsw_given(vm, OPT__PRINT_STREAMS)
    return print_streams(query, qopts, input_urls, vm);

#if defined(HAVE_WORKING_FORK) || defined(HAVE_WORKING_VFORK)
  #ifdef HAVE_GETPID
  if (background_given)
    {
      cc::log
          << "Running in background (pid: "
          << static_cast<long>(getpid())
          << ")."
          << std::endl;
    }
  #endif
#endif

  // For each input URL.

  size_t i = 0;

  const int max_retries  = vm[OPT__MAX_RETRIES].as<cc::max_retries>().value();
  const int retry_wait   = vm[OPT__RETRY_WAIT].as<cc::retry_wait>().value();

  exit_status es = ok;

  BOOST_FOREACH(const std::string& url, input_urls)
  {
    ++i;

    try
      {
        int retry = 0;

        while (retry <= max_retries)
          {
            print_retrying(retry, max_retries, retry_wait);
            ++retry;

            print_checking(i, n);
            quvi::media m;

            try
              {
                set_stream(url, qopts, vm);
                _curl = query.setup_curl(vm);
                m = query.parse(url, qopts);
              }
            catch(const quvi::error& e)
              {
                if (e.cannot_retry())
                  throw e;
                else
                  print_quvi_error(e);
              }

            cc::get(m, _curl, vm);
            break; // Stop retrying.
          }
        es = ok;
      }

    catch(const quvi::error& e)
      {
        print_quvi_error(e);
        es = application::error;
      }

    catch(const std::runtime_error& e)
      {
        cc::log << "error: " << e.what() << std::endl;
        es = application::error;
      }
  }
  return es;
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
