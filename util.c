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

#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdarg.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <assert.h>
#include <curl/curl.h>

#ifndef HAVE_MALLOC
#error Cannot compile without malloc() support
#endif

#include "cclive.h"

/* extract substring */
char           *
strsub(const char *s, const char *from, const char *to)
{
    const char e[] = "error: '%s' not found\n";
    char   *pf, *pt, *p;
    size_t  l;

    assert(s != 0);
    assert(from != 0);
    assert(to != 0);

    pf = strstr(s, from);
    if (!pf)
        return ((char *)cc_log(e, from));
    pf += strlen(from);

    pt = strstr(pf, to);
    if (!pt)
        return ((char *)cc_log(e, to));
    l = pt - pf;

    if (!(p = malloc(l + 1))) {
        perror("malloc");
        return (0);
    }
    p[0] = '\0';

    strncat(p, pf, l);
    return (p);
}

/* replace substring */
char           *
strrepl(const char *s, const char *what, const char *with)
{
    /*
     * http://members.dodo.com.au/~netocrat/c/source/replacebench.c the
     * optimized version. no copyright claimed. with minor changes.
     */
    int     rl, wl, retlen, l;
    const char *p, *q;
    char   *ret, *r;

    assert(s != 0);
    assert(what != 0);
    assert(with != 0);

    rl = strlen(what);
    wl = strlen(with);

    if (rl != wl) {
        int     c;
        for (c = 0, p = s;
             (q = strstr(p, what)) != 0;
             p = q + rl, c++);
        retlen = p - s + strlen(p) + c * (wl - rl);
    } else
        retlen = strlen(s);

    if (!(ret = malloc(retlen + 1))) {
        perror("malloc");
        return (0);
    }
    for (r = ret, p = s; (q = strstr(p, what)) != 0; p = q + rl) {
        l = q - p;
        memcpy(r, p, l);
        r += l;
        memcpy(r, with, wl);
        r += wl;
    }

    strcpy(r, p);
    return (ret);
}

void
strreplch(char *s, const char c, const char with) {
    char *p = s;

    assert(s != 0);

    while (*p != '\0') {
        if (*p == c)
            *p = with;
        *p++;
    }
}

/* remove all occurences of c in a string */
void
strrmch(char *s, const char c)
{
    char   *s1 = s, *s2 = s;

    assert(s != 0);

    while (*s1 != '\0') {
        if (*s1 == c)
            ++s1;
        else
            *s2++ = *s1++;
    }
    *s2 = '\0';
}

/* check if file exists; zero or file length */
double
file_exists(const char *path)
{
    double  len = 0;
    FILE   *f = 0;

    assert(path != 0);

    if ((f = fopen(path, "r"))) {
        fseek(f, 0, SEEK_END);
        len = (double)ftell(f);
        fclose(f);
    }
    return (len);
}

/* fetch data from url */
int
fetch_link(const char *url, mem_t page, const int log_fetch)
{
    CURLcode rc;
    int     ret = 1;

    assert(url != 0);
    assert(page != 0);

    page->p = 0;
    page->size = 0;

    if (log_fetch)
        cc_log("fetch %s ...", url);

    curl_easy_setopt(cc.curl, CURLOPT_URL, url);
    curl_easy_setopt(cc.curl, CURLOPT_ENCODING, "");
    curl_easy_setopt(cc.curl, CURLOPT_WRITEFUNCTION, writemem_cb);
    curl_easy_setopt(cc.curl, CURLOPT_WRITEDATA, page);

    if ((rc = curl_easy_perform(cc.curl)) == CURLE_OK) {
        long    httpcode;
        curl_easy_getinfo(cc.curl, CURLINFO_RESPONSE_CODE, &httpcode);
        if (httpcode == 200) {
            cc_log("done.\n");
            ret = 0;
        } else
            cc_log("\nerror: server returned http/%d\n", httpcode);
    } else {
        cc_log("\nerror: %s\n", cc.curl_errmsg);
    }
    return (ret);
}

/* general purpose log function; notice stderr */
int
cc_log(const char *fmt,...)
{
    va_list args;
    char   *p = 0;
    int     n;

    assert(fmt != 0);

    if (cc.gi.quiet_given)
        return (0);

    va_start(args, fmt);
    n = vasprintf(&p, fmt, args);
    va_end(args);

    if (n > 0 && p != 0)
        fprintf(stderr, "%s", p);
    FREE(p);

    return (0);
}
