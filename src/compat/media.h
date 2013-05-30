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

#ifndef compat_media_h
#define compat_media_h

namespace quvi {

class media_base
{
  virtual void _init(quvi_t, quvi_media_t) = 0;
protected:
  inline virtual ~media_base() { }
};

class media_impl
{
protected:
  std::string _content_type;
  std::string _stream_url;
  std::string _file_ext;
  std::string _title;
  std::string _id;
  double _content_length;
protected:
  inline media_impl(const media_impl& a): _content_length(0)  { _copy(a); }
  inline media_impl(): _content_length(0)                     { }
  inline virtual ~media_impl()                                { }
  inline media_impl& operator=(const media_impl& a)
  {
    if (this != &a)
      _copy(a);
    return *this;
  }
  inline void _copy(const media_impl& a)
  {
    _content_type = a._content_type;
    _stream_url = a._stream_url;
    _file_ext = a._file_ext;
    _title = a._title;
    _id = a._id;

    _content_length = a._content_length;
  }
public:
  inline const std::string& content_type() const  { return _content_type; }
  inline const std::string& stream_url() const    { return _stream_url; }
  inline const std::string& file_ext() const      { return _file_ext; }
  inline const std::string& title() const         { return _title; }
  inline const std::string& id() const            { return _id; }
  inline double content_length() const            { return _content_length; }
};

class media_pt4 : public media_impl
{
  void _init(quvi_t, quvi_media_t);
public:
  inline media_pt4(const media_pt4& a): media_impl(a)       { }
  inline media_pt4(quvi_t q, quvi_media_t qm): media_impl() { _init(q, qm); }
  inline media_pt4(): media_impl()                          { }
  inline virtual ~media_pt4() { }
  inline media_pt4& operator=(const media_pt4& a)
  {
    if (this != &a)
      _copy(a);
    return *this;
  }
};

class media_pt9 : public media_impl
{
  void _init(quvi_t, quvi_media_t);
public:
  inline media_pt9(const media_pt9& a): media_impl(a)       { }
  inline media_pt9(quvi_t q, quvi_media_t qm): media_impl() { _init(q, qm); }
  inline media_pt9(): media_impl()                          { }
  inline virtual ~media_pt9() { }
  inline media_pt9& operator=(const media_pt9& a)
  {
    if (this != &a)
      _copy(a);
    return *this;
  }
};

#ifdef HAVE_LIBQUVI_0_9
typedef class media_pt9 media;
#else
typedef class media_pt4 media;
#endif

} // namespace quvi

#endif // compat_media_h

// vim: set ts=2 sw=2 tw=72 expandtab:
