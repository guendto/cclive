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

#include <sstream>
#include <vector>
#include <iomanip>

#include <pcrecpp.h>

#include "except.h"
#include "log.h"
#include "opts.h"
#include "util.h"
#include "quvi.h"

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
    quvi_close(&quvi);
}

static void
handle_error(QUVIcode rc) {

    switch (rc) {
    case QUVI_NOSUPPORT:
        throw NoSupportException(s.str());
    case QUVI_PCRE:
        throw ParseException(s.str());
    default:
        break;
    }

    std::stringstream s;

    s << "quvi: "
      << quvi_strerror(quvimgr.handle(),rc);

    throw QuviException(s.str());
}

static int
status_callback(long param, void *data) {
    quvi_word status = quvi_loword(param);
    quvi_word type   = quvi_hiword(param);

    switch (status) {
    case QUVIS_FETCH:
        switch (type) {
        default:
            logmgr.cout()
                << "fetch "
                << static_cast<char *>(data)
                << " ...";
            break;
        case QUVIST_CONFIG:
            logmgr.cout()
                << "fetch config ...";
            break;
        case QUVIST_PLAYLIST:
            logmgr.cout()
                << "fetch playlist ...";
            break;
        case QUVIST_DONE:
            logmgr.cout()
                << "done."
                << std::endl;
            break;
        }
        break;

    case QUVIS_VERIFY:
        switch (type) {
        default:
            logmgr.cout()
                << "verify video link ...";
            break;
        case QUVIST_DONE:
            logmgr.cout()
                << "done."
                << std::endl;
            break;
        }
        break;
    }

    logmgr.cout() << std::flush;

    return 0;
}

void
QuviMgr::init() {
    QUVIcode rc = quvi_init(&quvi);
    if (rc != QUVI_OK)
        handle_error(rc);

    quvi_setopt(quvi,
        QUVIOPT_STATUSFUNCTION, status_callback);
}

quvi_t
QuviMgr::handle() const {
    return quvi;
}

void
QuviMgr::curlHandle(CURL **curl) {
    assert(curl != 0);
    quvi_getinfo(quvi, QUVII_CURL, curl);
}

// QuviVideo

QuviVideo::QuviVideo()
    : length    (0),
      pageLink  (""),
      id        (""),
      title     (""),
      link      (""),
      suffix    (""),
      contentType(""),
      hostId    (""),
      initial   (0),
      filename  ("")
{
}

QuviVideo::QuviVideo(const std::string& url)
    : length    (0),
      pageLink  (url),
      id        (""),
      title     (""),
      link      (""),
      suffix    (""),
      contentType(""),
      hostId    (""),
      initial   (0),
      filename  ("")
{
}

QuviVideo::QuviVideo(const QuviVideo& o)
    : length    (o.length),
      pageLink  (o.pageLink),
      id        (o.id),
      title     (o.title),
      link      (o.link),
      suffix    (o.suffix),
      contentType(o.contentType),
      hostId    (o.hostId),
      initial   (o.initial),
      filename  (o.filename)
{
}

QuviVideo&
QuviVideo::operator=(const QuviVideo& o) {
    length  = o.length;
    pageLink= o.pageLink;
    id      = o.id;
    title   = o.title;
    link    = o.link;
    suffix  = o.suffix;
    contentType = o.contentType;
    hostId  = o.hostId;
    initial = o.initial;
    filename= o.filename;
    return *this;
}

QuviVideo::~QuviVideo() {
}

void
QuviVideo::parse(std::string url /*=""*/) {

    if (url.empty())
        url = pageLink;

    assert(!url.empty());

    const Options opts =
        optsmgr.getOptions();

    if (opts.format_given) {
        quvi_setopt(quvimgr.handle(),
            QUVIOPT_FORMAT, opts.format_arg);
    }

    CURL *curl = 0;
    quvimgr.curlHandle(&curl);
    assert(curl != 0);

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,
        opts.connect_timeout_arg);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT,
        opts.connect_timeout_socks_arg);

    quvi_video_t video;

    QUVIcode rc =
        quvi_parse(quvimgr.handle(),
            const_cast<char*>(url.c_str()), &video);

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L);

    if (rc != QUVI_OK)
        handle_error(rc);

    quvi_getprop(video, QUVIP_LENGTH, &length);

#define _getprop(id,dst) \
    do { quvi_getprop(video, id, &s); dst = s; } while (0)

    char *s; // _getprop uses this.
    _getprop(QUVIP_PAGELINK, pageLink);
    _getprop(QUVIP_ID, id);
    _getprop(QUVIP_TITLE, title);
    _getprop(QUVIP_LINK, link);
    _getprop(QUVIP_SUFFIX, suffix);
    _getprop(QUVIP_CONTENTTYPE, contentType);
    _getprop(QUVIP_HOSTID, hostId);
#undef _getstr
    quvi_parse_close(&video);

    formatOutputFilename();
}

static int video_num = 0;

void
QuviVideo::formatOutputFilename() {

    const Options opts = optsmgr.getOptions();

    if (!opts.output_video_given) {
        std::stringstream b;

        if (opts.number_videos_given) {
            b << std::setw(4)
              << std::setfill('0')
              << ++video_num
              << "_";
        }

        customOutputFilenameFormatter(b);

        filename = b.str();

        typedef unsigned int _uint;

        for (register _uint i=1;
            i<INT_MAX && !opts.overwrite_given; ++i) {

            initial = Util::fileExists(filename);

            if (initial == 0)
                break;
            else if (initial >= length)
                throw NothingToDoException();
            else {
                if (opts.continue_given)
                    break;
            }

            std::stringstream tmp;
            tmp << b.str() << "." << i;
            filename = tmp.str();
        }
    }
    else {
        initial = Util::fileExists(opts.output_video_arg);

        if (initial >= length)
            throw NothingToDoException();

        if (opts.overwrite_given)
            initial = 0;

        filename = opts.output_video_arg;
    }

    if (!opts.continue_given)
        initial = 0;
}

void
QuviVideo::customOutputFilenameFormatter(
    std::stringstream& b)
{
    const Options opts  = optsmgr.getOptions();
    std::string fmt     = opts.filename_format_arg;

    std::string _id = this->id;
    Util::subStrReplace(_id, "-", "_");

    std::string _title = title;

    // Apply --regexp.
    if (opts.regexp_given)
        applyTitleRegexp(_title);

    // Remove leading and trailing whitespace.
    pcrecpp::RE("^[\\s]+", pcrecpp::UTF8())
        .Replace("", &_title);
    pcrecpp::RE("\\s+$", pcrecpp::UTF8())
        .Replace("", &_title);

    // Replace format specifiers.
    Util::subStrReplace(fmt, "%t", _title.empty() ? _id : _title);
    Util::subStrReplace(fmt, "%i", _id);
    Util::subStrReplace(fmt, "%h", hostId);
    Util::subStrReplace(fmt, "%s", suffix);

    b << fmt;
}

void
QuviVideo::applyTitleRegexp(std::string& src) {

    const Options opts =
        optsmgr.getOptions();

    if (opts.find_all_given) {
        pcrecpp::StringPiece sp(src);
        pcrecpp::RE re(opts.regexp_arg, pcrecpp::UTF8());

        src.clear();

        std::string s;
        while (re.FindAndConsume(&sp, &s))
            src += s;
    }
    else {
        std::string tmp = src;
        src.clear();

        pcrecpp::RE(opts.regexp_arg, pcrecpp::UTF8())
            .PartialMatch(tmp, &src);
    }
}

const double&
QuviVideo::getLength() const {
    return length;
}

const std::string&
QuviVideo::getPageLink() const {
    return pageLink;
}

const std::string&
QuviVideo::getId() const {
    return id;
}

const std::string&
QuviVideo::getTitle() const {
    return title;
}

const std::string&
QuviVideo::getLink() const {
    return link;
}

const std::string&
QuviVideo::getSuffix() const {
    return suffix;
}

const std::string&
QuviVideo::getContentType() const {
    return contentType;
}

const std::string&
QuviVideo::getHostId() const {
    return hostId;
}

const double&
QuviVideo::getInitial() const {
    return initial;
}

const std::string&
QuviVideo::getFilename() const {
    return filename;
}

void
QuviVideo::updateInitial() {
    initial = Util::fileExists(filename);

    if (initial >= length)
        throw NothingToDoException();
}


