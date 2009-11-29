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

#include <string>

#include "except.h"

RuntimeException::RuntimeException()
    : rc(CCLIVE_OK), error("")
{
}

RuntimeException::RuntimeException(const ReturnCode& rc)
    : rc(rc), error("")
{
}

RuntimeException::RuntimeException(
    const ReturnCode& rc,
    const std::string& error)
    : rc(rc), error(error)
{
}

RuntimeException::~RuntimeException() {
}

const std::string
RuntimeException::what() const {
    static const char
    errorStrings[_CCLIVE_MAX_RETURNCODES][48] = {
        "no error",
        "(reserved)", // gengetopt uses this (1)
        "invalid option argument",
        "curl_easy_init returned null",
        "file already fully retrieved; nothing to do",
        "system", // CCLIVE_SYSTEM
        "no support",
        "network",
        "fetch",
        "parse",
        "internal",
    };

    ReturnCode _rc = rc;
    if (_rc >= _CCLIVE_MAX_RETURNCODES)
        _rc = CCLIVE_INTERNAL;

    std::string msg = errorStrings[_rc];

    if (error.length() > 0) {
        if (msg.length() > 0)
            msg += ": ";

        msg += error;

        if (_rc == CCLIVE_INTERNAL)
            msg += ": " + static_cast<int>(rc);
    }
    return msg;
}

const ReturnCode&
RuntimeException::getReturnCode() const {
    return rc;
}

FileOpenException::FileOpenException(const std::string& err)
    : RuntimeException(CCLIVE_SYSTEM, err)
{
}


