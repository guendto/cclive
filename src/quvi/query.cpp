/* cclive
 * Copyright (C) 2010,2011  Toni Gundogdu <legatvs@gmail.com>
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

#include "config.h"

#include <cassert>

#include <quvi/ccquvi.h>

namespace quvi
{

// Constructor.

query::query()
  : _quvi(NULL), _curl(NULL)
{
  _init();
}

// Copy constructor.

query::query(const query& q)
  : _quvi(NULL), _curl(NULL)
{
  _init();
}

// Copy assignment operator.

query& query::operator=(const query& q)
{
  if (this != &q)
    {
      _close();
      _init();
    }
  return *this;
}

// Destructor.

query::~query()
{
  _close();
}

// Init.

void query::_init()
{

  const QUVIcode rc = quvi_init(&_quvi);

  if (rc != QUVI_OK)
    throw error(_quvi,rc);

  assert(_quvi != NULL);

  quvi_getinfo(_quvi, QUVIINFO_CURL, &_curl);
  assert(_curl != NULL);
}

// Close.

void query::_close()
{
  if (_quvi)
    quvi_close(&_quvi); // Resets to NULL.
  assert(_quvi == NULL);
  _curl = NULL;
}

// Set common libquvi options.

void query::_set_opts(const options& opts) const
{
  // Friend of quvi::options class -> clean API.

  if (!opts._format.empty())
    quvi_setopt(_quvi, QUVIOPT_FORMAT, opts._format.c_str());

  quvi_setopt(_quvi, QUVIOPT_STATUSFUNCTION, opts._statusfunc);
#ifdef _0
  quvi_setopt(_quvi, QUVIOPT_NOVERIFY, opts._verify ? 1L:0L);
#endif

  quvi_setopt(_quvi, QUVIOPT_NORESOLVE, opts._resolve ? 1L:0L);
  quvi_setopt(_quvi, QUVIOPT_CATEGORY, QUVIPROTO_HTTP);
}

// Query formats.

std::string query::formats(const std::string& url,
                           const options& opts) const
{
  _set_opts(opts);

  char *formats = NULL;

  QUVIcode rc   = quvi_query_formats(_quvi,
                                     const_cast<char*>(url.c_str()),
                                     &formats);
  if (rc != QUVI_OK)
    throw error(_quvi,rc);

  std::string r = formats;
  quvi_free(formats);

  return r;
}


// Parse.

media query::parse(const std::string& pageURL, const options& opts) const
{
  _set_opts(opts);

  quvi_media_t qm;

  QUVIcode rc =
    quvi_parse(_quvi, const_cast<char*>(pageURL.c_str()), &qm);

  if (rc != QUVI_OK)
    throw error(_quvi,rc);

  assert(qm != NULL);

  media m(qm);
  quvi_parse_close(&qm);

  return m;
}

// Get.

void* query::curlHandle() const
{
  return _curl;
}

// Support.

std::map<std::string,std::string> query::support() const
{
  std::map<std::string,std::string> map;
  char *d=NULL, *f=NULL;

  while (quvi_next_supported_website(_quvi, &d, &f) == QUVI_OK)
    {
      map[d] = f;
      quvi_free(d);
      d = NULL;
      quvi_free(f);
      f = NULL;
    }

  return map;
}

} // namespace quvi

// vim: set ts=2 sw=2 tw=72 expandtab:
