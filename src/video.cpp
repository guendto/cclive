/*
 * cclive Copyright (C) 2009 Toni Gundogdu. This file is part of cclive.
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

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <climits>

#include "util.h"
#include "singleton.h"
#include "cmdline.h"
#include "opts.h"
#include "except.h"
#include "curl.h"
#include "video.h"

VideoProperties::VideoProperties()
    : id(""),     link(""),        host(""),       title(""),   length(0), 
      initial(0), contentType(""), suffix("flv"),  filename("")
{
}

void
VideoProperties::setId(const std::string& id) {
    this->id = id;
}

void
VideoProperties::setLink(std::string link) {
    this->link = curlmgr.unescape(link);
}

void
VideoProperties::setHost(const std::string& host) {
    this->host = host;
}

void
VideoProperties::setLength(const double length) {
    this->length = length;
}

void
VideoProperties::setContentType(const std::string& contentType) {
    this->contentType = contentType;

    std::string::size_type pos = contentType.find("/");

    if (pos != std::string::npos) {
        suffix = contentType.substr(pos+1);
        // set to "flv" for these:
        if (suffix.find("octet") != std::string::npos
            || suffix.find("x-flv") != std::string::npos
            || suffix.find("plain") != std::string::npos)
        {
            suffix = "flv";
        }
    }

    // We have all needed info now.
    formatOutputFilename();
}

void
VideoProperties::setInitial(const double initial) {
    this->initial = initial;
}

void
VideoProperties::setTitle(const std::string& title) {
    this->title = title;
}

const std::string&
VideoProperties::getId() const {
    return id;
}

const std::string&
VideoProperties::getLink() const {
    return link;
}

const std::string&
VideoProperties::getHost() const {
    return host;
}

const double
VideoProperties::getLength() const {
    return length;
}

const double
VideoProperties::getInitial() const {
    return initial;
}

const std::string&
VideoProperties::getContentType() const {
    return contentType;
}

const std::string&
VideoProperties::getFilename() const {
    return filename;
}

static int video_num;

void
VideoProperties::formatOutputFilename() {
    Util::subStrReplace(id, "-", "_");

    Options opts = optsmgr.getOptions();

    if (!opts.output_video_given) {
        std::stringstream b;

        if (opts.number_videos_given)
            b << ++video_num << "_";

        if (opts.title_given && title.length() > 0)
            b << title;
        else
            b << host << "_" << id;

        b << "." << suffix;
        filename = b.str();

        for (register int i=1; i<INT_MAX; ++i) {
            initial = Util::fileExists(filename);

            if (initial == 0)
                break;
            else if (initial == length)
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
        if (initial == length)
            throw NothingToDoException();
        filename = opts.output_video_arg;
    }

    if (!opts.continue_given)
        initial = 0;
}

VideoProperties::
    NothingToDoException::NothingToDoException()
    : RuntimeException()
{
}
