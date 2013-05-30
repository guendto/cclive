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

#include <cstring>
#include <ccquvi>

namespace quvi
{

static void _get_s(quvi_media_t qm, QUVIproperty qp, std::string& dst)
{
  char *s;

  dst.clear();
  quvi_getprop(qm, qp, &s);

  if (strlen(s) >0)
    dst = s;
}

void media_pt4::_init(quvi_t, quvi_media_t qm)
{
  quvi_getprop(qm, QUVIPROP_MEDIACONTENTLENGTH, &_content_length);
  _get_s(qm, QUVIPROP_MEDIACONTENTTYPE, _content_type);
  _get_s(qm, QUVIPROP_FILESUFFIX, _file_ext);
  _get_s(qm, QUVIPROP_MEDIAURL, _stream_url);
  _get_s(qm, QUVIPROP_PAGETITLE, _title);
  _get_s(qm, QUVIPROP_MEDIAID, _id);
}

} // namespace quvi

// vim: set ts=2 sw=2 tw=72 expandtab:
