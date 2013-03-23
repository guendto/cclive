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

#include <cstring>
#include <sstream>

#include <ccquvi>
#include <ccutil>

namespace quvi
{

void query_pt9::_init()
{
  _quvi = quvi_new();
  if (quvi_ok(_quvi) == QUVI_FALSE)
    throw error(_quvi);
}

void query_pt9::_close()
{
  quvi_free(_quvi);
  _quvi = NULL;
}

void query_pt9::_configure(const quvi::options& qopts) const
{
  quvi_set(_quvi, QUVI_OPTION_USER_AGENT, qopts.useragent.c_str());
  quvi_set(_quvi, QUVI_OPTION_CALLBACK_STATUS, qopts.statusfunc);
}

void query_pt9::setup_curl() const
{
  CURL *c;
  quvi_get(_quvi, QUVI_INFO_CURL_HANDLE, &c);
  cc::curl_setup(c);
}

std::string query_pt9::streams(const std::string& url,
                               const quvi::options& opts) const
{
  _configure(opts);

  quvi_media_t qm = quvi_media_new(_quvi, url.c_str());
  if (quvi_ok(_quvi) == QUVI_FALSE)
    {
      quvi_media_free(qm);
      throw error(_quvi);
    }

  std::stringstream ss;
  char *id;

  while (quvi_media_stream_next(qm) == QUVI_TRUE)
    {
      quvi_media_get(qm, QUVI_MEDIA_STREAM_PROPERTY_ID, &id);
      if (strlen(id) >0)
        {
          if (ss.str().length() >0)
            ss << ",";
          ss << id;
        }
    }
  quvi_media_free(qm);
  return ss.str();
}

media query_pt9::parse(const std::string& url,
                       const quvi::options& opts) const
{
  _configure(opts);

  quvi_media_t qm = quvi_media_new(_quvi, url.c_str());
  if (quvi_ok(_quvi) == QUVI_FALSE)
    throw error(_quvi);

  if (! opts.stream.empty())
    quvi_media_stream_select(qm, opts.stream.c_str());

  media r(_quvi, qm);
  quvi_media_free(qm);

  return r;
}

std::map<std::string,std::string> query_pt9::support() const
{
  std::map<std::string,std::string> r;
  char *v;

  while (quvi_script_next(_quvi, QUVI_SCRIPT_TYPE_MEDIA) == QUVI_TRUE)
    {
      quvi_script_get(_quvi, QUVI_SCRIPT_TYPE_MEDIA,
                      QUVI_SCRIPT_PROPERTY_DOMAINS, &v);
      r[v] = "";
    }
  return r;
}

} // namespace quvi

// vim: set ts=2 sw=2 tw=72 expandtab:
