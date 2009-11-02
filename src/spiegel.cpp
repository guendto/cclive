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

#include <map>

#include "hosthandler.h"

#define HOST "spiegel"

SpiegelHandler::SpiegelHandler()
    : HostHandler()
{
    props.setHost   (HOST);
    props.setDomain ("spiegel.de");
    props.setFormats("flv|vp6_576|vp_928|h264_1400");
}

void
SpiegelHandler::parseId() {
    std::string id;
    partialMatch("(?i)\\/video\\/video-(.*?)\\.", &id, props.getPageLink());
    props.setId(id);
}

void
SpiegelHandler::parseTitle() {
    // Done in parseLink.
}

void
SpiegelHandler::parseLink() {

    const std::string playlist_url =
        "http://www1.spiegel.de/active/playlist/fcgi/playlist.fcgi/"
        "asset=flashvideo/mode=id/id=" + props.getId();

    const std::string pl =
        fetch(playlist_url, "playlist", true);

    std::string headline;
    partialMatch("(?i)<headline>(.*?)</", &headline, pl);
    props.setTitle(headline);

    const std::string config =
        fetch("http://video.spiegel.de/flash/"
            + props.getId() + ".xml", "config", true);

    pcrecpp::StringPiece tmp(config);
    pcrecpp::RE re("(?i)<filename>(.*?)</");

    const std::string _re("(?i)_(\\d+)\\.");
    pcrecpp::RE re_rate(_re);

    std::string format =
        Util::parseFormatMap(HOST);

    if (format == "flv")
        format = "vp6_64";

    std::map<std::string, std::string> m;
    std::string path, rate, _path;

    while (re.FindAndConsume(&tmp, &path)) {
        if (re_rate.PartialMatch(path, &rate))
            m[rate] = path;

        if (pcrecpp::RE("(?i)"+format).PartialMatch(path))
            _path = path; // Grab this while here.
    }

    if (format == "best")
        _path = (m.begin())->second;

    if (_path.empty())
        throw HostHandler::ParseException("unable to construct video link");

    std::string lnk = "http://video.spiegel.de/flash/" + _path;
    curlmgr.escape(lnk);
    props.setLink(lnk);
}


