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

#include <ccinternal>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <glib.h>

#ifndef foreach
#define foreach BOOST_FOREACH
#endif

#include <ccoptions>
#include <ccinput>

namespace cc
{

namespace po = boost::program_options;

typedef std::vector<std::string> vst;
typedef po::variables_map pvm;

static bool _has_uri_scheme(const std::string& s)
{
  gchar *p = g_uri_parse_scheme(s.c_str());
  const bool r = (p != NULL);
  g_free(p);
  return r;
}

static bool _push(vst& dst, const std::string& s)
{
  gchar *u = g_uri_unescape_string(s.c_str(), NULL);
  bool r = _has_uri_scheme(u);
  if (r)
    dst.push_back(u);
  g_free(u);
  return r;
}

static void _extract_uris(vst& dst, const std::string& s)
{
  gchar **r = g_uri_list_extract_uris(s.c_str());
  gint i = 0;
  while (r[i] != NULL)
    _push(dst, r[i++]);
  g_strfreev(r);
}

static std::string _read_file(const std::string& fpath)
{
  GError *e = NULL;
  gchar *c = NULL;
  std::string r;

  if (g_file_get_contents(fpath.c_str(), &c, NULL, &e) ==FALSE)
    {
      if (e != NULL)
        {
          static const char E[] = "error: %s: while reading file: %s";
          std::clog << (boost::format(E) % fpath % e->message).str()
                    << std::endl;
          g_error_free(e);
        }
    }
  else
    r = c;
  g_free(c);
  return r;
}

static void _unable_determine(const std::string& s)
{
  static const char E[] =
    "error: ignoring input value `%s': not an URL or a file";
  std::clog << (boost::format(E) % s).str() << std::endl;
}

static void _have_rargs(vst& dst, const pvm& m)
{
  const vst &args = m["url"].as<vst>();
  foreach (const std::string &a, args)
  {
    if (g_file_test(a.c_str(), G_FILE_TEST_IS_REGULAR) ==TRUE)
      _extract_uris(dst, _read_file(a));
    else
      {
        if (!_push(dst, a))
          _unable_determine(a);
      }
  }
}

static std::string _read_from(std::istream& is)
{
  std::string s;
  char c;
  while (is.get(c))
    s += c;
  return s;
}

static void _no_rargs(vst& dst, const pvm& m)
{
  _extract_uris(dst, _read_from(std::cin));
}

template<class T> static T _duplicates(T first, T last)
{
  while (first != last)
    {
      T next(first);
      last = std::remove(++next, last, *first);
      first = next;
    }
  return last;
}

void input::_parse()
{
  const po::variables_map m = cc::opts.map();

  if (m.count("url") ==0)
    _no_rargs(_urls, m);
  else
    _have_rargs(_urls, m);

  _urls.erase(_duplicates(_urls.begin(), _urls.end()), _urls.end());
}

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
