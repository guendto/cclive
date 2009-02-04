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
#include <string.h>
#include <assert.h>
#include <curl/curl.h>

#include "cclive.h"
#include "mem.h"

static int /* page handler for youtube */
handle_youtube (struct cc_mem_s *page) {
    char *id=0,*t=0;
    int rc=1;

    const char id_begin[] = "\"video_id\": \"";
    const char id_end[]   = "\"";

    const char t_begin[]  = "\"t\": \"";
    const char t_end[]    = "\"";

    assert(page         != 0);
    assert(page->p      != 0);
    assert(page->size   > 0);

    id = strsub(page->p, id_begin, id_end);
    if (id)
        t = strsub(page->p, t_begin, t_end);

    if (id && t) {
        char xurl[128];

        snprintf(xurl,sizeof(xurl),
            "http://youtube.com/get_video?video_id=%s&t=%s",id,t);

        if (!strcmp(cc.gi.download_arg,"mp4"))
            strcat(xurl,"&fmt=18");
        else if(!strcmp(cc.gi.download_arg,"3gpp"))
            strcat(xurl,"&fmt=17");
        else if(!strcmp(cc.gi.download_arg,"xflv"))
            strcat(xurl,"&fmt=6");

        rc = prep_video(xurl,id,"youtube");
    }

    free(id);
    free(t);
    free(page->p);

    return(rc);
}

static int /* page handler for break.com */
handle_break (struct cc_mem_s *page) {
    char *id=0,*fpath=0,*fname=0,*xurl=0;
    int rc=1;

    const char id_begin[]   = "ContentID='";
    const char id_end[]     = "'";

    const char fpath_begin[]= "ContentFilePath='";
    const char fpath_end[]  = "'";

    const char fn_begin[]   = "FileName='";
    const char fn_end[]     = "'";

    assert(page         != 0);
    assert(page->p      != 0);
    assert(page->size   > 0);

    id = strsub(page->p, id_begin, id_end);
    if (id) {
        fpath = strsub(page->p, fpath_begin, fpath_end);
        if (fpath)
            fname = strsub(page->p, fn_begin, fn_end);
    }

    if (id && fpath && fname) {
        asprintf(&xurl,
            "http://media1.break.com/dnet/media/%s/%s.%s",fpath,fname,
            (!strcmp(cc.gi.download_arg,"wmv")) ? "wmv":"flv");
        rc = prep_video(xurl,id,"break");
    }

    free(id);
    free(fpath);
    free(fname);
    free(xurl);
    free(page->p);

    return(rc);
}

static int /* page handler for google video */
handle_gvideo (struct cc_mem_s *page) {
    char *id=0,*xurl=0;
    int rc=1;

    const char id_begin[]   = "docid: '";
    const char id_end[]     = "'";

    const char flv_begin[]  = "videoUrl\\x3d";
    const char flv_end[]    = "\\x26hl";

    const char mp4_begin[]  = "href=\"http://vp.";
    const char mp4_end[]    = "\"";

    assert(page         != 0);
    assert(page->p      != 0);
    assert(page->size   > 0);

    id = strsub(page->p, id_begin, id_end);
    if (id) {
        if (!strcmp(cc.gi.download_arg,"mp4")) {
            char *mp4 = strsub(page->p, mp4_begin, mp4_end);
            if (mp4) {
                char *p;
                asprintf(&p,"http://vp.%s",mp4);
                xurl = curl_easy_unescape(cc.curl,p,0,0);
                free(p);
            } else {
                cc_log("error: mp4 format unavailable\n");
            }
            free(mp4);
        } else {
            char *flv = strsub(page->p, flv_begin, flv_end);
            if (flv)
                xurl = curl_easy_unescape(cc.curl,flv,0,0);
            free(flv);
        }
    }

    if (xurl)
        rc = prep_video(xurl,id,"gvideo");

    curl_free(xurl);
    free(page->p);
    free(id);

    return(rc);
}

static int /* page handler for evisor.tv */
handle_evisor (struct cc_mem_s *page) {
    char *id=0,*xurl=0;
    int rc=1;

    const char id_begin[]   = "unit_long";
    const char id_end[]     = "\"";

    const char xurl_begin[] = "file=";
    const char xurl_end[]   = "\"";

    assert(page         != 0);
    assert(page->p      != 0);
    assert(page->size   > 0);

    id = strsub(page->p, id_begin, id_end);
    if (id) {
        xurl = strsub(page->p, xurl_begin, xurl_end);
        if (xurl)
            rc = prep_video(xurl,id,"evisor");
        free(xurl);
        free(id);
    }

    free(page->p);
    return(rc);
}

static int /* page handler for sevenload.com */
handle_7load (struct cc_mem_s *page) {
/*
 * 1) parse page html for config url
 * 2) parse config for video link
 */
    char *id=0,*config=0,*xurl=0,*p=0;
    int rc=1;

    const char config_begin[]   = "configPath=";
    const char config_end[]     = "\"";

    const char id_begin[]       = "item id=\"";
    const char id_end[]         = "\"";

    const char xurl_begin[]     = "video url=\"";
    const char xurl_end[]       = "\"";

    assert(page         != 0);
    assert(page->p      != 0);
    assert(page->size   > 0);

    config = strsub(page->p, config_begin, config_end);
    free(page->p);

    if (!config)
        return(0);

    cc_log("fetch config xml ...");

    p  = curl_easy_unescape(cc.curl,config,0,0);
    free(config);

    rc = fetch_link(p,page,0);
    curl_free(p);

    if (rc)
        return(0);

    id = strsub(page->p, id_begin, id_end);
    if (id) {
        xurl = strsub(page->p, xurl_begin, xurl_end);
        if (xurl)
            rc = prep_video(xurl,id,"7load");
        free(xurl);
        free(id);
    }

    free(page->p);
    return(rc);
}

static int /* page handler for liveleak.com */
handle_lleak (struct cc_mem_s *page) {
/*
 * 1) parse page html for config url
 * 2) fetch config and parse it for playlist url
 * 3) parse playlist for video link
 */
    char *id=0,*config=0,*pl=0,*xurl=0,*p=0;
    int rc=1;

    const char id_begin[]       = "token=";
    const char id_end[]         = "&";

    const char config_begin[]   = "'config','";
    const char config_end[]     = "'";

    const char pl_begin[]       = "<file>"; /* pl = playlist */
    const char pl_end[]         = "</file>";

    const char xurl_begin[]     = "<location>";
    const char xurl_end[]       = "</location>";

    assert(page         != 0);
    assert(page->p      != 0);
    assert(page->size   > 0);

    id = strsub(page->p, id_begin, id_end);
    if (id)
        config = strsub(page->p, config_begin, config_end);
    free(page->p);

    if (!id || !config) {
        free(config);
        free(id);
        return(0);
    }

    cc_log("fetch xml config ...");

    p  = curl_easy_unescape(cc.curl,config,0,0);
    free(config);

    rc = fetch_link(p,page,0);
    curl_free(p);

    if (rc)
        return(0);

    pl = strsub(page->p, pl_begin, pl_end);
    free(page->p);

    if (rc)
        return(rc);

    cc_log("fetch playlist ...");

    p  = curl_easy_unescape(cc.curl,pl,0,0);
    free(pl);

    rc = fetch_link(p,page,0);
    curl_free(p);

    if (rc)
        return(rc);

    xurl = strsub(page->p, xurl_begin, xurl_end);
    free(page->p);

    rc = prep_video(xurl,id,"lleak");
    free(xurl);
    free(id);

    return(rc);
}

static char * /* convert embed link to video page link */
embed2video(char *url) {
    struct lookup_s {
        char *what;
        char *with;
    };
    const struct lookup_s lookup[] = {
        {"/v/",             "/watch?v="}, /* youtube */
        {"googleplayer.swf","videoplay"}, /* gvideo */
        {"/pl/",            "/videos/"},  /* 7load */
        {"/e/",             "/view?i="},  /* lleak */
    };
    char *p=0;
    int i,c=sizeof(lookup)/sizeof(lookup[0]);

    for (i=0; i<c; ++i) {
        if (strstr(url,lookup[i].what)) {
            p = strrepl(url, lookup[i].what, lookup[i].with);
            break;
        }
    }
    return(p);
}

typedef int (*handlerfunc)(struct cc_mem_s *);

struct host_s {
    char *lookup;
    handlerfunc fp;
};

static const struct host_s hosts[] = {
    {"youtube.com",     handle_youtube},
    {"break.com",       handle_break},
    {"video.google.",   handle_gvideo},
    {"evisor.tv",       handle_evisor},
    {"sevenload.com",   handle_7load},
    {"liveleak.com",    handle_lleak},
};

void /* --supported-hosts */
list_hosts (void) {
    const int c = sizeof(hosts)/sizeof(hosts[0]);
    int i;
    for (i=0; i<c; ++i)
        cc_log("%s\n",hosts[i].lookup);
}

int /* handle host */
handle_host (char *url) {
    struct cc_mem_s page;
    char *_url=0;
    int i,c=sizeof(hosts)/sizeof(hosts[0]);

    assert(url != 0);

    if ((_url=embed2video(url)) != 0)
        url = _url;

    for (i=0; i<c; ++i) {
        memset(&page,0,sizeof(page));
        if (strstr(url,hosts[i].lookup) != 0) {
            int rc = fetch_link(url,&page,1);
            free(_url); /* url points to _url if embed => page conv. was made */
            if (!rc)
                return((hosts[i].fp)(&page));
            else
                return(1);
        }
    }
    cc_log("error: no support: %s\n",url);
    return(1);
}
