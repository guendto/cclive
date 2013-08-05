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

#ifndef cclive_input_h
#define cclive_input_h

namespace cc
{

class input
{
public:
  inline input(const po::variables_map& vm) { _parse(vm); }
  inline virtual ~input() { }
public:
  inline const std::vector<std::string>& urls() { return _urls; }
private:
  void _parse(const po::variables_map& vm);
private:
  std::vector<std::string> _urls;
};

} // namespace cc

#endif // cclive_input_h

// vim: set ts=2 sw=2 tw=72 expandtab:
