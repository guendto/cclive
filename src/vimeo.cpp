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

#include <cstring>

#include "hosthandler.h"
#include "curl.h"
#include "opts.h"

VimeoHandler::VimeoHandler()
    : HostHandler()
{
    props.setHost   ("vimeo");
    props.setDomain ("vimeo.com");
    props.setFormats("flv|hd");
}

void
VimeoHandler::parseId() {
    props.setId( Util::subStr(pageContent, "clip_id=", "\"") );
}

void
VimeoHandler::parseTitle() {
    props.setTitle(
        Util::subStr(pageContent, "<meta name=\"title\" content=\"", "\""));
}

void
VimeoHandler::parseLink() {

    // config

    std::string config_path =
        "http://vimeo.com/moogaloop/load/clip:" + props.getId();

    std::string config = 
        curlmgr.fetchToMem(config_path, "config");

    std::string sign =
        Util::subStr( config, "<request_signature>", "</request_signature>" );

    std::string exp =
        Util::subStr( config,
            "<request_signature_expires>", "</request_signature_expires>" );

    std::string hd =
        Util::subStr( config, "<hd_button>", "</hd_button>", false );

    // video link

    std::string lnk =
        "http://vimeo.com/moogaloop/play/clip:" +props.getId()+
        "/" +sign+ "/" +exp;

    Options opts = optsmgr.getOptions();

    if (!strcmp(opts.format_arg, "best")
        || !strcmp(opts.format_arg, "hd"))
    {
        if (hd == "1")
            lnk += "/?q=hd";
    }

    props.setLink(lnk);
}
