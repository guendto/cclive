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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <curl/curl.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "except.h"
#include "opts.h"
#include "macros.h"
#include "video.h"
#include "progressbar.h"
#include "log.h"
#include "curl.h"
#include "retry.h"

static CURL *curl;

static std::string
formatError (const long& httpcode) {
    std::stringstream s;
    s << "server returned http/" << httpcode << "";
    return s.str();
}

static std::string
formatError (const CURLcode& code) {
    std::stringstream s;
    s << curl_easy_strerror(code) << " (rc=" << code << ")";
    return s.str();
}

CurlMgr::CurlMgr()
    : httpcode(0)
{
    curl = NULL;
}

// Keeps -Weffc++ happy.
CurlMgr::CurlMgr(const CurlMgr& o)
    : httpcode(o.httpcode)
{
    curl = NULL;
}

// Ditto.
CurlMgr&
CurlMgr::operator=(const CurlMgr&) {
    return *this;
}

CurlMgr::~CurlMgr() {
    curl_easy_cleanup(curl);
    curl = NULL;
}

void
CurlMgr::init() {
    curl = curl_easy_init();
    if (!curl)
        throw RuntimeException(CCLIVE_CURLINIT);

    const Options opts = optsmgr.getOptions();

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, opts.agent_arg);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, opts.debug_given);

    const char *proxy = opts.proxy_arg;
    if (opts.no_proxy_given)
        proxy = "";

    curl_easy_setopt(curl, CURLOPT_PROXY, proxy);
}

static void *
_realloc(void *p, const size_t size) {
    if (p) return realloc(p,size);
    return malloc(size);
}

struct mem_s {
    size_t size;
    char *p;
};

static size_t
callback_writemem(void *p, size_t size, size_t nmemb, void *data) {
    mem_s *m = reinterpret_cast<mem_s*>(data);
    const size_t rsize = size * nmemb;
    void *tmp = _realloc(m->p, m->size+rsize+1);
    if (tmp) {
        m->p = reinterpret_cast<char*>(tmp);
        memcpy(&(m->p[m->size]), p, rsize);
        m->size += rsize;
        m->p[m->size] = '\0';
    }
    return rsize;
}

std::string
CurlMgr::fetchToMem(const std::string& url, const std::string &what) {
    logmgr.cout() << "fetch ";
    if (what.empty())
        logmgr.cout() << url;
    else
        logmgr.cout() << what;
    logmgr.cout() << " ..." << std::flush;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_ENCODING, "");

    mem_s mem;
    memset(&mem, 0, sizeof(mem));

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_writemem);

    const Options opts = optsmgr.getOptions();

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,
        opts.connect_timeout_arg);

    /*
     * http://curl.haxx.se/docs/knownbugs.html:
     * 
     * "When connecting to a SOCKS proxy, the (connect) timeout is
     * not properly acknowledged after the actual TCP connect (during
     * the SOCKS 'negotiate' phase)."
     */
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,
        opts.connect_timeout_socks_arg);

    const CURLcode rc =
        curl_easy_perform(curl);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L);

    std::string errmsg;

    httpcode = 0;

    if (CURLE_OK == rc) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);
        if (httpcode == 200 || httpcode == 206)
            logmgr.cout() << "done." << std::endl;
        else
            errmsg = formatError(httpcode);
    }
    else
        errmsg = formatError(rc);

    std::string content;

    if (NULL != mem.p) {
        content = mem.p;
        _FREE(mem.p);
    }

    if (!errmsg.empty())
        throw FetchException(errmsg, httpcode);

    return content;
}

void
CurlMgr::queryFileLength(VideoProperties& props) {
    logmgr.cout() << "verify video link ..." << std::flush;

    const Options opts = optsmgr.getOptions();

    curl_easy_setopt(curl, CURLOPT_URL, props.getLink().c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);      // GET -> HEAD

    mem_s mem;
    memset(&mem, 0, sizeof(mem));

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_writemem);

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,
                     opts.connect_timeout_arg);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,
                     opts.connect_timeout_socks_arg);

    const CURLcode rc =
        curl_easy_perform(curl);

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L); // reset HEAD -> GET

    std::string errmsg;

    httpcode = 0;

    if (CURLE_OK == rc)
    {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);

        if (httpcode == 200 || httpcode == 206)
        {
            const char *ct = NULL;
            curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);

            double len = 0;
            curl_easy_getinfo(curl,
                CURLINFO_CONTENT_LENGTH_DOWNLOAD, &len);

            props.setLength(len);
            props.setContentType(ct);

            logmgr.cout() << "done." << std::endl;
        }
        else
            errmsg = formatError(httpcode);
    }
    else
        errmsg = formatError(rc);

    if (NULL != mem.p)
        _FREE(mem.p);

    if (!errmsg.empty())
        throw FetchException(errmsg, httpcode);

    props.formatOutputFilename();
}

