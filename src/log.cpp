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

#include "config.h"

#include <fstream>
#include <cstdio>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "except.h"
#include "macros.h"
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
    : _out(NULL), _err(NULL), _cout(NULL), _cerr(NULL), rc(CCLIVE_OK)
{
    _init();
}

    // Keeps -Weffc++ happy.
LogMgr::LogMgr(const LogMgr&)
    : _out(NULL), _err(NULL), _cout(NULL), _cerr(NULL), rc(CCLIVE_OK)
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
    const Options opts = optsmgr.getOptions();
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

std::ostream&
LogMgr::cerr(const RuntimeException& except,
             const bool& prepend_newline /*=true*/)
{
    if (prepend_newline)
        *_cerr << "\n";

    *_cerr << "error: " << except.what() << std::endl;

    rc = except.getReturnCode();

    return *_cerr;
}

std::ostream&
LogMgr::cerr(const std::string& what,
             const bool& prepend_newline, /*=true*/
             const bool& prepend_error, /*=true*/
             const bool& append_newline /*=true*/)
{
    if (prepend_newline)
        *_cerr << "\n";

    if (prepend_error)
        *_cerr << "error: ";
        
    *_cerr << what;
    
    if (append_newline)
        *_cerr << std::endl;
    else
        *_cerr << std::flush;

    return *_cerr;
}

const ReturnCode&
LogMgr::getReturnCode() const {
    return rc;
}

void
LogMgr::resetReturnCode() {
    rc = CCLIVE_OK;
}
