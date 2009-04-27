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

#include <fstream>
#include <cstdio>

#include "singleton.h"
#include "macros.h"
#include "cmdline.h"
#include "opts.h"
#include "log.h"

// LogBuffer

LogBuffer::LogBuffer(const int& fd)
    : fd(fd), verbose(true)
{
    setp(buffer, buffer+(BufferSize-1));
}

LogBuffer::~LogBuffer() {
    sync();
}

void
LogBuffer::setVerbose(const bool& verbose) {
    this->verbose = verbose;
}

int
LogBuffer::flushBuffer() {
    const int n = pptr() - pbase();
    if (!verbose)
        return n;
    if (write(fd,buffer,n) != n)
        return EOF;
    pbump(-n);
    return n;
}

std::streambuf::int_type
LogBuffer::overflow(std::streambuf::int_type c) {
    if (c != EOF) {
        *pptr() = c;
        pbump(1);
    }
    if (flushBuffer() == EOF)
        return EOF;
    return c;
}

int
LogBuffer::sync() {
    if (flushBuffer() == EOF)
        return -1;
    return 0;
}

// LogMgr

LogMgr::LogMgr()
    : _out(NULL), _err(NULL), _cout(NULL), _cerr(NULL)
{
    _init();
}

    // Keeps -Weffc++ happy.
LogMgr::LogMgr(const LogMgr&)
    : _out(NULL), _err(NULL), _cout(NULL), _cerr(NULL)
{
    _init();
}

    // Ditto.
LogMgr&
LogMgr::operator=(const LogMgr&) {
    return *this;
}

void
LogMgr::_init() {
    _out = new LogBuffer( fileno(stdout) );
    _err = new LogBuffer( fileno(stderr) );

    _cout = new std::ostream(_out);
    _cerr = new std::ostream(_err);
}

void
LogMgr::init() {
    Options opts = optsmgr.getOptions();
    _out->setVerbose(!opts.quiet_given);
    _err->setVerbose(!opts.quiet_given);
}

LogMgr::~LogMgr() {
    _DELETE(_cerr);
    _DELETE(_cout);
    _DELETE(_err);
    _DELETE(_out);
}

std::ostream&
LogMgr::cout() const {
    return *_cout;
}

std::ostream&
LogMgr::cerr() const {
    return *_cerr;
}
