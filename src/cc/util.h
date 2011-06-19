/* cclive
 * Copyright (C) 2011  Toni Gundogdu <legatvs@gmail.com>
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

#ifndef cclive_util_h
#define cclive_util_h

namespace quvi
{
class query;
class media;
class url;
} // namespace quvi

namespace cc
{

class options;
class file;

void exec(const file&, const quvi::url&, const options& opts);
void get(const quvi::query&, quvi::media&, const options&);
void go_background(const std::string&, bool&);
std::string perror(const std::string& p="");
void wait(const int);

} // namespace cc

#endif // cclive_util_h

/* vim: set ts=2 sw=2 tw=72 expandtab: */
