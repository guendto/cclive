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
#include <sys/stat.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
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
    close(fd);
}

void
LogBuffer::setVerbose(const bool& verbose) {
    this->verbose = verbose;
}

int
LogBuffer::flushBuffer() {
    if (!verbose)
        return EOF;
    const int n = pptr() - pbase();
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
    : lbout(NULL), lberr(NULL), oscout(NULL), oscerr(NULL),
      rc(CCLIVE_OK), fname("")
{
    _init();
}

    // Keeps -Weffc++ happy.
LogMgr::LogMgr(const LogMgr&)
    : lbout(NULL), lberr(NULL), oscout(NULL), oscerr(NULL),
      rc(CCLIVE_OK), fname("")
{
    _init();
}

    // Ditto.
LogMgr&
LogMgr::operator=(const LogMgr&) {
    return *this;
}

void
LogMgr::_init(const std::string& fname) {

    this->fname = fname;

    _DELETE(lbout);
    _DELETE(lberr);
    _DELETE(oscout);
    _DELETE(oscerr);

    int fdout = fileno(stdout),
        fderr = fileno(stderr);

    if (!fname.empty()) {
        fdout = open(
                fname.c_str(),
                O_WRONLY|O_CREAT|O_TRUNC,
                S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH
            );
        fderr = fdout;
    }

    lbout = new LogBuffer(fdout);
    lberr = new LogBuffer(fderr);

    oscout = new std::ostream(lbout);
    oscerr = new std::ostream(lberr);
}

void
LogMgr::init() {
    const Options opts = optsmgr.getOptions();

    if (opts.background_given)
        _init(opts.logfile_arg);
    else {
        lbout->setVerbose(!opts.quiet_given);
        lberr->setVerbose(!opts.quiet_given);
    }
}

LogMgr::~LogMgr() {
    _DELETE(oscerr);
    _DELETE(oscout);
    _DELETE(lberr);
    _DELETE(lbout);
}

std::ostream&
LogMgr::cout() const {
    return *oscout;
}

std::ostream&
LogMgr::cerr() const {
    return *oscerr;
}

std::ostream&
LogMgr::cerr(const RuntimeException& except,
             const bool& prepend_newline /*=true*/)
{
    if (prepend_newline)
        *oscerr << "\n";

    *oscerr << "error: " << except.what() << std::endl;

    rc = except.getReturnCode();

    return *oscerr;
}

std::ostream&
LogMgr::cerr(const std::string& what,
             const bool& prepend_newline, /*=true*/
             const bool& prepend_error, /*=true*/
             const bool& append_newline /*=true*/)
{
    if (prepend_newline)
        *oscerr << "\n";

    if (prepend_error)
        *oscerr << "error: ";
        
    *oscerr << what;
    
    if (append_newline)
        *oscerr << std::endl;
    else
        *oscerr << std::flush;

    return *oscerr;
}

const ReturnCode&
LogMgr::getReturnCode() const {
    return rc;
}

void
LogMgr::resetReturnCode() {
    rc = CCLIVE_OK;
}


