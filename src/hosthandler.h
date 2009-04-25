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

#ifndef hosthandler_h
#define hosthandler_h

class HostHandler {
public:
    HostHandler();
    virtual ~HostHandler();
public:
    virtual const bool  isHost   (std::string url)   = 0;
    virtual void        parseId  ()                  = 0;
    virtual void        parseLink()                  = 0;
public:
    void                    parsePage          (const std::string& pageContent);
    const VideoProperties&  getVideoProperties () const;
protected:
    std::string         pageContent;
    VideoProperties    props;
public:
    class ParseException : public RuntimeException {
    public:
        ParseException(const std::string&);
    };
};

// Youtube

class YoutubeHandler : public HostHandler {
public:
    virtual const bool  isHost   (std::string url);
    virtual void        parseId  ();
    virtual void        parseLink();
};

// Google

class GoogleHandler : public HostHandler {
public:
    virtual const bool  isHost   (std::string url);
    virtual void        parseId  ();
    virtual void        parseLink();
};

// Break

class BreakHandler : public HostHandler {
    virtual const bool  isHost   (std::string url);
    virtual void        parseId  ();
    virtual void        parseLink();
};

// EvisorTv

class EvisorHandler : public HostHandler {
    virtual const bool  isHost   (std::string url);
    virtual void        parseId  ();
    virtual void        parseLink();
};

// Sevenload

class SevenloadHandler : public HostHandler {
    virtual const bool  isHost   (std::string url);
    virtual void        parseId  ();
    virtual void        parseLink();
};

// Liveleak

class LiveleakHandler : public HostHandler {
    virtual const bool  isHost   (std::string url);
    virtual void        parseId  ();
    virtual void        parseLink();
};

// Dailymotion

class DailymotionHandler : public HostHandler {
    virtual const bool  isHost   (std::string url);
    virtual void        parseId  ();
    virtual void        parseLink();
};

#endif
