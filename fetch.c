/*
 * cclive Copyright (C) 2009 Toni Gundogdu.
 * This file is part of cclive.
 *
 * cclive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cclive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <curl/curl.h>

#include "mem.h"
#include "cclive.h"

int /* fetch video page */
cc_fetch (char *url, struct cc_mem_s *page, int log) {
    CURLcode rc;
    int ret;

    assert(url  != 0);
    assert(page != 0);

    ret         = 1;
    page->p     = 0;
    page->size  = 0;

    if (log)
        cc_log("fetch %s ...",url);

    curl_easy_setopt(cc.curl, CURLOPT_URL,           url);
    curl_easy_setopt(cc.curl, CURLOPT_ENCODING,      "");
    curl_easy_setopt(cc.curl, CURLOPT_WRITEFUNCTION, cc_writemem_cb);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEDATA,     page);

    if ( (rc = curl_easy_perform(cc.curl)) == CURLE_OK) {
        CURLcode httpcode;
        curl_easy_getinfo(cc.curl,CURLINFO_RESPONSE_CODE,&httpcode);
        if (httpcode == 200) {
            cc_log("done.\n");
            ret = 0;
        }
        else
            cc_log("\nerror: server returned http/%d\n",httpcode);
    } else {
        cc_log("\nerror: server returned %s\n",cc.curl_errmsg);
    }
    return(ret);
}