struct write_s {
    write_s()
        : filename(NULL), initial(0), file(NULL) { }
    char *filename;
    double initial;
    FILE *file;
};

static size_t
callback_writefile(void *data, size_t size, size_t nmemb, void *p) {
    write_s *w = reinterpret_cast<write_s*>(p);
    if (NULL != w && !w->file && NULL != w->filename) {
        const char *mode = w->initial > 0 ? "ab" : "wb";
        w->file = fopen(w->filename, mode);
        if (!w->file)
            return -1;
    }
    return fwrite(data, size, nmemb, w->file);
}

int
callback_progress(
    void *p,
    double total,
    double now,
    double utotal,
    double unow)
{
    ProgressBar *pb = static_cast<ProgressBar*>(p);
    pb->update(now);
    return 0;
}

void
CurlMgr::fetchToFile(VideoProperties& props) {
    const Options opts   = optsmgr.getOptions();

    bool continue_given =
        static_cast<bool>(opts.continue_given);

    if (retrymgr.getRetryUntilRetrievedFlag()) {
        props.updateInitial();
        continue_given = true;
    }

    const double initial = props.getInitial();

    if (continue_given && initial > 0) {

        const double remaining =
            props.getLength() - initial;

        logmgr.cout()
            << "from: "
            << std::setprecision(0)
            << initial
            << " ("
            << std::setprecision(1)
            << _TOMB(initial)
            << "M)  remaining: "
            << std::setprecision(0)
            << remaining
            << " ("
            << std::setprecision(1)
            << _TOMB(remaining)
            << "M)"
            << std::endl;
    }

    curl_easy_setopt(curl, CURLOPT_URL, props.getLink().c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_writefile);

    write_s write;
    memset(&write, 0, sizeof(write));

    write.initial  = initial;
    write.filename = const_cast<char*>(props.getFilename().c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &write);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, &callback_progress);

    ProgressBar pb;
    pb.init(props);
    curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &pb);

    curl_easy_setopt(curl, CURLOPT_ENCODING, "identity");
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
    curl_easy_setopt(curl, CURLOPT_RESUME_FROM, static_cast<long>(initial));
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT,
                     opts.connect_timeout_arg);

    curl_off_t limit_rate = opts.limit_rate_arg * 1024;
    curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, limit_rate);

    const CURLcode rc =
        curl_easy_perform(curl);

    curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_RESUME_FROM, 0L);
    curl_easy_setopt(curl, CURLOPT_MAX_RECV_SPEED_LARGE, (curl_off_t) 0);

    if (NULL != write.file) {
        fflush(write.file);
        fclose(write.file);
    }

    std::string errmsg;

    httpcode = 0;

    if (CURLE_OK == rc) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);
        if (httpcode != 200 && httpcode != 206)
            errmsg = formatError(httpcode);
    }
    else
        errmsg = formatError(rc);

    if (!errmsg.empty())
        throw FetchException(errmsg, httpcode);

    pb.finish();
    logmgr.cout() << std::endl;
}

const std::string&
CurlMgr::unescape(std::string& url) const {
    char *p = curl_easy_unescape(curl, url.c_str(), 0, 0);
    if (!p)
        throw RuntimeException(CCLIVE_SYSTEM, "curl_easy_unescape: returned null");
    url     = p;
    curl_free(p);
    return url;
}

const std::string&
CurlMgr::escape(std::string& url) const {
    char *p = curl_easy_escape(curl, url.c_str(), 0);
    if (!p)
        throw RuntimeException(CCLIVE_SYSTEM, "curl_easy_escape: returned null");
    url = p;
    curl_free(p);
    return url;
}

CurlMgr::FetchException::FetchException(
    const std::string& error,
    const long& httpcode)
    : RuntimeException(CCLIVE_FETCH, error), httpcode(httpcode)
{
}

const long&
CurlMgr::FetchException::getHTTPCode() const {
    return httpcode;
}
