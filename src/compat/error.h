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

#ifndef compat_error_h
#define compat_error_h

namespace quvi
{

class error_base
{
  virtual void _init(quvi_t) = 0;
public:
  virtual bool cannot_retry() const = 0;
  virtual std::string to_s() const = 0;
protected:
  inline virtual ~error_base() { }
};

class error_impl : error_base
{
protected:
  inline error_impl(const error_impl& a): _resp_code(0), _quvi_code(0)
  {
    _copy(a);
  }
  inline error_impl(): _resp_code(0), _quvi_code(0) { }
  inline virtual ~error_impl()                      { }
  inline error_impl& operator=(const error_impl& a)
  {
    if (this != &a)
      _copy(a);
    return *this;
  }
  inline void _copy(const error_impl& a)
  {
    _resp_code = a._resp_code;
    _quvi_code = a._quvi_code;
    _what = a._what;
  }
public:
  inline const std::string& what() const
  {
    return _what;
  }
  inline long response_code() const
  {
    return _resp_code;
  }
  inline long quvi_code() const
  {
    return _quvi_code;
  }
protected:
  std::string _what;
  long _resp_code;
  long _quvi_code;
};

class error_pt4 : public error_impl
{
  void _init(quvi_t);
public:
  inline error_pt4(): error_impl()                    { }
  inline error_pt4(const error_pt4& a): error_impl(a) { }
  inline ~error_pt4()                                 { }
  inline error_pt4(quvi_t q, const long qc): error_impl()
  {
    _quvi_code = qc;
    _init(q);
  }
  inline error_pt4& operator=(const error_pt4& a)
  {
    if (this != &a)
      _copy(a);
    return *this;
  }
  inline bool cannot_retry() const
  {
    return (_resp_code >= 400 || _quvi_code != QUVI_OK);
  }
  std::string to_s() const;
};

class error_pt9 : public error_impl
{
  void _init(quvi_t);
public:
  inline error_pt9(): error_impl()                    { }
  inline error_pt9(const error_pt9& a): error_impl(a) { }
  inline ~error_pt9()                                 { }
  inline error_pt9(quvi_t q, const long qc=-1): error_impl()
  {
    _init(q); // Sets _quvi_code
  }
  inline error_pt9& operator=(const error_pt9& a)
  {
    if (this != &a)
      _copy(a);
    return *this;
  }
  inline bool cannot_retry() const
  {
    return (_resp_code >= 400 || _quvi_code != QUVI_OK);
  }
  std::string to_s() const;
};

#ifdef HAVE_LIBQUVI_0_9
typedef class error_pt9 error;
#else
typedef class error_pt4 error;
#endif

} // namespace quvi

#endif // compat_error_h

// vim: set ts=2 sw=2 tw=72 expandtab:
