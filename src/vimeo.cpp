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
#include <cstring>

#include "error.h"
#include "except.h"
#include "video.h"
#include "util.h"
#include "singleton.h"
#include "curl.h"
#include "cmdline.h"
#include "opts.h"
#include "hosthandler.h"

const bool
VimeoHandler::isHost(std::string url) {
    props.setHost   ("vimeo");
    props.setDomain ("vimeo.com");
    return Util::toLower(url).find(props.getDomain())
        != std::string::npos;
}

void
VimeoHandler::parseId() {
    const char *begin = "clip_id=";
    const char *end   = "\"";

    props.setId( Util::subStr(pageContent, begin, end) );
}

void
VimeoHandler::parseLink() {

    // config

    std::string config_path =
        "http://vimeo.com/moogaloop/load/clip:" + props.getId();

    std::string config = 
        curlmgr.fetchToMem(config_path, "config");

    const char *sig_begin = "<request_signature>";
    const char *sig_end   = "</request_signature>";

    std::string sign = Util::subStr( config, sig_begin, sig_end );

    const char *exp_begin = "<request_signature_expires>";
    const char *exp_end   = "</request_signature_expires>";

    std::string exp = Util::subStr( config, exp_begin, exp_end );

    const char *hd_begin = "<hd_button>";
    const char *hd_end   = "</hd_button>";

    std::string hd = Util::subStr( config, hd_begin, hd_end, false );

    // video link

    std::string xurl =
        "http://vimeo.com/moogaloop/play/clip:" +props.getId()+
        "/" +sign+ "/" +exp;

    Options opts = optsmgr.getOptions();

    if (!strcmp(opts.format_arg, "best")
        || !strcmp(opts.format_arg, "hd"))
    {
        if (hd == "1")
            xurl += "/?q=hd";
    }

    props.setLink(xurl);
}
