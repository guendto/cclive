/* cclive
 * Copyright (C) 2013  Toni Gundogdu <legatvs@gmail.com>
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

#include <boost/format.hpp>
#include <ccquvi>

namespace quvi
{

void error_pt9::_init(quvi_t q)
{
  _what = quvi_errmsg(q);
  quvi_get(q, QUVI_INFO_RESPONSE_CODE, &_resp_code);
  quvi_get(q, QUVI_INFO_ERROR_CODE, &_quvi_code);
}

std::string error_pt9::to_s() const
{
  return (boost::format("what=%s, resp_code=%ld, quvi_code=%ld")
          % _what.c_str()
          % _resp_code
          % _quvi_code).str();
}

} // namespace quvi

// vim: set ts=2 sw=2 tw=72 expandtab:
