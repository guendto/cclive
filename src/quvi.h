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

#ifndef quvimgr_h
#define quvimgr_h

#include <curl/curl.h>
#include <quvi/quvi.h>

#include "singleton.h"

class QuviVideo;

class QuviMgr : public Singleton<QuviMgr> {
public:
    QuviMgr          ();
    QuviMgr          (const QuviMgr&);
    QuviMgr& operator=(const QuviMgr&);
    virtual ~QuviMgr ();
public:
    void    init        ();
    quvi_t  handle      () const;
    void    curlHandle  (CURL **curl);
private:
    quvi_t quvi;
};

#define quvimgr QuviMgr::getInstance()

class QuviVideo {
public:
    QuviVideo           ();
    QuviVideo           (const std::string& url);
    QuviVideo           (const QuviVideo&);
    QuviVideo& operator=(const QuviVideo&);
    virtual ~QuviVideo();
public:
    void parse(std::string url="");
public:
    const double&      getLength()      const;
    const std::string& getPageLink()    const;
    const std::string& getId()          const;
    const std::string& getTitle()       const;
    const std::string& getLink()        const;
    const std::string& getSuffix()      const;
    const std::string& getContentType() const;
    const std::string& getHostId()      const;
    const double&      getInitial()     const;
public:
    void               formatOutputFilename();
    void               customOutputFilenameFormatter(
                           std::stringstream& b);
    void               updateInitial();
    const std::string& getFilename()    const;
private:
    double length;
    std::string pageLink;
    std::string id;
    std::string title;
    std::string link;
    std::string suffix;
    std::string contentType;
    std::string hostId;
    double initial;
    std::string filename;
};

#endif


