/*
* Copyright (C) 2010  Toni Gundogdu <legatvs@gmail.com>
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

#include <pcrecpp.h>

#include <boost/format.hpp>

#include "cclive/re.h"

namespace cclive
{
namespace re
{

static pcrecpp::RE_Options
_init_re_opts (const std::string& flags)
{

  pcrecpp::RE_Options opts;

  opts.set_caseless (strstr (flags.c_str (), "i") != 0);
  opts.set_utf8 (true);

  return opts;
}

bool
subst (const std::string& re, std::string& src)
{

  std::string pat, sub, flags;

  static const char delims_b[] = "\\{\\<\\[\\(\\/";
  static const char delims_c[] = "\\}\\>\\]\\)\\/";

  boost::format fmt =
    boost::format ("^s[%1%](.*)[%2%][%3%](.*)[%4%](.*)$")
    % delims_b % delims_c % delims_b % delims_c;

  pcrecpp::RE rx (fmt.str (), pcrecpp::UTF8 ());

  if ( rx.PartialMatch (re, &pat, &sub, &flags) )
    {

      if (src.empty()) // Verify regexp only.
        return true;

      pcrecpp::RE_Options opts = _init_re_opts (flags);

      pcrecpp::RE subs (pat, opts);

      (strstr (flags.c_str (), "g"))
      ? subs.GlobalReplace (sub, &src)
      : subs.Replace (sub, &src);

      return true;
    }
  return false;
}

bool
match (const std::string& re, std::string& src)
{

  std::string pat, flags;

  pcrecpp::RE rx ("^\\/(.*)\\/(.*)$", pcrecpp::UTF8 ());

  if (rx.PartialMatch (re, &pat, &flags))
    {

      if (src.empty ()) // Verify regexp only.
        return true;

      pcrecpp::RE_Options opts = _init_re_opts (flags);

      if (strstr (flags.c_str (), "g") != 0)
        {

          pcrecpp::StringPiece sp (src);
          pcrecpp::RE re (pat, opts);

          src.clear ();

          std::string s;

          while (re.FindAndConsume (&sp, &s))
            src += s;
        }

      else
        {
          std::string tmp = src;
          src.clear ();
          pcrecpp::RE (pat, opts).PartialMatch (tmp, &src);
        }

      return true;
    }
  return false;
}

void
trim (std::string& src)
{
  subst ("s{^[\\s]+}//",   src);
  subst ("s{\\s+$}//",     src);
  subst ("s{\\s\\s+}/ /g", src);
}

}
} // End namespace.

// vim: set ts=2 sw=2 tw=72 expandtab:
