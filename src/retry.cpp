/*
* Copyright (C) 2010 Toni Gundogdu.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#include "quvi.h"
#include "curl.h"
#include "log.h"
#include "opts.h"

static int retries_so_far = 0;

static void
check_counter() {
    const Options opts = optsmgr.getOptions();

    if (++retries_so_far >= opts.retry_arg)
        throw NoMoreRetriesException();

}

static void
retry_msg(const QuviException& x) {
    const Options opts = optsmgr.getOptions();

    std::stringstream b;
    b << "\nerror: "
      << x.what()
      << "\nretry "
      << retries_so_far
      << "/"
      << opts.retry_arg
      << " ... wait "
      << opts.retry_wait_arg
      << "s";

    logmgr.cerr(b.str(), false, false, false);
    sleep(opts.retry_wait_arg);
    logmgr.cerr() << std::endl;
}

void
fetch_page(QuviVideo& qv, const bool& reset_counter=false) {

    if (reset_counter)
        retries_so_far = 0;

    check_counter();

    try {
        qv.parse();
    }
    catch (const QuviException& x) {
        const long httpcode = x.getHttpCode();

        if (httpcode >= 400 && httpcode <= 500)
            throw x;

        retry_msg(x);

        fetch_page(qv);
    }
}

void
fetch_file(QuviVideo& qv, const bool& reset_counter=false) {

    if (reset_counter)
        retries_so_far = 0;

    check_counter();

    try {
        curlmgr.fetchToFile(qv);
    }
    // This is actually a curl error:
    // * We reuse the QuviException class
    catch (const QuviException& x) {
        const long httpcode = x.getHttpCode();

        if (httpcode >= 400 && httpcode <= 500)
            throw x;

        retry_msg(x);

        fetch_file(qv);
    }
}

int
number_of_retries() {
    return retries_so_far;
}


