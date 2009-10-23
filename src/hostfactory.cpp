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

#include <tr1/memory>
#include <iostream>
#include <string>

#include "hosthandler.h"
#include "hostfactory.h"

#define SHP std::tr1::shared_ptr

typedef unsigned int _uint;

SHP<HostHandler>
HostHandlerFactory::createHandler(const std::string& url) {
    for (register _uint type=Youtube; type < _last_type; ++type) {
        SHP<HostHandler> p = createHandler((HandlerType)type);
        if (p->isHost(url))
            return p;
    }
    throw UnsupportedHostException(url);
}

void
HostHandlerFactory::printHosts() {
    for (register _uint type=Youtube; type < _last_type; ++type) {
        SHP<HostHandler> p = createHandler((HandlerType)type);
        VideoProperties props = p->getVideoProperties();
        std::cout << props.getDomain()
                  << "\t"
                  << props.getFormats()
                  << "\n";
    }
    std::cout
      << "\nNote: Some videos may have limited number of formats available.\n"
      << std::flush;
}

SHP<HostHandler>
HostHandlerFactory::createHandler(const HandlerType& type) {
    switch (type) {
    case Youtube:    return SHP<YoutubeHandler>    (new YoutubeHandler);
    case Google:     return SHP<GoogleHandler>     (new GoogleHandler);
    case Break:      return SHP<BreakHandler>      (new BreakHandler);
    case Evisor:     return SHP<EvisorHandler>     (new EvisorHandler);
    case Sevenload:  return SHP<SevenloadHandler>  (new SevenloadHandler);
    case Liveleak:   return SHP<LiveleakHandler>   (new LiveleakHandler);
    case Dailymotion:return SHP<DailymotionHandler>(new DailymotionHandler);
    case Vimeo:      return SHP<VimeoHandler>      (new VimeoHandler);
    case Golem:      return SHP<GolemHandler>      (new GolemHandler);
    case Clipfish:   return SHP<ClipfishHandler>   (new ClipfishHandler);
    case Funnyhub:   return SHP<FunnyhubHandler>   (new FunnyhubHandler);
    case Myubo:      return SHP<MyuboHandler>      (new MyuboHandler);
    case Cctv:       return SHP<CctvHandler>       (new CctvHandler);
    case Ehrensenf:  return SHP<EhrensenfHandler>  (new EhrensenfHandler);
    case Spiegel:    return SHP<SpiegelHandler>    (new SpiegelHandler);
    case Redtube:    return SHP<RedtubeHandler>    (new RedtubeHandler);
    case Youjizz:    return SHP<YoujizzHandler>    (new YoujizzHandler);
    case Xvideos:    return SHP<XvideosHandler>    (new XvideosHandler);
    // Keeps -Weffc++ quiet.
    case _last_type: break;
    }
    throw RuntimeException(CCLIVE_INTERNAL);
}

HostHandlerFactory::
    UnsupportedHostException::UnsupportedHostException(const std::string& url)
        : RuntimeException(CCLIVE_NOSUPPORT, url)
{
}


