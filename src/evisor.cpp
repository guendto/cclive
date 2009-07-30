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

#include "hosthandler.h"

EvisorHandler::EvisorHandler()
    : HostHandler()
{
    props.setHost   ("evisor");
    props.setDomain ("evisor.tv");
    props.setFormats("flv");
}

void
EvisorHandler::parseId() {
    props.setId( Util::subStr(pageContent, "unit_long", "\"") );
}

void
EvisorHandler::parseLink() {
    props.setLink( Util::subStr(pageContent, "file=", "\"") );
}
