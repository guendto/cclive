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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#include <assert.h>
#include <curl/curl.h>

#include "cclive.h"
#include "progress.h"

static void
store_fname(const char *fname)
{
    assert(fname != 0);
    if (cc.gi.exec_given) {
        char   *tmp = 0;
        if (asprintf(&tmp, "\"%s\"", fname) > 0)
            llst_append(&cc.fnames, tmp);
        FREE(tmp);
    }
}

/* check remote file length */
static int
query_filelen(const char *xurl, double *len, char **ct)
{
    CURLcode rc, httpcode;
    FILE   *f = tmpfile();
    int     ret = 1;

    assert(xurl != 0);
    assert(len != 0);
    assert(ct != 0);

    if (!f) {
        perror("tmpfile");
        return (1);
    }
    cc_log("verify video link ...");

    curl_easy_setopt(cc.curl, CURLOPT_URL, xurl);
    curl_easy_setopt(cc.curl, CURLOPT_NOBODY, 1);       /* GET => HEAD */
    curl_easy_setopt(cc.curl, CURLOPT_WRITEDATA, f);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEFUNCTION, 0);

    *len = 0;
    *ct = 0;
    rc = curl_easy_perform(cc.curl);

    curl_easy_getinfo(cc.curl, CURLINFO_RESPONSE_CODE, &httpcode);

    if (rc == CURLE_OK && httpcode == 200) {
        rc = curl_easy_getinfo(cc.curl, CURLINFO_CONTENT_TYPE, ct);

        if ((rc == CURLE_OK) && *ct)
            curl_easy_getinfo(cc.curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, len);

        if ((rc == CURLE_OK) && *ct && *len) {
            ret = 0;
            cc_log("done.\n");
        } else {
            curl_easy_getinfo(cc.curl, CURLINFO_RESPONSE_CODE, &httpcode);
            cc_log("\nerror: server returned http/%d\n", httpcode);
        }
    } else {
        if (strlen(cc.curl_errmsg))
            cc_log("\nerror: %s\n", cc.curl_errmsg);
        else
            cc_log("\nerror: server returned http/%d\n", httpcode);
    }

    curl_easy_setopt(cc.curl, CURLOPT_HTTPGET, 1);      /* reset: HEAD => GET */
    fclose(f);

    return (ret);
}

static int      video_num;

/* return video output filename */
static char    *
create_fname(
             double *initial,
             double total,
             const char *id,
             const char *suffix,
             const char *host,
             const char *title)
{
    char   *p = 0;
    int     l;

    assert(initial != 0);
    assert(id != 0);
    assert(suffix != 0);
    assert(host != 0);

    if (!cc.gi.output_video_given) {
        const char *dflt = title ? "%s%s.%s" : "%s%s_%s.%s";
        char    fname[PATH_MAX];
        char    num[8];
        char   *n = 0;
        int     i;

        num[0] = '\0';
        if (cc.gi.number_videos_given)
            snprintf(num, sizeof(num), "%03d_", ++video_num);

        if (title)
            snprintf(fname, sizeof(fname), dflt, num, title, suffix);
        else
            snprintf(fname, sizeof(fname), dflt, num, host, id, suffix);

        /* create a temporary output filename if needed by adding a suffix */
        for (i = 1; i < INT_MAX; ++i) {
            *initial = file_exists(fname);
            if (*initial == 0) {
                break;
            } else if (*initial == total) {
                store_fname(fname);
                cc_log("error: file is already fully retrieved; "
                       "nothing to do\n");
                return (0);
            } else {
                if (cc.gi.continue_given)
                    break;
            }
            if (title)
                snprintf(fname, sizeof(fname), dflt, num, title, suffix);
            else
                snprintf(fname, sizeof(fname), dflt, num, host, id, suffix);
            if (asprintf(&n, ".%d", i) > 0)
                strlcat(fname, n, sizeof(fname));
            FREE(n);
        }
        l = asprintf(&p, "%s", fname);
    } else {
        *initial = file_exists(cc.gi.output_video_arg);
        if (*initial == total) {
            store_fname(cc.gi.output_video_arg);
            cc_log("error: file is already fully retrieved; nothing to do\n");
            return (0);
        }
        l = asprintf(&p, "%s", cc.gi.output_video_arg);
    }
    return (p);
}

/* used to pass data between dl_file and write_cb */
struct getdata_s {
    double          initial;
    char           *fname;
    FILE           *f;
};

/* curl write callback function for file transfers */
static int
write_cb(void *data, size_t size, size_t nmemb, void *stream)
{
    struct getdata_s *get = (struct getdata_s *)stream;

    assert(data != 0);
    assert(stream != 0);

    if (get && !get->f) {
        const char *mode = get->initial ? "ab" : "wb";
        if (!(get->f = fopen(get->fname, mode))) {
            perror(get->fname);
            return (-1);
        }
    }
    return (fwrite(data, size, nmemb, get->f));
}

/* copy a remote file from url */
static int
dl_file(
        const char *xurl,
        const char *fname,
        double initial,
        const double total)
{
    struct progressbar_s bp;
    struct getdata_s get;
    curl_off_t r;
    CURLcode rc;
    int     ret = 0;

    assert(xurl != 0);
    assert(fname != 0);

    memset(&bp, 0, sizeof(bp));
    memset(&get, 0, sizeof(get));

    if (cc.gi.continue_given && initial > 0) {
        double  remaining = total - initial;
        cc_log("from: %.0f (%.1fM)  remaining: %.0f (%.1fM)\n",
               initial, ToMB(initial), remaining, ToMB(remaining));
    } else {
        initial = 0;
    }

    bar_init(&bp, initial, total);
    bp.fname = (char *)fname;

    get.initial = initial;
    get.fname = (char *)fname;

    curl_easy_setopt(cc.curl, CURLOPT_URL, xurl);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEDATA, &get);
    curl_easy_setopt(cc.curl, CURLOPT_NOPROGRESS, 0);
    curl_easy_setopt(cc.curl, CURLOPT_PROGRESSFUNCTION, &progress_cb);
    curl_easy_setopt(cc.curl, CURLOPT_PROGRESSDATA, &bp);
    curl_easy_setopt(cc.curl, CURLOPT_ENCODING, "identity");
    curl_easy_setopt(cc.curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(cc.curl, CURLOPT_RESUME_FROM, (long)initial);

    r = (curl_off_t) cc.gi.limit_rate_arg * 1024;
    curl_easy_setopt(cc.curl, CURLOPT_MAX_RECV_SPEED_LARGE, r);

    if ((rc = curl_easy_perform(cc.curl)) != CURLE_OK) {
        curl_easy_getinfo(cc.curl, CURLINFO_RESPONSE_CODE, &rc);
        cc_log("\nerror: ");
        if (cc.curl_errmsg)
            cc_log("%s", cc.curl_errmsg);
        else
            cc_log("server closed with http/%d", rc);
        cc_log("\n");
        ret = 1;
    }
    if (get.f) {
        fflush(get.f);
        fclose(get.f);
    }
    if (!ret)
        bar_finish(&bp);

    curl_easy_setopt(cc.curl, CURLOPT_HEADER, 1);
    curl_easy_setopt(cc.curl, CURLOPT_NOPROGRESS, 1);
    curl_easy_setopt(cc.curl, CURLOPT_RESUME_FROM, 0);
    curl_easy_setopt(cc.curl, CURLOPT_MAX_RECV_SPEED_LARGE, 0);

    cc_log("\n");
    return (ret);
}

/* prepare video file for extraction */
int
prep_video(
           const char *xurl,
           const char *id,
           const char *host,
           const char *title
)
{
    double  len = 0, initial = 0;
    char   *fname = 0, *ct = 0;
    int     rc = 1;

    assert(xurl != 0);
    assert(id != 0);
    assert(host != 0);

    rc = query_filelen(xurl, &len, &ct);
    if (!rc) {
        const char *_title = (title != 0 && strlen(title) > 0) ? title : 0;
        fname =
            create_fname(&initial, len, id, cc.gi.download_arg, host, _title);
        if (fname) {
            const char file[] = "file: %s  %.1fM  [%s]\n";
            rc = 0;
            if (cc.gi.no_extract_given)
                cc_log(file, fname, ToMB(len), ct);
            else if (cc.gi.emit_csv_given)
                fprintf(stdout, "csv:\"%s\",\"%.0f\",\"%.0f\",\"%s\"\n",
                        fname, len, initial, xurl);
            else {
                if (cc.gi.print_fname_given)
                    cc_log(file, fname, ToMB(len), ct);
                rc = dl_file(xurl, fname, initial, len);
                if (!rc)
                    store_fname(fname);
            }
            FREE(fname);
        }
    }
    return (rc);
}
