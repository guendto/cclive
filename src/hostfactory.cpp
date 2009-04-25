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

#include <iostream>
#include <string>
#include <tr1/memory>

#include "except.h"
#include "video.h"
#include "hosthandler.h"
#include "hostfactory.h"

std::tr1::shared_ptr<HostHandler>
HostHandlerFactory::createHandler(const std::string& url) {
    for (register int type=Youtube; type < _last_type; ++type) {
        std::tr1::shared_ptr<HostHandler> p = createHandler((HandlerType)type);
        if (p->isHost(url))
            return p;
    }
    throw UnsupportedHostException();
}

void
HostHandlerFactory::printHosts() {
    for (register int type=Youtube; type < _last_type; ++type) {
        std::tr1::shared_ptr<HostHandler> p = createHandler((HandlerType)type);
        p->isHost(""); // Sets the domain string
        std::cout << p->getVideoProperties().getDomain() << "\n";
    }
    std::cout << std::flush;
}

std::tr1::shared_ptr<HostHandler>
HostHandlerFactory::createHandler(const HandlerType& type) {
    switch (type) {
        case Youtube: {
            std::tr1::shared_ptr<YoutubeHandler> p(new YoutubeHandler);
            return p;
        }
        case Google: {
            std::tr1::shared_ptr<GoogleHandler> p(new GoogleHandler);
            return p;
        }
        case Break: {
            std::tr1::shared_ptr<BreakHandler> p(new BreakHandler);
            return p;
        }
        case Evisor: {
            std::tr1::shared_ptr<EvisorHandler> p(new EvisorHandler);
            return p;
        }
        case Sevenload: {
            std::tr1::shared_ptr<SevenloadHandler> p(new SevenloadHandler);
            return p;
        }
        case Liveleak: {
            std::tr1::shared_ptr<LiveleakHandler> p(new LiveleakHandler);
            return p;
        }
        case Dailymotion: {
            std::tr1::shared_ptr<DailymotionHandler> p(new DailymotionHandler);
            return p;
        }
        default:
            break;
    }
    throw UnsupportedHostException();
}
