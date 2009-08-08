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

#include <fstream>
#include <string>
#include <vector>

#ifdef HAVE_ICONV
#include <iconv.h>
#endif

#include <pcrecpp.h>

#include "hosthandler.h"
#include "opts.h"
#include "log.h"
#include "curl.h"

HostHandler::HostHandler()
    : pageContent(""), props(VideoProperties())
{
}

HostHandler::~HostHandler() {
}

const bool
HostHandler::isHost(std::string url) {
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
HostHandler::parsePage(const std::string& url) {

    props.setPageLink(url);
    pageContent = curlmgr.fetchToMem(url);

    // Call overridden functions.
    parseTitle();
    parseId   ();
    parseLink ();

    // Handle title encoding. Done here since we still have page html.
    toUnicode();

    // Apply regexp to title.
    applyRegexp();

    pageContent.clear();
}

void
HostHandler::toUnicode() {
#ifdef HAVE_ICONV
    std::string charset;
    try {
        charset =
            Util::subStr(pageContent, "charset=", "\"");

        iconv_t cd =
            iconv_open( "UTF-8", charset.c_str() );

        if (cd == (iconv_t)-1) {
            if (errno == EINVAL) {
                logmgr.cerr()
                    << "warn: conversion from "
                    << charset
                    << " to UTF-8 not available"
                    << std::endl;
            }
            else
                perror("iconv_popen");
            return;
        }

        char inbuf[256];
        ICONV_CONST char *inptr = inbuf;
        size_t insize = props.getTitle().length();

        if (insize >= sizeof(inbuf))
            insize = sizeof(inbuf);

        snprintf(inbuf, sizeof(inbuf), props.getTitle().c_str());

        char outbuf[256];
        size_t avail = sizeof(outbuf);
        char *wptr   = (char *)outbuf;
        memset(wptr, 0, sizeof(outbuf));

        size_t rc = iconv(cd, &inptr, &insize, &wptr, &avail);
        if (rc == (size_t)-1) {
            logmgr.cerr()
                << "error while converting characters from "
                << charset
                << " to UTF-8"
                << std::endl;
            return;
        }
        iconv_close(cd);
        cd = 0;

        props.setTitle(outbuf);
    }
    catch (const HostHandler::ParseException&) { }
#endif
}

void
HostHandler::applyRegexp() {
    Options opts = optsmgr.getOptions();

    if (!opts.regexp_given)
        return;

    std::string title;

    if (opts.find_all_given) {
        pcrecpp::StringPiece sp(props.getTitle());
        pcrecpp::RE re(opts.regexp_arg, pcrecpp::UTF8());

        std::string s;
        while (re.FindAndConsume(&sp, &s))
            title += s;
    }
    else {
        pcrecpp::RE(opts.regexp_arg, pcrecpp::UTF8())
            .PartialMatch(props.getTitle(), &title);
    }

    // Remove leading and trailing whitespace.
    pcrecpp::RE("(\\s+)").Replace("", &title);
    pcrecpp::RE("(\\s+)$").Replace("", &title);

    props.setTitle(title);
}

const VideoProperties&
HostHandler::getVideoProperties() const {
    return props;
}

HostHandler::
    ParseException::ParseException(const std::string& error)
        : RuntimeException(CCLIVE_PARSE, error)
{
}
