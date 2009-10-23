/*
 * Copyright (C) 2009 i.am.a.real.coward@gmail.com
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

#include "hosthandler.h"

XvideosHandler::XvideosHandler()
    : HostHandler()
{
    props.setHost   ("xvideos");
    props.setDomain ("xvideos.com");
    props.setFormats("flv");
}

void
XvideosHandler::parseId() {
    std::string id;
    partialMatch("(?i)id_video=(.*?)&amp;", &id);
    props.setId(id);
}

void
XvideosHandler::parseTitle() {
    std::string title;
    partialMatch("(?i)description content=\"xvideos (.*?) free", &title);
    props.setTitle(title);
}

void
XvideosHandler::parseLink() {
  std::string lnk;
  partialMatch("(?i)flv_url=(.*?)&amp;", &lnk);
  curlmgr.escape(lnk);
  props.setLink(lnk);
}


