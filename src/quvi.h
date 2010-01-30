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

#include <vector>
#include <iterator>
#include <tr1/memory>

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

class QuviVideoLink {
public:
    std::string url;
    std::string ct;
    std::string suffix;
    double length;
    double initial;
    std::string filename;
    bool nothing_todo;
};

namespace quvi {

typedef std::tr1::shared_ptr<QuviVideoLink> SHPQuviVideoLink;

typedef std::vector<SHPQuviVideoLink> QuviVideoLinkVector;
typedef QuviVideoLinkVector::iterator QuviVideoLinkIter;

typedef std::vector<std::string> StringVector;
typedef StringVector::const_iterator StringVectorIter;

}

class QuviVideo {
public:
    QuviVideo           ();
    QuviVideo           (const std::string& url);
public:
    void parse(std::string url="");
public:
    const std::string& getPageUrl() const;
    const std::string& getPageTitle() const;
    const std::string& getId() const;
    const std::string& getFileUrl() const;
    const std::string& getFileContentType() const;
    const std::string& getFileSuffix() const;
    const double& getFileLength() const;
    const double& getInitialFileLength() const;
    const std::string& getFileName() const;
    const std::string& getHostId() const;
    const bool& getNothingTodo() const;
public:
    void resetVideoLink();
    void nextVideoLink();
    void updateInitialLength();
private:
    static void toFileName(
        const std::string& pageTitle,
        const std::string& videoId,
        const std::string& hostId,
        quvi::SHPQuviVideoLink qvl,
        const int& linkIndex,
        const int& totalLinks);
private:
    quvi::QuviVideoLinkVector videoLinks;
    quvi::QuviVideoLinkIter currentVideoLink;
    std::string pageUrl;
    std::string pageTitle;
    std::string videoId;
    std::string hostId;
};

#endif


