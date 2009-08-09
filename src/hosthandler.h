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

#ifndef hosthandler_h
#define hosthandler_h

#include <string>
#include <vector>

#include <pcrecpp.h>

#include "except.h"
#include "video.h"
#include "util.h"

class HostHandler {
public:
    HostHandler();
    virtual ~HostHandler();
public:
    virtual void parseId   () = 0;
    virtual void parseTitle() = 0;
    virtual void parseLink () = 0;
public:
    void                    parsePage          (const std::string& url);
    const bool              isHost             (std::string url);
    const VideoProperties&  getVideoProperties () const;
    void                    partialMatch       (const std::string& re,
                                                const pcrecpp::Arg& dst,
                                                const std::string& data="");
private:
    void toUnicode   ();
    void applyRegexp (std::string& title);
protected:
    std::string     pageContent;
    VideoProperties props;
public:
    class ParseException : public RuntimeException {
    public:
        ParseException(const std::string&);
    };
};

// Youtube

class YoutubeHandler : public HostHandler {
public:
    YoutubeHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Google

class GoogleHandler : public HostHandler {
public:
    GoogleHandler();
    virtual void  parseId   ();
    virtual void  parseTitle();
    virtual void  parseLink ();
};

// Break

class BreakHandler : public HostHandler {
public:
    BreakHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// EvisorTv

class EvisorHandler : public HostHandler {
public:
    EvisorHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Sevenload

class SevenloadHandler : public HostHandler {
public:
    SevenloadHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Liveleak

class LiveleakHandler : public HostHandler {
public:
    LiveleakHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Dailymotion

class DailymotionHandler : public HostHandler {
public:
    DailymotionHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Vimeo

class VimeoHandler : public HostHandler {
public:
    VimeoHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Golem

class GolemHandler : public HostHandler {
public:
    GolemHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Clipfish

class ClipfishHandler : public HostHandler {
public:
    ClipfishHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Funnyhub

class FunnyhubHandler : public HostHandler {
public:
    FunnyhubHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

// Myubo

class MyuboHandler : public HostHandler {
public:
	MyuboHandler();
    virtual void parseId   ();
    virtual void parseTitle();
    virtual void parseLink ();
};

#endif
