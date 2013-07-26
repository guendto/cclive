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

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#if defined (HAVE_SIGNAL_H) && defined (HAVE_SIGNAL)
#define WITH_SIGNAL
#endif

#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include <curl/curl.h>
#include <pcrecpp.h>

#include <ccquvi>
#include <ccoptions>
#include <ccprogressbar>
#include <ccre>
#include <ccutil>
#include <cclog>
#include <ccfile>

namespace cc
{

namespace po = boost::program_options;

file::file(const quvi::media& media)
  : _initial_length(0), _nothing_todo(false)
{
  try
    {
      _init(media);
    }
  catch (const cc::nothing_todo_error&)
    {
      _nothing_todo = true;
    }
}

#define E "server response code %ld, expected 200 or 206 (conn_code=%ld)"

static std::string format_unexpected_http_error(
  const long resp_code,
  const long conn_code)
{
  return (boost::format(E) % resp_code % conn_code).str();
}

#undef E

#define E "%s (curl_code=%ld, resp_code=%ld, conn_code=%ld)"

static std::string format_error(const CURLcode curl_code,
                                const long resp_code,
                                const long conn_code)
{
  const std::string e = curl_easy_strerror(curl_code);
  return (boost::format(E) % e % curl_code % resp_code % conn_code).str();
}

#undef E

static std::string io_error(const std::string& fpath)
{
  std::string s = fpath + ": ";
  if (errno)
    s += cc::perror();
  else
    s += "unknown i/o error";
  return (s);
}

static std::string io_error(const cc::file& f)
{
  return io_error(f.path());
}

class write_data
{
public:
  inline write_data(cc::file *f):o(NULL), f(f) { }
  inline ~write_data()
  {
    if (o == NULL)
      return;

    o->flush();
    o->close();

    delete o;
    o = NULL;
  }
  inline void open_file()
  {
    std::ios_base::openmode mode = std::ofstream::binary;

    if (cc::opts.flags.overwrite)
      mode |= std::ofstream::trunc;
    else
      {
        if (f->should_continue())
          mode |= std::ofstream::app;
      }

    o = new std::ofstream(f->path().c_str(), mode);
    if (o->fail())
      throw std::runtime_error(io_error(*f));
  }
public:
  std::ofstream *o;
  cc::file *f;
};

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
  write_data *w = reinterpret_cast<write_data*>(userdata);
  const size_t rsize = size*nmemb;

  w->o->write(static_cast<char*>(ptr), rsize);
  if (w->o->fail())
    return w->f->set_errmsg(io_error(*w->f));

  w->o->flush();
  if (w->o->fail())
    return w->f->set_errmsg(io_error(*w->f));

  return rsize;
}

#ifdef WITH_SIGNAL
static volatile sig_atomic_t recv_usr1;

static void handle_usr1(int s)
{
  if (s == SIGUSR1)
    recv_usr1 = 1;
}
#endif

static int progress_cb(void *ptr, double, double now, double, double)
{
#ifdef WITH_SIGNAL
  if (recv_usr1)
    {
      recv_usr1 = 0;
      return 1; // Return a non-zero value to abort this transfer.
    }
#endif
  return reinterpret_cast<progressbar*>(ptr)->update(now);
}

static void _set(write_data *w, const quvi::media& m, CURL *c,
                 progressbar *pb, const double initial_length)
{
  curl_easy_setopt(c, CURLOPT_URL, m.stream_url().c_str());

  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(c, CURLOPT_WRITEDATA, w);

  curl_easy_setopt(c, CURLOPT_PROGRESSFUNCTION, progress_cb);
  curl_easy_setopt(c, CURLOPT_PROGRESSDATA, pb);
  curl_easy_setopt(c, CURLOPT_NOPROGRESS, 0L);

  curl_easy_setopt(c, CURLOPT_ENCODING, "identity");
  curl_easy_setopt(c, CURLOPT_HEADER, 0L);

  if (cc::opts.flags.timestamp)
    curl_easy_setopt(c, CURLOPT_FILETIME, 1L);

  const po::variables_map map = cc::opts.map();

  curl_easy_setopt(c, CURLOPT_MAX_RECV_SPEED_LARGE,
                   static_cast<curl_off_t>(map["throttle"].as<int>()*1024));

  curl_easy_setopt(c, CURLOPT_RESUME_FROM_LARGE,
                   static_cast<curl_off_t>(initial_length));
}

