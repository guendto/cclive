/* cclive
 * Copyright (C) 2010,2011,2013  Toni Gundogdu <legatvs@gmail.com>
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

#include <boost/format.hpp>
#include <pcrecpp.h>

#include <ccerror>
#include <ccre>

namespace cc
{

namespace re
{

static void chk_err(const pcrecpp::RE& re)
{
  if (re.error().length() >0)
    {
      BOOST_THROW_EXCEPTION(cc::error::tuple()
        << cc::error::errinfo_tuple(
            boost::make_tuple(re.pattern(), re.error())));
    }
}

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
      chk_err(subs);

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
          chk_err(re);
          src.clear();

          std::string s;
          while (re.FindAndConsume(&sp, &s))
            src += s;
        }
      else
        {
          std::string tmp = src;
          src.clear();
          pcrecpp::RE re(pat, opts);
          chk_err(re);
          re.PartialMatch(tmp, &src);
        }
      return true;
    }
  return false;
}

bool grep(const std::string& r, const std::string& s)
{
  pcrecpp::RE re(r, pcrecpp::UTF8());
  chk_err(re);
  return re.PartialMatch(s);
}

static void tr_subst(const std::string& r, std::string& s)
{
  static const char pattern[] = "^s\\/(.*)\\/(.*)\\/(.*)$";

  pcrecpp::RE rx(pattern, pcrecpp::UTF8());
  std::string pat, sub, flags;

  if (! rx.PartialMatch(r, &pat, &sub, &flags))
    {
      const std::string& m =
        (boost::format("nothing matched `%1%'") % pattern).str();

      BOOST_THROW_EXCEPTION(cc::error::tuple()
        << cc::error::errinfo_tuple(boost::make_tuple(r, m)));
    }

  if (s.empty()) // Validate regexp only.
    return;

  pcrecpp::RE_Options o = _init_re_opts(flags);
  pcrecpp::RE subs(pat, o);
  chk_err(subs);

  (strstr(flags.c_str(), "g"))
  ? subs.GlobalReplace(sub, &s)
  : subs.Replace(sub, &s);
}

static void tr_filter(const std::string& r, std::string& s)
{
  static const char pattern[] = "^\\/(.*)\\/(.*)$";

  pcrecpp::RE rx(pattern, pcrecpp::UTF8());
  std::string pat, flags;

  if (! rx.PartialMatch(r, &pat, &flags))
    {
      const std::string& m =
        (boost::format("nothing matched `%1%'") % pattern).str();

      BOOST_THROW_EXCEPTION(cc::error::tuple()
        << cc::error::errinfo_tuple(boost::make_tuple(r, m)));
    }

  if (s.empty())  // Validate regexp only.
    return;

  pcrecpp::RE_Options o = _init_re_opts(flags);

  if (strstr(flags.c_str(), "g") != 0)
    {
      std::string orig(s);
      pcrecpp::StringPiece sp(orig);
      s.clear();

      pcrecpp::RE re(pat, o);
      chk_err(re);
      std::string tmp;

      while (re.FindAndConsume(&sp, &tmp))
        s += tmp;
    }
  else
    {
      std::string tmp = s;
      s.clear();
      pcrecpp::RE re(pat, o);
      chk_err(re);
      re.PartialMatch(tmp, &s);
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
