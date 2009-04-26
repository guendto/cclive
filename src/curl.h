/*
 * cclive Copyright (C) 2009 Toni Gundogdu. This file is part of cclive.
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

#ifndef curl_h
#define curl_h

class CurlMgr : public Singleton<CurlMgr> {
public:
    CurlMgr();
    virtual ~CurlMgr();
    CurlMgr(const CurlMgr&);
    CurlMgr& operator=(const CurlMgr&);
public:
    void         init           ();
    std::string  fetchToMem     (const std::string& url,
                                    const std::string& what="");
    void         queryFileLength(VideoProperties&);
    void         fetchToFile    (VideoProperties&);
    void         logIntoYoutube ();
    const std::string& unescape (std::string& url) const;
public:
    class FetchException : public RuntimeException {
    public:
        FetchException(const std::string&);
    };
};

#define curlmgr CurlMgr::getInstance()

#endif