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

#include <cstring>
#include <ccquvi>

namespace quvi
{

static void _get_s(quvi_media_t qm, QuviMediaProperty qmp, std::string& dst)
{
  char *s = NULL;

  dst.clear();
  quvi_media_get(qm, qmp, &s);

  if (s != NULL && strlen(s) >0)
    dst = s;
}

static void _qmi_s(quvi_http_metainfo_t qmi,
                   const QuviHTTPMetaInfoProperty qmip,
                   std::string& dst)
{
  char *s = NULL;

  dst.clear();
  quvi_http_metainfo_get(qmi, qmip, &s);

  if (s != NULL && strlen(s) >0)
    dst = s;
}

void media_pt9::_init(quvi_t q, quvi_media_t qm)
{
  _get_s(qm, QUVI_MEDIA_STREAM_PROPERTY_URL, _stream_url);
  _get_s(qm, QUVI_MEDIA_PROPERTY_TITLE, _title);
  _get_s(qm, QUVI_MEDIA_PROPERTY_ID, _id);

  quvi_http_metainfo_t qmi = quvi_http_metainfo_new(q, _stream_url.c_str());
  if (quvi_ok(q) == QUVI_FALSE)
    {
      quvi_http_metainfo_free(qmi);
      throw error(q);
    }

  quvi_http_metainfo_get(qmi, QUVI_HTTP_METAINFO_PROPERTY_LENGTH_BYTES,
                         &_content_length);

  _qmi_s(qmi, QUVI_HTTP_METAINFO_PROPERTY_CONTENT_TYPE, _content_type);
  _qmi_s(qmi, QUVI_HTTP_METAINFO_PROPERTY_FILE_EXTENSION, _file_ext);

  quvi_http_metainfo_free(qmi);
}

} // namespace quvi

// vim: set ts=2 sw=2 tw=72 expandtab:
