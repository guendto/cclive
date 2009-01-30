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
#include <assert.h>
#include <curl/curl.h>

#include "cclive.h"
#include "progress.h"

long /* check remote file length */
cc_getlen (char *xurl, double *len, char **ct) {
    CURLcode rc, httpcode;
    FILE *f;
    int ret;

    assert(xurl != 0);
    assert(len  != 0);
    assert(ct   != 0);

    cc_log("verify video link ...");

    f       = tmpfile();
    ret     = 1;
    *len    = 0;
    *ct     = 0;

    curl_easy_setopt(cc.curl, CURLOPT_URL, xurl);
    curl_easy_setopt(cc.curl, CURLOPT_NOBODY, 1); /* GET => HEAD request */
    curl_easy_setopt(cc.curl, CURLOPT_WRITEDATA, f);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEFUNCTION, 0);

    rc = curl_easy_perform(cc.curl);

    curl_easy_getinfo(cc.curl, CURLINFO_RESPONSE_CODE, &httpcode);

    if (rc == CURLE_OK && httpcode == 200) {
        rc = curl_easy_getinfo(cc.curl,CURLINFO_CONTENT_TYPE, ct);

        if ( (rc == CURLE_OK) && *ct)
            curl_easy_getinfo(cc.curl,CURLINFO_CONTENT_LENGTH_DOWNLOAD,len);

        if ( (rc == CURLE_OK) && *ct && *len) {
            ret = 0;
            cc_log("done.\n");
        } else {
            curl_easy_getinfo(cc.curl,CURLINFO_RESPONSE_CODE,&httpcode);
            cc_log("\nerror: server returned http/%d\n",httpcode);
        }
    } else {
        if (strlen(cc.curl_errmsg))
            cc_log("\nerror: %s\n",cc.curl_errmsg);
        else
            cc_log("\nerror: server returned http/%d\n",httpcode);
    }

    curl_easy_setopt(cc.curl, CURLOPT_HTTPGET, 1); /* reset: HEAD => GET */

    fclose(f);
    return(ret);
}

struct ccget_s { /* used to pass data between cc_get and write_cb */
    double initial;
    char *fn;
    FILE *f;
};

static int /* curl write callback function for file transfers */
write_cb (void *data, size_t size, size_t nmemb, void *stream) {
    struct ccget_s *get = (struct ccget_s *)stream;

    assert(data   != 0);
    assert(stream != 0);

    if (get && !get->f) {
        const char *mode = get->initial ? "ab" : "wb";
        if ( !(get->f = fopen(get->fn,mode)) ) {
            perror(get->fn);
            return(-1);
        }
    }
    return(fwrite(data,size,nmemb,get->f));
}

int /* copy a remote file from url */
cc_get (char *xurl, char *fn, double initial, double total) {
    struct ccprogress_s bp;
    struct ccget_s get;
    CURLcode rc;
    int ret;

    assert(xurl != 0);
    assert(fn   != 0);

    ret = 0;

    memset(&bp,0,sizeof(bp));
    memset(&get,0,sizeof(get));

    if (cc.gi.continue_given && initial > 0) {
        double remaining = total - initial;
        cc_log("from: %.0f (%.2fMB)  remaining: %.0f (%.2fMB)\n",
            initial, ToMB(initial), remaining, ToMB(remaining));
    } else {
        initial = 0;
    }

    cc_bar_init(&bp, initial, total);
    bp.fn       = fn;

    get.initial = initial;
    get.fn      = fn;

    curl_easy_setopt(cc.curl, CURLOPT_URL,              xurl);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEFUNCTION,    write_cb);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEDATA,        &get);
    curl_easy_setopt(cc.curl, CURLOPT_NOPROGRESS,       0);
    curl_easy_setopt(cc.curl, CURLOPT_PROGRESSFUNCTION, &cc_progress_cb);
    curl_easy_setopt(cc.curl, CURLOPT_PROGRESSDATA,     &bp);
    curl_easy_setopt(cc.curl, CURLOPT_ENCODING,         "identity");
    curl_easy_setopt(cc.curl, CURLOPT_HEADER,           0);
    curl_easy_setopt(cc.curl, CURLOPT_RESUME_FROM,      (long)initial);

    if ( (rc = curl_easy_perform(cc.curl)) != CURLE_OK) {
        curl_easy_getinfo(cc.curl, CURLINFO_RESPONSE_CODE, &rc);
        cc_log("\nerror: ");
        if (cc.curl_errmsg)
            cc_log("%s",cc.curl_errmsg);
        else
            cc_log("server closed with http/%d",rc);
        cc_log("\n");
        ret = 1;
    }

    if (get.f) {
        fflush(get.f);
        fclose(get.f);
    }

    if (!ret)
        cc_bar_finish(&bp);

    curl_easy_setopt(cc.curl, CURLOPT_HEADER,       1);
    curl_easy_setopt(cc.curl, CURLOPT_NOPROGRESS,   1);
    curl_easy_setopt(cc.curl, CURLOPT_RESUME_FROM,  0);

    cc_log("\n");
    return(ret);
}

char * /* return video output filename */
cc_getfn (
    double *initial,
    double total,
    char *id,
    char *suffix,
    char *host)
{
    char *p;

    assert(initial  != 0);
    assert(id       != 0);
    assert(suffix   != 0);
    assert(host     != 0);

    p = 0;

    if (!cc.gi.output_video_given) {
        const char dflt[] = "%s-(%s).%s";
        char tmp[PATH_MAX];
        char *n;
        int i;

        snprintf(tmp,sizeof(tmp),dflt,host,id,suffix);

        /* create a temporary output filename if needed by adding a suffix */
        for (i=1; i<INT_MAX; ++i) {
            *initial = cc_file_exists(tmp);
            if (*initial == 0) {
                break;
            } else if (*initial == total) {
                cc_log("error: file is already fully retrieved; "
                        "nothing to do\n");
                return(0);
            } else {
                if (cc.gi.continue_given)
                    break;
            }
            snprintf(tmp,sizeof(tmp),dflt,host,id,suffix);
            asprintf(&n,".%d",i);
            strcat(tmp,n);
            free(n);
        }
        asprintf(&p,tmp);
    }
    else {
        *initial = cc_file_exists(cc.gi.output_video_arg);
        if (*initial == total) {
            cc_log("error: file is already fully retrieved; nothing to do\n");
            return(0);
        }
        asprintf(&p,cc.gi.output_video_arg);
    }
    return(p);
}
