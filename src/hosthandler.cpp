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

#include "hosthandler.h"
#include "opts.h"
#include "log.h"
#include "curl.h"
#include "retry.h"

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

#ifdef HAVE_ICONV
    // Convert character set encoding to utf8. Skip if charset was not found.
    try   {
        std::string charset;
        partialMatch("(?i)charset=\"?(.*?)([\"\\/>\\s]|$)", &charset);

        std::string tmp = props.getTitle();
        props.setTitle( Util::toUnicode(tmp, charset) );
    }
    catch (const HostHandler::ParseException& x) { }
#endif

    std::string title = props.getTitle();

    // Convert predefined HTML character entities.
    Util::fromHtmlEntities(title);

    const Options opts = optsmgr.getOptions();

    // Apply regexp.
    if (opts.regexp_given)
        Util::perlMatch(optsmgr.getOptions().regexp_arg, title);

    // Remove leading and trailing whitespace.
    pcrecpp::RE("^[\\s]+").Replace("", &title);
    pcrecpp::RE("\\s+$").Replace("", &title);

    props.setTitle(title);

    pageContent.clear();
}

typedef CurlMgr::FetchException FetchError;

std::string
HostHandler::fetch(const std::string& url,
                    const std::string& what,
                    const bool& reset/*=false*/)
{
    if (reset)
        retrymgr.reset();

    std::string tmp;
    try   { tmp = curlmgr.fetchToMem(url,what); }
    catch (const FetchError& x) {
        retrymgr.handle(x);
        fetch(url,what);
    }
    return tmp;
}

void
HostHandler::partialMatch(
    const std::string& re,
    const pcrecpp::Arg& dst,
    const std::string& data/*=""*/)
{
    const std::string& content = !data.empty() ? data : pageContent;
    if (!pcrecpp::RE(re).PartialMatch(content, dst))
        throw HostHandler::ParseException("no match: "+re);
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


