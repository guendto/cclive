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

#include "config.h"

#include <quvi/quvi.h>

static void
handleError(QUVIcode rc) {
    std::stringstream s;
    s << "quvi error: " << quvi_strerr(quvimgr.handle(),rc);
    throw QuviMgr::QuviException(s.str());
}

QuviMgr::QuviMgr()
    : quvi(NULL)
{
}

// Keeps -Weffc++ happy.
QuviMgr::QuviMgr(const QuviMgr&)
    : quvi(NULL)
{
}

// Ditto.
QuviMgr&
QuviMgr::operator=(const QuviMgr&) {
    return *this;
}

QuviMgr::~QuviMgr() {
    quvi_close(quvi);
    quvi = NULL;
}

void
QuviMgr::init() {
    QUVIcode rc = quvi_init(&quvi);
    if (rc != QUVI_OK)
        handle_error();
}

quvi_t
QuviMgr::handle() const {
    return quvi;
}

void
QuviMgr::curlHandle(CURL **curl) {
    quvi_getinfo(quvi, QUVII_CURL, curl);
}

// QuviVideo

QuviVideo::QuviVideo()
    : video(NULL)
{
}

QuviVideo::QuviVideo(const std::string& url)
    : video(NULL)
{
    parse(url);
}

QuviVideo::~QuviVideo() {
    quvi_parse_close(&video);
}

void
QuviVideo::parse(const std::string& url) {

    QUVIcode rc =
        quvi_parse(quvimgr.handle(), url.c_str(), &video);

    if (rc != QUVI_OK)
        handleError(rc);
}


