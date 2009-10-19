/*
 * Copyright (C) 2009 Toni Gundogdu.
 *
 * This file is part of cclive.
 * 
 * cclive is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * cclive is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef util_h
#define util_h

class QuviVideo;

class Util {
public:
    static const double fileExists   (const std::string& path);
    static std::string& subStrReplace(std::string& src,
                                        const std::string& what,
                                        const std::string& with);
    static std::string& embedToPage  (std::string& url);
    static std::string& nocookieToYoutube (std::string& url);
    static std::string& lastfmToYoutube(std::string& url);
    static std::string& toLower      (std::string& src);
    static std::vector<std::string>
                        tokenize     (const std::string& src,
                                        const std::string& delims);
    static std::string parseFormatMap(const std::string& host);
    static const std::string& toUnicode(std::string& src,
                                    const std::string& from);
};

#endif


