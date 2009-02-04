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
#include <stdlib.h>
#include <memory.h>
#include <pwd.h>
#include <unistd.h>
#include <curl/curl.h>

#include "cclive.h"

int /* log into youtube */
login_youtube (void) {
    struct _mem_s data;
    char *pass=0,*req=0;
    CURLcode rc;
    int ret=1;

    memset(&data,0,sizeof(data));

    if (!cc.gi.youtube_pass_given) {
        char *prompt=0,*p=0;
        asprintf(&prompt,"enter login password for %s:",
            cc.gi.youtube_user_arg);
        p = getpass(prompt);
        FREE(prompt);
        pass = strdup(p);
        bzero(p,strlen(pass));
    }

    asprintf(&req,
        "http://uk.youtube.com/login?current_form=loginform"
        "&username=%s&password=%s&action_login=log+in&hl=en-GB",
        cc.gi.youtube_user_arg, pass ? pass:cc.gi.youtube_pass_arg);

    FREE(pass);

    curl_easy_setopt(cc.curl, CURLOPT_URL,              req);
    curl_easy_setopt(cc.curl, CURLOPT_COOKIEFILE,       "");
    curl_easy_setopt(cc.curl, CURLOPT_ENCODING,         "");
    curl_easy_setopt(cc.curl, CURLOPT_WRITEFUNCTION,    writemem_cb);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEDATA,        &data);

    cc_log("[youtube] attempt to login as %s ...",
        cc.gi.youtube_user_arg);

    rc = curl_easy_perform(cc.curl);
    FREE(req);

    if (rc == CURLE_OK) {
        CURLcode httpcode;
        curl_easy_getinfo(cc.curl, CURLINFO_RESPONSE_CODE, &httpcode);
        if (httpcode == 200) {
            if (strstr(data.p,"your log-in was incorrect") != 0) {
                cc_log("\nerror: login was incorrect\n");
            } else if (strstr(data.p,"check your password") != 0) {
                cc_log("\nerror: check your login password\n");
            } else if (strstr(data.p,"too many login failures") != 0) {
                cc_log("\nerror: too many login failures, try later");
            } else {
                curl_easy_setopt(cc.curl, CURLOPT_COOKIE, "is_adult=1");
                cc_log("done.\n");
                ret = 0;
            }
        } else {
            cc_log("\nerror: server returned http:/%d",httpcode);
        }
    }

    FREE(data.p);
    return(ret);
}
