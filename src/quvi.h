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

#ifndef quvi_h
#define quvi_h

#include "singleton.h"

class QuviVideo;

class QuviMgr : public Singleton<QuviMgr> {
public:
    QuviMgr          ();
    QuviMgr          (const QuviMgr&);
    QuviMgr operator=(const QuviMgr&);
    virtual ~QuviMgr ();
public:
    void    init        ();
    quvi_t  handle      () const;
    void    curlHandle  (CURL **curl);
private:
    void handleError();
private:
    quvi_t quvi;
public:
    class QuviException : public RuntimeException {
    public:
        QuviException(const std::string&);
        const long& httpcode() const;
        const long& curlcode() const;
    };
};

class QuviVideo {
public:
    QuviVideo();
    QuviVideo(const std::string& url);
    virtual ~QuviVideo();
public:
    void parse(const std::string& url);
private:
    quvi_video_t video;
};

#define quvimgr QuviMgr::getInstance()

#endif