static void _restore(CURL *c)
{
  curl_easy_setopt(c, CURLOPT_RESUME_FROM_LARGE, 0L);
  curl_easy_setopt(c, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(c, CURLOPT_HEADER, 1L);

  curl_easy_setopt(c, CURLOPT_MAX_RECV_SPEED_LARGE,
                   static_cast<curl_off_t>(0L));
}

static bool _handle_error(const long resp_code, const CURLcode rc,
                          std::string& errmsg)
{
  cc::log << std::endl;

  // If an unrecoverable error then do not attempt to retry.
  if (resp_code >= 400 && resp_code <= 500)
    throw std::runtime_error(errmsg);

  // Otherwise.
  bool r = false; // Attempt to retry by default.
#ifdef WITH_SIGNAL
  if (rc == 42) // 42=Operation aborted by callback (libcurl).
    {
      errmsg = "sigusr1 received: interrupt current download";
      r = true; // Skip - do not attempt to retry.
    }
#endif
  cc::log << "error: " << errmsg << std::endl;
  return r;
}

namespace fs = boost::filesystem;

bool file::write(const quvi::media& m, CURL *curl) const
{
  write_data w(const_cast<cc::file*>(this));
  w.open_file();

  progressbar pb(*this, m);
  _set(&w, m, curl, &pb, _initial_length);

#ifdef WITH_SIGNAL
  recv_usr1 = 0;
  if (signal(SIGUSR1, handle_usr1) == SIG_ERR)
    {
      cc::log << "warning: ";
      if (errno)
        cc::log << cc::perror();
      else
        cc::log << "unable to catch SIGUSR1";
      cc::log << std::endl;
    }
#endif

  const CURLcode rc = curl_easy_perform(curl);
  _restore(curl);

  // Restore curl settings.

  curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
  curl_easy_setopt(curl, CURLOPT_FILETIME, 0L);
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(curl, CURLOPT_RESUME_FROM_LARGE, 0L);
  curl_easy_setopt(curl,
                   CURLOPT_MAX_RECV_SPEED_LARGE,
                   static_cast<curl_off_t>(0L));

  long resp_code = 0;
  long conn_code = 0;

  curl_easy_getinfo(curl, CURLINFO_HTTP_CONNECTCODE, &conn_code);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp_code);

  std::string error;

  if (CURLE_OK == rc)
    {
      if (resp_code != 200 && resp_code != 206)
        error = format_unexpected_http_error(resp_code, conn_code);
    }
  else
    {
      if (CURLE_WRITE_ERROR == rc) // write_cb returned != rsize
        error = _errmsg;
      else
        error = format_error(rc, resp_code, conn_code);
    }

  if (!error.empty())
    return _handle_error(resp_code, rc, error);

  pb.finish();
  cc::log << std::endl;

  if (cc::opts.flags.timestamp)
    {
      long ft = -1;
      curl_easy_getinfo(curl, CURLINFO_FILETIME, &ft);
      if (ft >=0)
        fs::last_write_time(_path, ft);
    }

  return true;
}

static double to_mb(const double bytes)
{
  return bytes/(1024*1024);
}

std::string file::to_s(const quvi::media& m) const
{
  const double length = to_mb(m.content_length());

  boost::format fmt = boost::format("%s  %.2fM  [%s]")
                      % _name % length % m.content_type();

  return fmt.str();
}

static fs::path output_dir(const po::variables_map& map)
{
  fs::path dir;
  if (map.count("output-dir"))
    dir = map["output-dir"].as<std::string>();
  return fs::system_complete(dir);
}

typedef std::vector<std::string> vst;

void file::_init(const quvi::media& media)
{
  _title = media.title();

  const po::variables_map map = cc::opts.map();

  if (map.count("output-file"))
    {
      // Overrides --filename-format.

      fs::path p = output_dir(map);

      p /= map["output-file"].as<std::string>();

#if BOOST_FILESYSTEM_VERSION > 2
      _name = p.filename().string();
#else
      _name = p.filename();
#endif
      _path           = p.string();
      _initial_length = file::exists(_path);

      if ( _initial_length >= media.content_length() && ! opts.flags.overwrite)
        throw cc::nothing_todo_error();
    }

  else
    {
      // Cleanup media title.

      std::string title = media.title();
      vst tr;

      if (map.count("tr"))
        tr = map["tr"].as<vst>();
      else // Use built-in default value.
        {
          if (map.count("regexp")) // Deprecated.
            cc::re::capture(map["regexp"].as<std::string>(), title);
          else
            tr.push_back("/(\\w|\\pL|\\s)/g");
        }

      foreach (const std::string r, tr)
      {
        cc::re::tr(r, title);
      }
      cc::re::trim(title);

      // --filename-format

      std::string fname_format = map["filename-format"].as<std::string>();

      pcrecpp::RE("%i").GlobalReplace(media.id(), &fname_format);
      pcrecpp::RE("%t").GlobalReplace(title, &fname_format);
      pcrecpp::RE("%h").GlobalReplace("nohostseq", &fname_format);
      pcrecpp::RE("%s").GlobalReplace(media.file_ext(), &fname_format);

      if (map.count("subst")) // Deprecated.
        {
          std::istringstream iss(map["subst"].as<std::string>());
          vst v;

          std::copy(
            std::istream_iterator<std::string >(iss),
            std::istream_iterator<std::string >(),
            std::back_inserter<vst>(v)
          );

          foreach (const std::string s, v)
          {
            cc::re::subst(s, fname_format);
          }
        }

      std::stringstream b;

      b << fname_format;

      // Output dir.

      const fs::path out_dir = output_dir(map);
      fs::path templ_path    = out_dir;

      templ_path /= b.str();

      // Path, name.

      fs::path p = fs::system_complete(templ_path);

#if BOOST_FILESYSTEM_VERSION > 2
      _name = p.filename().string();
#else
      _name = p.filename();
#endif
      _path = p.string();

      if (! opts.flags.overwrite)
        {
          for (int i=0; i<INT_MAX; ++i)
            {
              _initial_length = file::exists(_path);

              if (_initial_length == 0)
                break;

              else if (_initial_length >= media.content_length())
                throw cc::nothing_todo_error();

              else
                {
                  if (opts.flags.cont)
                    break;
                }

              boost::format fmt =
                boost::format("%1%.%2%") % templ_path.string() % i;

              p = fs::system_complete(fmt.str());

#if BOOST_FILESYSTEM_VERSION > 2
              _name = p.filename().string();
#else
              _name = p.filename();
#endif
              _path = p.string();
            }
        }
    }

  if (opts.flags.overwrite)
    _initial_length = 0;
}

double file::exists(const std::string& path)
{
  fs::path p( fs::system_complete(path) );

  double size = 0;

  if (fs::exists(p))
    size = static_cast<double>(fs::file_size(p));

  return size;
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
