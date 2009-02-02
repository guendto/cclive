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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <curl/curl.h>

#include "mem.h"
#include "cclive.h"

char * /* extract substring */
strsub (const char *s, const char *from, const char *to) {
    char *pf,*pt,*p;
    size_t l;
    const char e[] = "error: '%s' not found\n";

    assert(s    != 0);
    assert(from != 0);
    assert(to   != 0);

    pf = strstr(s,from);
    if (!pf) return((char*)cc_log(e,from));
    pf += strlen(from);

    pt = strstr(pf,to);
    if (!pt) return((char*)cc_log(e,to));
    l = pt-pf;

    if ( !(p=malloc(l+1)) ) {
        perror("malloc");
        return(0);
    }
    p[0] = '\0';

    strncat(p,pf,l);
    return(p);
}

char * /* replace substring */
strrepl (const char *s, const char *what, const char *with) {
/* http://members.dodo.com.au/~netocrat/c/source/replacebench.c
 * the optimized version. no copyright claimed. with minor changes. */
    int rl,wl,retlen,l;
    const char *p,*q;
    char *ret,*r;

    assert(s    != 0);
    assert(what != 0);
    assert(with != 0);

    rl = strlen(what);
    wl = strlen(with);

    if (rl != wl) {
        int c;
        for(c=0,p=s;
            (q=strstr(p,what)) != 0;
            p=q+rl,c++);
        retlen = p-s+strlen(p)+c*(wl-rl);
    }
    else
        retlen = strlen(s);

    if ( !(ret=malloc(retlen+1))) {
        perror("malloc");
        return(0);
    }

    for (r=ret,p=s; (q=strstr(p,what)) != 0; p=q+rl) {
        l = q-p;
        memcpy(r,p,l);
        r += l;
        memcpy(r,with,wl);
        r += wl;
    }

    strcpy(r,p);
    return(ret);
}

double /* check if file exists; zero or file length */
file_exists (char *path) {
    double len=0;
    FILE *f;

    assert(path != 0);

    if ( (f=fopen(path,"r")) ) {
        fseek(f,0,SEEK_END);
        len = (double)ftell(f);
        fclose(f);
    }
    return(len);
}

int /* fetch data from url */
fetch_link (char *url, struct cc_mem_s *page, int log) {
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
