/*
 * Copyright (C) 2009,2010 Toni Gundogdu.
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

#ifndef except_h
#define except_h

#include "error.h"

class RuntimeException {
public:
    RuntimeException();
    RuntimeException(const ReturnCode& rc);
    RuntimeException(const ReturnCode& rc, const std::string& error);
    virtual ~RuntimeException();
public:
    const std::string   what()  const;
    const ReturnCode&   getReturnCode() const;
private:
    ReturnCode rc;
    std::string error;
};

class ParseException : public RuntimeException {
public:
    ParseException(const std::string&);
};

class NoSupportException : public RuntimeException {
public:
    NoSupportException(const std::string&);
};

class NothingToDoException : public RuntimeException {
public:
    NothingToDoException();
};

class FileOpenException : public RuntimeException {
public:
    FileOpenException(const std::string&);
};

class QuviException : public RuntimeException {
public:
    QuviException(const std::string&);
    QuviException(const std::string&, const long &httpcode);
    const long& getHttpCode() const;
    const long& getCurlCode() const;
private:
    long httpcode;
    long curlcode;
};

class QuviNoVideoLinkException : public RuntimeException {
public:
    QuviNoVideoLinkException();
};

class NoMoreRetriesException : public RuntimeException {
public:
    NoMoreRetriesException();
};

#endif


