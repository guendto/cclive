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

#ifndef log_h
#define log_h

#include "singleton.h"

// LogBuffer

class LogBuffer : public std::streambuf {
public:
    LogBuffer(const int& fd, const bool& close_after);
    virtual ~LogBuffer();
public:
    void setVerbose(const bool&);
protected:
    int              flushBuffer ();
    virtual int_type overflow    (int_type);
    virtual int      sync        ();
protected:
    enum { BufferSize=1024 };
    char buffer[BufferSize];
private:
    int fd;
    bool verbose;
    bool close_after;
};

// LogMgr

class LogMgr : public Singleton<LogMgr> {
public:
    LogMgr            ();
    LogMgr            (const LogMgr&);
    LogMgr& operator= (const LogMgr&);
    virtual ~LogMgr   ();
public:
    void               init();
    std::ostream&      cout()  const;
    std::ostream&      cerr()  const;
    std::ostream&      cerr(const RuntimeException& except,
                            const bool& prepend_newline=true);
    std::ostream&      cerr(const std::string& what,
                            const bool& prepend_newline=true,
                            const bool& prepend_error=true,
                            const bool& append_newline=true);
    const ReturnCode&  getReturnCode() const;
    void               resetReturnCode();
protected:
    void            _init(const std::string& fname="");
private:
    LogBuffer    *lbout, *lberr;
    std::ostream *oscout, *oscerr;
    ReturnCode   rc;
    std::string  fname;
};

#define logmgr LogMgr::getInstance()

#endif


