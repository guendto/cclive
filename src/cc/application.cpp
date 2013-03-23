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

#include <iomanip>
#include <vector>
#include <ctime>

#include <boost/algorithm/string/classification.hpp> // is_any_of
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>
#include <boost/random.hpp>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include <ccquvi>
#include <ccapplication>
#include <ccoptions>
#include <ccutil>
#include <cclog>
#include <ccre>

namespace cc
{

static boost::mt19937 _rng;

static void rand_decor()
{
  boost::uniform_int<> r(2,5);
  boost::variate_generator<boost::mt19937&, boost::uniform_int<> > v(_rng,r);

  const int n = v();
  for (int i=0; i<n; ++i) cc::log << ".";
}

static void handle_fetch(const quvi_word type, void*)
{
  rand_decor();
  if (type == QUVISTATUSTYPE_DONE)
    cc::log << " ";
}

static void print_done()
{
  cc::log << "done.\n";
}

static void handle_verify(const quvi_word type)
{
  rand_decor();
  if (type == QUVISTATUSTYPE_DONE)
    print_done();
}

static void handle_resolve(const quvi_word type)
{
  rand_decor();
  if (type == QUVISTATUSTYPE_DONE)
    cc::log << " ";
}

static int status_callback(long param, void *ptr)
{
  const quvi_word status = quvi_loword(param);
  const quvi_word type   = quvi_hiword(param);

  switch (status)
    {
    case QUVISTATUS_FETCH :
      handle_fetch(type,ptr);
      break;
    case QUVISTATUS_VERIFY:
      handle_verify(type);
      break;
    case QUVISTATUS_RESOLVE:
      handle_resolve(type);
      break;
    }

  cc::log << std::flush;

  return QUVI_OK;
}

template<class Iterator>
static Iterator make_unique(Iterator first, Iterator last)
{
  while (first != last)
    {
      Iterator next(first);
      last  = std::remove(++next, last, *first);
      first = next;
    }
  return last;
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

static const char depr_msg[] =
  "WARNING '--format {help,list}' are deprecated and will be removed "
  "in the later\nWARNING versions. Use '--print-streams' instead.";

static const char format_usage[] =
  "Usage:\n"
  "   --format arg                get format arg of media\n"
  "   --format list               print domains with formats\n"
  "   --format list arg           match arg to supported domain names\n"
  "Examples:\n"
  "   --format list youtube       print youtube formats\n"
  "   --format fmt34_360p         get format fmt34_360p of media";

static application::exit_status print_format_help()
{
  std::cout << format_usage << "\n" << depr_msg << std::endl;
  return application::ok;
}

typedef std::map<std::string,std::string> map_ss;

static void print_host(const map_ss::value_type& t)
{
  std::cout
      << t.first
      << ":\n  "
      << t.second
      << "\n"
      << std::endl;
}

namespace po = boost::program_options;

typedef std::vector<std::string> vst;

static application::exit_status
handle_format_list(const po::variables_map& map, const quvi::query& query)
{
  map_ss m = query.support();

  // -f list <pattern>

  if (map.count("url"))
    {
      const std::string arg0 = map["url"].as<vst>()[0];
      foreach (map_ss::value_type& t, m)
      {
        if (t.first.find(arg0) != std::string::npos)
          print_host(t);
      }
    }

  // -f list

  else
    {
      foreach (map_ss::value_type& t, m)
      {
        print_host(t);
      }
    }

  std::cout << depr_msg << std::endl;

  return application::ok;
}

static application::exit_status
print_streams(const quvi::query& query, const quvi::options &qopts,
              const vst& input)
{
  const size_t n = input.size();
  size_t i = 0;

  foreach (std::string url, input)
  {
    try
      {
        print_checking(++i,n);
        query.setup_curl();

        const std::string r = query.streams(url, qopts);
        print_done();

        if (cc::opts.flags.print_streams)
          {
            vst a;
            boost::split(a, r, boost::is_any_of("|,"));
            foreach (const std::string s, a)
            {
              cc::log << s << "\n";
            }
          }
        else
          cc::log << std::setw(10) << r << " : " << url << std::endl;
      }
    catch(const quvi::error& e)
      {
        print_quvi_error(e);
        return application::error;
      }
  }
  return application::ok;
}

static void read_from(std::istream& is, vst& dst)
{
  std::string s;
  char ch = 0;

  while (is.get(ch))
    s += ch;

  std::istringstream iss(s);
  std::copy(
    std::istream_iterator<std::string >(iss),
    std::istream_iterator<std::string >(),
    std::back_inserter<vst>(dst)
  );
}

static bool is_url(const std::string& s)
{
  return strstr(const_cast<char*>(s.c_str()), "://") != NULL;
}

static void parse_prefer_format(const std::string& url, std::string& fmt,
                                const po::variables_map& map)
{
  vst vb, va = map["prefer-format"].as<vst>();
  foreach (const std::string s, va)
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
                       const po::variables_map& map)
{
  std::string s = "default";
  if (map.count("stream"))
    s = map["stream"].as<std::string>();
  else if (map.count("format")) // --format takes precedence
    s = map["format"].as<std::string>();
  else
    {
      if (map.count("prefer-format"))
        parse_prefer_format(url, s, map);
    }
  qopts.stream = s;
}

extern char LICENSE[]; // cc/license.cpp

application::exit_status application::exec(int argc, char **argv)
{
  try
    {
      opts.exec(argc,argv);
    }
  catch(const std::exception& e)
    {
      std::clog << "error: " << e.what() << std::endl;
      return application::error;
    }

  const po::variables_map map = cc::opts.map();

  // Dump and terminate options.

  if (opts.flags.help)
    {
      std::cout << opts << std::flush;
      return application::ok;
    }
  else if (opts.flags.print_config)
    {
      opts.dump();
      return application::ok;
    }
  else if (opts.flags.version)
    {
      std::cout
          << "cclive "
#ifdef VN
          << VN
#else
          << PACKAGE_VERSION
#endif
          << " for " << CANONICAL_TARGET
          << "\n  libquvi "
          << quvi_version(QUVI_VERSION_LONG)
#ifdef HAVE_LIBQUVI_0_4_0
          << "\n  libquvi-scripts "
          << quvi_version(QUVI_VERSION_SCRIPTS)
#endif
          << std::endl;
      return application::ok;
    }
  else if (opts.flags.license)
    {
      std::cout << LICENSE << std::endl;
      return application::ok;
    }

  // --support

  quvi::query query; // Throws quvi::error caught in main.cpp

  if (opts.flags.support)
    {
      std::cout << quvi::support_to_s(query.support()) << std::flush;
      return application::ok;
    }

  // --format [<id> | [<help> | <list> [<pattern]]]

  if (map.count("format"))
    {
      const std::string format = map["format"].as<std::string>();

      if (format == "help")
        return print_format_help();

      else if (format == "list")
        return handle_format_list(map, query);
    }

  // Parse input.

  vst input;

  if (map.count("url") == 0)
    read_from(std::cin, input);
  else
    {
      vst args = map["url"].as< vst >();
      foreach(std::string arg, args)
      {
        if (!is_url(arg))
          {
            std::ifstream f(arg.c_str());
            if (f.is_open())
              read_from(f, input);
            else
              {
                std::clog
                    << "error: "
                    << arg
                    << ": "
                    << cc::perror("unable to open")
                    << std::endl;
              }
          }
        else
          input.push_back(arg);
      }
    }

  if (input.size() == 0)
    {
      std::clog << "error: no input urls" << std::endl;
      return application::error;
    }

  // Remove duplicates.

  input.erase(make_unique(input.begin(), input.end()), input.end());

  // Set up quvi.

  quvi::options qopts;
  qopts.resolve = ! opts.flags.no_resolve;
  qopts.statusfunc = status_callback;

  // Seed random generator.

  _rng.seed(static_cast<unsigned int>(std::time(0)));

  // Omit flag.

  bool omit = opts.flags.quiet;

  // Go to background.

#ifdef HAVE_FORK
  const bool background_given = opts.flags.background;

  if (background_given)
    {

      // (Boost) Throws std::runtime_error if fails.

      cc::go_background(map["log-file"].as<std::string>(), omit);
    }
#endif

  // Omit std output. Note that --background flips this above.

  cc::log.push(cc::omit_sink(omit));
  cc::log.setf(std::ios::fixed);

  // Print streams.

  if (opts.flags.print_streams || opts.flags.query_formats)
    return print_streams(query, qopts, input);

#if defined (HAVE_FORK) && defined (HAVE_GETPID)
  if (background_given)
    {
      cc::log
          << "Running in background (pid: "
          << static_cast<long>(getpid())
          << ")."
          << std::endl;
    }
#endif

  // For each input URL.

  const size_t n = input.size();
  size_t i = 0;

  const int max_retries  = map["max-retries"].as<int>();
  const int retry_wait   = map["retry-wait"].as<int>();

  exit_status es = ok;

  foreach(std::string url, input)
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
                set_stream(url, qopts, map);
                _curl = query.setup_curl();
                m = query.parse(url, qopts);
              }
            catch(const quvi::error& e)
              {
                if (e.cannot_retry())
                  throw e;
                else
                  print_quvi_error(e);
              }

            cc::get(m, _curl);
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

void application::_close()
{
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
