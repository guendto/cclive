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

#ifndef cc__input_h
#define cc__input_h

#include <ccinternal>

#include <istream>
#include <sstream>

#include <boost/noncopyable.hpp>
#include <glibmm.h>

#include <ccoptions>
#include <ccfstream>
#include <ccerror>

namespace cc
{

namespace po = boost::program_options;

struct input : boost::noncopyable
{
  typedef std::vector<std::string> vs;

  static inline vs parse(const po::variables_map& vm)
  {
    vs r;
    (vm.count("url") ==0)
      ? parse_without_rargs(vm,r)
      : parse_with_rargs(vm,r);
    return remove_duplicates(r);
  }

private:
  template<class T> static T duplicates(T first, T last)
  {
    while (first != last)
      {
        T next(first);
        last = std::remove(++next, last, *first);
        first = next;
      }
    return last;
  }

  static inline const vs& remove_duplicates(vs& r)
  {
    r.erase(duplicates(r.begin(), r.end()), r.end());
    if (r.size() ==0)
      BOOST_THROW_EXCEPTION(cc::error::no_input());
    return r;
  }

  static inline const vs& parse_without_rargs(const po::variables_map& vm,
                                              vs& dst)
  {
    return extract_uris(dst, read_stdin());
  }

  static inline const vs& parse_with_rargs(const po::variables_map& vm,
                                           vs& dst)
  {
    BOOST_FOREACH(const std::string& s, vm["url"].as<vs>())
    {
      const std::string u = Glib::uri_unescape_string(s);
      const std::string c = Glib::uri_parse_scheme(u);
      if (c.length() ==0)
        extract_uris(dst, cc::fstream::read(u));
      else if(c =="http" || c =="https")
        dst.push_back(u);
      else if (c =="file")
        read_from_escaped_uri(dst, u);
      else
        {
          BOOST_THROW_EXCEPTION(cc::error::tuple()
            << cc::error::errinfo_tuple(
                boost::make_tuple(s, "neither a valid URL or a local file")));
        }
    }
    return dst;
  }

  static inline const vs& extract_uris(vs& dst, const std::string& s)
  {
    gchar **r = g_uri_list_extract_uris(s.c_str());
    for (int i=0; r[i] != NULL; ++i)
      {
        const std::string u = Glib::uri_unescape_string(r[i]);
        const std::string c = Glib::uri_parse_scheme(u);
        if (c == "http" || c == "https")
          dst.push_back(u);
        else if (c =="file")
          read_from_escaped_uri(dst, u);
        else
          {
            BOOST_THROW_EXCEPTION(cc::error::tuple()
              << cc::error::errinfo_tuple(
                  boost::make_tuple(u, "an invalid URL")));
          }
      }
    g_strfreev(r);
    return dst;
  }

  static inline const vs& read_from_escaped_uri(vs& dst, const std::string& s)
  {
    try
      {
        const std::string r = Glib::filename_from_uri(s);
        extract_uris(dst, cc::fstream::read(r));
      }
    catch (const Glib::ConvertError& x)
      {
        BOOST_THROW_EXCEPTION(cc::error::tuple()
          << cc::error::errinfo_tuple(boost::make_tuple(s, x.what())));
      }
    return dst;
  }

  static inline std::string read_stdin()
  {
    std::stringstream s;
    char c;
    while (std::cin.get(c))
      s << c;
    return s.str();
  }
};

} // namespace cc

#endif // cc__input_h

// vim: set ts=2 sw=2 tw=72 expandtab:
