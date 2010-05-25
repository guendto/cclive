/*
 * Copyright (C) 2009,2010 Toni Gundogdu.
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
#include <map>
#include <algorithm>

#include <pcrecpp.h>

#include "except.h"
#include "log.h"
#include "opts.h"
#include "util.h"
#include "quvi.h"
#include "exec.h"

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
    std::stringstream s;

    s << "libquvi: "
      << quvi_strerror(quvimgr.handle(),rc);

    switch (rc) {
    case QUVI_NOSUPPORT:
        throw NoSupportException(s.str());
    case QUVI_PCRE: // Replaced largely by QUVI_LUA.
    case QUVI_LUA:
        throw ParseException(s.str());
    default:
        break;
    }
    throw QuviException(s.str());
}

static int
status_callback(long param, void *data) {
    quvi_word status = quvi_loword(param);
    quvi_word type   = quvi_hiword(param);

    switch (status) {
    case QUVISTATUS_FETCH:
        switch (type) {
        default:
            logmgr.cout()
                << "fetch "
                << static_cast<char *>(data)
                << " ...";
            break;
        case QUVISTATUSTYPE_CONFIG:
            logmgr.cout()
                << "fetch config ...";
            break;
        case QUVISTATUSTYPE_PLAYLIST:
            logmgr.cout()
                << "fetch playlist ...";
            break;
        case QUVISTATUSTYPE_DONE:
            logmgr.cout()
                << "done."
                << std::endl;
            break;
        }
        break;

    case QUVISTATUS_VERIFY:
        switch (type) {
        default:
            logmgr.cout()
                << "verify video link ...";
            break;
        case QUVISTATUSTYPE_DONE:
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
    quvi_getinfo(quvi, QUVIINFO_CURL, curl);
}

static int video_num; // --number-videos index

// QuviVideo

QuviVideo::QuviVideo() {
    video_num = 0;
}

QuviVideo::QuviVideo(const std::string& url)
    : pageUrl(url)
{
}

void
QuviVideo::parse(std::string url /*=""*/) {

    if (url.empty())
        url = pageUrl;

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

#define wrap_getprop(id,dst,type) \
    do { type tmp; quvi_getprop(video, id, &tmp); dst = tmp; } while (0)

    wrap_getprop(QUVIPROP_HOSTID,      hostId,     char*);
    wrap_getprop(QUVIPROP_PAGEURL,     pageUrl,    char*);
    wrap_getprop(QUVIPROP_PAGETITLE,   pageTitle,  char*);
    wrap_getprop(QUVIPROP_VIDEOID,     videoId,    char*);

    size_t count = 0;
    do    { ++count; }
    while (quvi_next_videolink(video) == QUVI_OK);

    int i = 0;
    do {
        quvi::SHPQuviVideoLink q(new QuviVideoLink);
        q->nothing_todo = false;
        try {
            wrap_getprop(QUVIPROP_VIDEOURL,              q->url,       char*);
            wrap_getprop(QUVIPROP_VIDEOFILECONTENTTYPE,  q->ct,        char*);
            wrap_getprop(QUVIPROP_VIDEOFILESUFFIX,       q->suffix,    char*);
            wrap_getprop(QUVIPROP_VIDEOFILELENGTH,       q->length,    double);

            QuviVideo::toFileName(
                pageTitle,
                videoId,
                hostId,
                q,
                ++i,
                count
            );
        }
        catch (const NothingToDoException& x) {
            logmgr.cerr() << "file: " << q->filename << "\n";
            logmgr.cerr(x, false);
            q->nothing_todo = true;
        }

        videoLinks.push_back(q);

    } while (quvi_next_videolink(video) == QUVI_OK);

#undef wrap_getprop

    quvi_parse_close(&video);

    if (videoLinks.size() == 0)
        throw QuviNoVideoLinkException();

    if (opts.exec_run_given)
        execmgr.append(*this);

    // Start from the first link.
    resetVideoLink();
}

#define wrap_get_s(n,r) \
    const std::string& QuviVideo::n() const { return r; }

#define wrap_get_b(n,r) \
    const bool& QuviVideo::n() const { return r; }

wrap_get_s(getPageUrl,          pageUrl)
wrap_get_s(getPageTitle,        pageTitle)
wrap_get_s(getId,               videoId)
wrap_get_s(getHostId,           hostId)
wrap_get_s(getFileUrl,          (*currentVideoLink)->url)
wrap_get_s(getFileContentType,  (*currentVideoLink)->ct)
wrap_get_s(getFileSuffix,       (*currentVideoLink)->suffix)
wrap_get_s(getFileName,         (*currentVideoLink)->filename)
#undef wrap_get_s

#define wrap_get_f(n,r) \
    const double& QuviVideo::n() const { return r; }

wrap_get_f(getFileLength,       (*currentVideoLink)->length)
wrap_get_f(getInitialFileLength,(*currentVideoLink)->initial)
#undef wrap_get_s

wrap_get_b(getNothingTodo,      (*currentVideoLink)->nothing_todo)
#undef wrap_get_b

void
QuviVideo::resetVideoLink() {
    currentVideoLink = videoLinks.begin();
}

void
QuviVideo::nextVideoLink() {
    if (++currentVideoLink == videoLinks.end())
        throw QuviNoVideoLinkException();
}

void
QuviVideo::updateInitialLength() {
    quvi::SHPQuviVideoLink q = (*currentVideoLink);

    q->initial = Util::fileExists(q->filename);

    if (q->initial >= q->length)
        throw NothingToDoException();
}

void
QuviVideo::toFileName(
    const std::string& pageTitle,
    const std::string& videoId,
    const std::string& hostId,
    quvi::SHPQuviVideoLink qvl,
    const int& linkIndex,
    const int& totalLinks)
{
    const Options opts = optsmgr.getOptions();

    if (!opts.output_video_given) {
        std::stringstream b;

        if (opts.number_videos_given) {
            b << std::setw(4)
              << std::setfill('0')
              << ++video_num
              << "_";
        }

        // Append video segment / link number.
        if (totalLinks > 1) {
            const int w = totalLinks < 100 ? 2:4;
            b << std::setw(w)
              << std::setfill('0')
              << linkIndex
              << "of"
              << std::setw(w)
              << std::setfill('0')
              << totalLinks
              << "_";
        }

        // --filename-format
        std::string fmt   = opts.filename_format_arg;
        std::string title = pageTitle;

        // --regexp
        if (opts.regexp_given)
            Util::perlMatch(opts.regexp_arg, title);

        // Remove leading and trailing whitespace.
        pcrecpp::RE("^[\\s]+", pcrecpp::UTF8())
            .Replace("", &title);
        pcrecpp::RE("\\s+$", pcrecpp::UTF8())
            .Replace("", &title);

        // Replace format specifiers.
        Util::subStrReplace(fmt, "%t", title.empty() ? videoId : title);
        Util::subStrReplace(fmt, "%i", videoId);
        Util::subStrReplace(fmt, "%s", qvl->suffix);
        Util::subStrReplace(fmt, "%h", hostId);

        if (opts.substitute_given) {
            std::istringstream iss(opts.substitute_arg);

            quvi::StringVector regexps;
            std::copy(
                std::istream_iterator<std::string >(iss),
                std::istream_iterator<std::string >(),
                std::back_inserter<quvi::StringVector >(regexps)
            );

            for (quvi::StringVectorIter iter=regexps.begin();
                iter!=regexps.end();
                ++iter)
            {
                Util::perlSubstitute(*iter, fmt);
            }
        }

        b << fmt;
        qvl->filename = b.str();

        if (!opts.overwrite_given) {
            for (int i=0; i<INT_MAX; ++i) {
                qvl->initial = Util::fileExists(qvl->filename);
                if (!qvl->initial)
                    break;
                else if (qvl->initial >= qvl->length)
                    throw NothingToDoException();
                else {
                    if (opts.continue_given)
                        break;
                }
                std::stringstream tmp;
                tmp << b.str() << "." << i;
                qvl->filename = tmp.str();
            }
        }
    }
    else {
        qvl->initial = Util::fileExists(opts.output_video_arg);

        if (qvl->initial >= qvl->length)
            throw NothingToDoException();

        qvl->filename = opts.output_video_arg;
    }

    if (opts.overwrite_given)
        qvl->initial = 0;
}


