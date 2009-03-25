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

#define _GNU_SOURCE

#include "config.h"

#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
  #include <string.h>
#endif
#include <assert.h>
#include <curl/curl.h>

#include "cclive.h"

static int /* page handler for youtube */
handle_youtube (mem_t page) {
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
        char *title = 0;

        snprintf(xurl,sizeof(xurl),
            "http://youtube.com/get_video?video_id=%s&t=%s",id,t);

        if (!strcmp(cc.gi.download_arg,"mp4"))
            strlcat(xurl,"&fmt=18",sizeof(xurl));
        else if(!strcmp(cc.gi.download_arg,"3gpp"))
            strlcat(xurl,"&fmt=17",sizeof(xurl));
        else if(!strcmp(cc.gi.download_arg,"xflv"))
            strlcat(xurl,"&fmt=6",sizeof(xurl));

#ifdef WITH_PERL
        title = page_title(page->p);
#endif
        rc = prep_video(xurl,id,"youtube",title);
        FREE(title);
    }

    FREE(id);
    FREE(t);
    FREE(page->p);

    return(rc);
}

static int /* page handler for break.com */
handle_break (mem_t page) {
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
        char *title = 0;
        int n;

        n = asprintf(&xurl,
            "http://media1.break.com/dnet/media/%s/%s.flv",fpath,fname);
#ifdef WITH_PERL
        title = page_title(page->p);
#endif
        rc = prep_video(xurl,id,"break",title);
        FREE(title);
    }

    FREE(id);
    FREE(fpath);
    FREE(fname);
    FREE(xurl);
    FREE(page->p);

    return(rc);
}

static int /* page handler for google video */
handle_gvideo (mem_t page) {
    char *id=0,*xurl=0;
    int rc=1;

    const char id_begin[]   = "docid: '";
    const char id_end[]     = "'";

    const char flv_begin[]  = "videoUrl\\x3d";
    const char flv_end[]    = "\\x26";

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
                char *p = 0;
                if (asprintf(&p,"http://vp.%s",mp4) > 0)
                    xurl = curl_easy_unescape(cc.curl,p,0,0);
                FREE(p);
            } else {
                cc_log("error: mp4 format unavailable\n");
            }
            FREE(mp4);
        } else {
            char *flv = strsub(page->p, flv_begin, flv_end);
            if (flv)
                xurl = curl_easy_unescape(cc.curl,flv,0,0);
            FREE(flv);
        }
    }

    if (xurl) {
        char *title = 0;
#ifdef WITH_PERL
        title = page_title(page->p);
#endif
        rc = prep_video(xurl,id,"gvideo",title);
        FREE(title);
    }

    curl_free(xurl);
    FREE(page->p);
    FREE(id);

    return(rc);
}

static int /* page handler for evisor.tv */
handle_evisor (mem_t page) {
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
        char *title = 0;
        xurl = strsub(page->p, xurl_begin, xurl_end);
        if (xurl) {
#ifdef WITH_PERL
            title = page_title(page->p);
#endif
            rc = prep_video(xurl,id,"evisor",title);
            FREE(title);
        }
        FREE(xurl);
        FREE(id);
    }

    FREE(page->p);
    return(rc);
}

static int /* page handler for sevenload.com */
handle_7load (mem_t page) {
/*
 * 1) parse page html for config url
 * 2) parse config for video link
 */
    char *config=0,*title=0;
    int rc=1;

    const char config_begin[]   = "configPath=";
    const char config_end[]     = "\"";

    const char id_begin[]       = "item id=\"";
    const char id_end[]         = "\"";

    const char xurl_begin[]     = "<location seeking=\"yes\">";
    const char xurl_end[]       = "</location>";

    assert(page         != 0);
    assert(page->p      != 0);
    assert(page->size   > 0);

    config = strsub(page->p, config_begin, config_end);
#ifdef WITH_PERL
    title = page_title(page->p);
#endif
    FREE(page->p);

    if (config) {
        char *tmp = curl_easy_unescape(cc.curl,config,0,0);
        if (tmp) {
            cc_log("fetch config xml ...");
            rc = fetch_link(tmp,page,0);
            if (!rc) {
                char *id = strsub(page->p, id_begin, id_end);
                if (id) {
                    char *xurl = strsub(page->p, xurl_begin, xurl_end);
                    if (xurl)
                        rc = prep_video(xurl,id,"7load",title);
                    FREE(xurl);
                }
                FREE(id);
            }
            FREE(page->p);
            curl_free(tmp);
        }
        FREE(config);
    }
    FREE(title);
    return(rc);
}

static int /* page handler for liveleak.com */
handle_lleak (mem_t page) {
/*
 * 1) parse page html for config url
 * 2) fetch config and parse it for playlist url
 * 3) parse playlist for video link
 */
    char *id=0,*config=0,*title=0;
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
    if (id) {
#ifdef WITH_PERL
        title = page_title(page->p);
#endif
        config = strsub(page->p, config_begin, config_end);
    }
    FREE(page->p);

    if (id && config) {
        char *tmp = curl_easy_unescape(cc.curl, config, 0, 0);
        cc_log("fetch xml config ...");
        if (tmp) {
            rc = fetch_link(tmp,page,0);
            curl_free(tmp);
            if (!rc) {
                char *pl = strsub(page->p, pl_begin, pl_end);
                FREE(page->p);
                if (pl) {
                    cc_log("fetch playlist ...");
                    tmp = curl_easy_unescape(cc.curl, pl, 0, 0);
                    rc = fetch_link(tmp,page,0);
                    curl_free(tmp);
                    if (!rc) {
                        char *xurl = strsub(page->p, xurl_begin, xurl_end);
                        if (xurl)
                            rc = prep_video(xurl,id,"lleak",title);
                        FREE(xurl);
                    }
                    FREE(page->p);
                }
                FREE(pl);
            }
        }
    }
    FREE(id);
    FREE(config);
    FREE(title);

    return(rc);
}

static int
handle_dmotion (mem_t page) {
    char *id=0,*xurl=0;
    int rc=1;

    const char id_begin[] = "swf%2F";
    const char id_end[]   = "\"";

    const char paths_begin[]= "\"video\", \"";
    const char paths_end[]  = "\"";

    assert(page         != 0);
    assert(page->p      != 0);
    assert(page->size   > 0);

    id = strsub(page->p, id_begin, id_end);
    if (id) {
        char *tmp=strsub(page->p, paths_begin, paths_end);
        char *paths=0;

        if (tmp)
            paths = curl_easy_unescape(cc.curl,tmp,0,0);

        if (paths) {
            const char sep[]="||";
            char *tok=strtok(paths,sep);
            while (tok) {
                char *type = strstr(tok,"@@");
                if (type) {
                    int n;
                    type += 2;
                    if (!strcmp(type,"spark"))
                        n = asprintf(&xurl,"http://dailymotion.com%s",tok);
                    if (!strcmp(type,cc.gi.download_arg)) {
                        FREE(xurl);
                        n = asprintf(&xurl,"http://dailymotion.com%s",tok);
                        break;
                    }
                }
                tok = strtok(0,sep);
            }
        }
        curl_free(paths);
        FREE(tmp);

        if (xurl) {
            char *title = 0;
            char *p = strstr(xurl,"@@");
            if (p)
                xurl[p-xurl] = '\0';
#ifdef WITH_PERL
            title = page_title(page->p);
#endif
            rc = prep_video(xurl,id,"dmotion",title);
            FREE(title);
        }
        FREE(xurl);
    }
    FREE(id);
    FREE(page->p);

    return(rc);
}

static char * /* convert embed link to video page link */
embed2video (const char *url) {
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

static char * /* convert lastfm-youtube link to youtube link */
lastfm2youtube (const char *url) {
    static const char lastfm_find[] = "+1-";
    char *p = strstr(url,lastfm_find);
    if (strstr(url,"last.fm") && p) {
        char id[16];
        char *_url = 0;

        p += strlen(lastfm_find);
        strlcpy(id,p,sizeof(id));
        if (asprintf(&_url,"http://youtube.com/watch?v=%s",id) > 0)
            return(_url);
    }
    return(0);
}

typedef int (*handlerfp)(mem_t);
struct host_s {
    char *lookup;
    handlerfp fp;
};

static const struct host_s hosts[] = {
    {"youtube.com",     handle_youtube},
    {"break.com",       handle_break},
    {"video.google.",   handle_gvideo},
    {"evisor.tv",       handle_evisor},
    {"sevenload.com",   handle_7load},
    {"liveleak.com",    handle_lleak},
    {"dailymotion.com", handle_dmotion},
};

void /* --supported-hosts */
list_hosts (void) {
    const int c = sizeof(hosts)/sizeof(hosts[0]);
    int i;
    for (i=0; i<c; ++i)
        cc_log("%s\n",hosts[i].lookup);
}

int /* handle host */
handle_host (const char *url) {
    struct _mem_s page;
    char *_url=0;
    int i,c=sizeof(hosts)/sizeof(hosts[0]);

    assert(url != 0);

    if ((_url = embed2video(url)) != 0)
        url = _url;
    else if ((_url = lastfm2youtube(url)) != 0)
        url = _url;

    for (i=0; i<c; ++i) {
        memset(&page,0,sizeof(page));
        if (strstr(url,hosts[i].lookup) != 0) {
            int rc = fetch_link(url,&page,1);
            FREE(_url); /* points to _url if any conv. was done above */
            return (rc == 0
                ? (hosts[i].fp)(&page)
                : 1);
        }
    }
    cc_log("error: no support: %s\n",url);
    return(1);
}
