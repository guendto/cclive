/* cclive
 * Copyright (C) 2010-2011  Toni Gundogdu <legatvs@gmail.com>
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

#include <sstream>

#include <boost/format.hpp>
#include <pcrecpp.h>

#include <ccre>

namespace cc
{
namespace re
{

static pcrecpp::RE_Options _init_re_opts(const std::string& flags)
{
  pcrecpp::RE_Options opts;
  opts.set_caseless(strstr(flags.c_str(), "i") != 0);
  opts.set_utf8(true);
  return opts;
}

bool subst(const std::string& re, std::string& src)
{
  std::string pat, sub, flags;

  static const char delims_b[] = "\\{\\<\\[\\(\\/";
  static const char delims_c[] = "\\}\\>\\]\\)\\/";

  boost::format fmt =
    boost::format("^s[%1%](.*)[%2%][%3%](.*)[%4%](.*)$")
    % delims_b % delims_c % delims_b % delims_c;

  pcrecpp::RE rx(fmt.str(), pcrecpp::UTF8());

  if (rx.PartialMatch(re, &pat, &sub, &flags))
    {
      if (src.empty()) // Verify regexp only.
        return true;

      pcrecpp::RE_Options opts = _init_re_opts(flags);
      pcrecpp::RE subs(pat, opts);

      (strstr(flags.c_str(), "g"))
      ? subs.GlobalReplace(sub, &src)
      : subs.Replace(sub, &src);

      return true;
    }
  return false;
}

bool capture(const std::string& re, std::string& src)
{
  std::string pat, flags;
  pcrecpp::RE rx("^\\/(.*)\\/(.*)$", pcrecpp::UTF8());

  if (rx.PartialMatch(re, &pat, &flags))
    {
      if (src.empty()) // Check regexp
        return true;

      pcrecpp::RE_Options opts = _init_re_opts(flags);
      if (strstr(flags.c_str(), "g") != 0)
        {
          std::string orig(src);
          pcrecpp::StringPiece sp(orig);
          pcrecpp::RE re(pat, opts);
          src.clear();

          std::string s;
          while (re.FindAndConsume(&sp, &s))
            src += s;
        }
      else
        {
          std::string tmp = src;
          src.clear();
          pcrecpp::RE(pat, opts).PartialMatch(tmp, &src);
        }
      return true;
    }
  return false;
}

bool grep(const std::string& r, const std::string& s)
{
  return pcrecpp::RE(r, pcrecpp::UTF8()).PartialMatch(s);
}

static void tr_subst(const std::string& r, std::string& s)
{
  pcrecpp::RE rx("^s\\/(.*)\\/(.*)\\/(.*)$", pcrecpp::UTF8());
  std::string pat, sub, flags;

  if (!rx.PartialMatch(r, &pat, &sub, &flags))
    {
      std::stringstream b;
      b << "--tr: " << "no idea what to do with `" << r << "'";
      throw std::runtime_error(b.str());
    }

  if (s.empty()) // Validate regexp only.
    return;

  pcrecpp::RE_Options o = _init_re_opts(flags);
  pcrecpp::RE subs(pat, o);

  (strstr(flags.c_str(), "g"))
  ? subs.GlobalReplace(sub, &s)
  : subs.Replace(sub, &s);
}

static void tr_filter(const std::string& r, std::string& s)
{
  pcrecpp::RE rx("^\\/(.*)\\/(.*)$", pcrecpp::UTF8());
  std::string pat, flags;

  if (!rx.PartialMatch(r, &pat, &flags))
    {
      std::stringstream b;
      b << "--tr: " << "no idea what to do with `" << r << "'";
      throw std::runtime_error(b.str());
    }

  if (s.empty())  // Validate regexp only.
    return;

  pcrecpp::RE_Options o = _init_re_opts(flags);

  if (strstr(flags.c_str(), "g") != 0)
    {
      std::string orig(s);
      pcrecpp::StringPiece sp(orig);
      s.clear();

      rx = pcrecpp::RE(pat, o);
      std::string tmp;

      while (rx.FindAndConsume(&sp, &tmp))
        s += tmp;
    }
  else
    {
      std::string tmp = s;
      s.clear();
      pcrecpp::RE(pat, o).PartialMatch(tmp, &s);
    }
}

void tr(const std::string& r, std::string& s)
{
  if (pcrecpp::RE("^s\\/", pcrecpp::UTF8()).PartialMatch(r))
    tr_subst(r, s);
  else
    tr_filter(r, s);
}

void trim(std::string& s)
{
  tr_subst("s/^[\\s]+//",   s);
  tr_subst("s/\\s+$//",     s);
  tr_subst("s/\\s\\s+/ /g", s);
}

} // namespace re

} // namespace cc

// vim: set ts=2 sw=2 tw=72 expandtab:
