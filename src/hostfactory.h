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

#ifndef hostfactory_h
#define hostfactory_h

class HostHandler;

class HostHandlerFactory {
public:
    static std::tr1::shared_ptr<HostHandler>
        createHandler(const std::string& url);
    static void printHosts();
protected:
    enum HandlerType {
        Youtube,    Google,    Break,       Evisor,
        Sevenload,  Liveleak,  Dailymotion, Vimeo,
        Golem,      Clipfish,  Funnyhub,    Myubo,
		Cctv,       Ehrensenf, Spiegel,     
        Redtube,    _last_type };
protected:
    static std::tr1::shared_ptr<HostHandler>
        createHandler(const HandlerType& type);
public:
    class UnsupportedHostException : public RuntimeException {
    public:
        UnsupportedHostException(const std::string& url);
    };
};

#endif
