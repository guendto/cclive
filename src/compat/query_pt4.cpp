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

#include <curl/curl.h>

#include <ccquvi>
#include <ccutil>

namespace quvi
{

void query_pt4::_init()
{
  const QUVIcode qc = quvi_init(&_quvi);
  if (qc != QUVI_OK)
    throw error(_quvi, qc);
}

void query_pt4::_close()
{
  if (_quvi != NULL)
    {
      quvi_close(&_quvi);
      _quvi = NULL;
    }
}

void query_pt4::_configure(const quvi::options& opts) const
{
  if (! opts.stream.empty())
    quvi_setopt(_quvi, QUVIOPT_FORMAT, opts.stream.c_str());

  quvi_setopt(_quvi, QUVIOPT_STATUSFUNCTION, opts.statusfunc);
  quvi_setopt(_quvi, QUVIOPT_NORESOLVE, opts.resolve ? 0L:1L);
  quvi_setopt(_quvi, QUVIOPT_CATEGORY, QUVIPROTO_HTTP);
}

void query_pt4::setup_curl() const
{
  CURL *c;
  quvi_getinfo(_quvi, QUVIINFO_CURL, &c);
  cc::curl_setup(c);
}

std::string query_pt4::streams(const std::string& url,
                               const quvi::options& opts) const
{
  _configure(opts);

  char *fmts;

  const QUVIcode qc =
    quvi_query_formats(_quvi, const_cast<char*>(url.c_str()), &fmts);

  if (qc != QUVI_OK)
    throw error(_quvi, qc);

  std::string r = fmts;
  quvi_free(fmts);

  return r;
}

media query_pt4::parse(const std::string& url, const options& opts) const
{
  _configure(opts);

  quvi_media_t qm;
  QUVIcode qc = quvi_parse(_quvi, const_cast<char*>(url.c_str()), &qm);

  if (qc != QUVI_OK)
    throw error(_quvi, qc);

  media r(_quvi, qm);
  quvi_parse_close(&qm);

  return r;
}

std::map<std::string,std::string> query_pt4::support() const
{
  quvi_setopt(_quvi, QUVIOPT_CATEGORY, QUVIPROTO_HTTP);

  std::map<std::string,std::string> r;
  char *a, *b;

  while (quvi_next_supported_website(_quvi, &a, &b) == QUVI_OK)
    {
      r[a] = b;
      quvi_free(a);
      quvi_free(b);
    }
  return r;
}

} // namespace quvi

// vim: set ts=2 sw=2 tw=72 expandtab:
