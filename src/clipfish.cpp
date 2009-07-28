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

#include "error.h"
#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "curl.h"
#include "hosthandler.h"

ClipfishHandler::ClipfishHandler()
    : HostHandler()
{
    props.setHost   ("clipfish");
    props.setDomain ("clipfish.de");
    props.setFormats("flv");
}

void
ClipfishHandler::parseId() {
    props.setId( Util::subStr(props.getPageLink(), "/video/", "/") );
}

void
ClipfishHandler::parseLink() {
    std::string config_url =
        "http://www.clipfish.de/video_n.php?p=0|DE&vid=" + props.getId();

    std::string config =
        curlmgr.fetchToMem(config_url, "config");

    props.setLink( Util::subStr(config, "&url=", "&") );
}
