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

#ifndef curl_h
#define curl_h

#include "singleton.h"

class QuviVideo;

class CurlMgr : public Singleton<CurlMgr> {
public:
    CurlMgr           ();
    CurlMgr           (const CurlMgr&);
    CurlMgr& operator=(const CurlMgr&);
    virtual ~CurlMgr  ();
public:
    void         init           ();
    std::string  fetchToMem     (const std::string& url,
                                    const std::string& what="");
    void         fetchToFile    (QuviVideo&);
    const std::string& unescape (std::string& url) const;
    const std::string& escape   (std::string& url) const;
private:
    long httpcode;
};

#define curlmgr CurlMgr::getInstance()

#endif


