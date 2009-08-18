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

#include <string>
#include <sstream>
#include <climits>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HOST_W32
#include <windows.h>
#define sleep(n) Sleep(n*1000)
#endif

#include "except.h"
#include "video.h"
#include "curl.h"
#include "log.h"
#include "opts.h"
#include "retry.h"

RetryMgr::RetryMgr()
    : retries(0), retryUntilRetrievedFlag(false)
{
}

void
RetryMgr::reset() {
    retries = 0;
    retryUntilRetrievedFlag = false;
}

void
RetryMgr::handle(const CurlMgr::FetchException& x) {

    logmgr.cerr(x);

    switch ( x.getHTTPCode() ) {

    case 403: // Forbidden
    case 404: // Not found
        throw x;

    default:
        const Options opts =
            optsmgr.getOptions();

        int retry = opts.retry_arg;

        if (retryUntilRetrievedFlag)
            retry = 0; // Overrides --retry setting if we are downloading a file

        if (++retries <= opts.retry_arg || retry == 0) {
            std::stringstream b;

            b << "retry #"
              << retries
              << " ... wait "
              << opts.retry_wait_arg
              << " second(s).";

            logmgr.cerr(b.str(), false, false, false);

            sleep(opts.retry_wait_arg);

            logmgr.cerr() << std::endl;

            if (retries == INT_MAX-1)
                retries = 0;
        }
        else
            throw x;
    }
}

void
RetryMgr::setRetryUntilRetrievedFlag() {
    retryUntilRetrievedFlag = true;
}

const bool&
RetryMgr::getRetryUntilRetrievedFlag() const {
    return retryUntilRetrievedFlag;
}
