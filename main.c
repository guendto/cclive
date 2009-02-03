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
#include <curl/curl.h>

#include "cclive.h"

struct cclive_s cc;

static void /* init curl handle which will be reused */
init_curl (void) {
    if ( !(cc.curl = curl_easy_init()) ) {
        fprintf(stderr,"error: curl_easy_init returned null\n");
        exit(EXIT_FAILURE);
    }
    curl_easy_setopt(cc.curl, CURLOPT_USERAGENT,       cc.gi.agent_arg);
    curl_easy_setopt(cc.curl, CURLOPT_FOLLOWLOCATION,  1);
    curl_easy_setopt(cc.curl, CURLOPT_AUTOREFERER,     1);
    curl_easy_setopt(cc.curl, CURLOPT_NOBODY,          0);
    curl_easy_setopt(cc.curl, CURLOPT_VERBOSE,         cc.gi.debug_given);
    curl_easy_setopt(cc.curl, CURLOPT_ERRORBUFFER,     cc.curl_errmsg);
    if (cc.gi.proxy_given)
        curl_easy_setopt(cc.curl, CURLOPT_PROXY, cc.gi.proxy_arg);
    if (cc.gi.limit_rate_given) {
        curl_off_t r = (curl_off_t)cc.gi.limit_rate_arg*1024;
        curl_easy_setopt(cc.curl, CURLOPT_MAX_RECV_SPEED_LARGE, r);
    }
}

static void
run_subseq_sep (void) {
    llst_node_t curr = cc.subseq;
    char *cmd=0;
    while (curr != 0) {
        cmd = strrepl(cc.gi.subsequent_arg,"%i",curr->str);
        if (cmd)
            system(cmd);
        free(cmd);
        curr = curr->next;
    }
}

static void
run_subseq_all (void) {
    size_t rsize = strlen(cc.gi.subsequent_arg);
    llst_node_t curr = cc.subseq;
    char *cmd=0;

    while (curr != 0) { /* calc. req. space */
        rsize += strlen(curr->str)+1;
        curr = curr->next;
    }

    cmd = malloc(rsize+2); /* +1 whitespace for trailing cmd, +1 for '\0' */
    if (cmd) {
        char *arg = strrepl(cc.gi.subsequent_arg,"%i"," ");
        if (arg) {
            cmd[0] = '\0';
            strcat(cmd,arg);
            free(arg);
            strcat(cmd," ");

            curr = cc.subseq;
            while (curr != 0) {
                strcat(cmd,curr->str);
                strcat(cmd," ");
                curr = curr->next;
            }
            system(cmd);
        }
        free(cmd);
    }
    else
        perror("malloc");
}

static void /* run subsequent command */
run_subseq (void) {
    if (!strcmp(cc.gi.subsequent_mode_arg,"sep"))
        run_subseq_sep();
    else
        run_subseq_all();
}

static void /* function to be called at exit */
handle_exit (void) {
    cmdline_parser_free(&cc.gi);
    curl_easy_cleanup(cc.curl);
    llst_free(&cc.subseq);
    free(cc.curl_errmsg);
    free(cc.pp);
}

static const char copyr_notice[] =
"Copyright (C) 2009 Toni Gundogdu <legatvs@gmail.com>.\n\n"
"This program is free software: you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation, either version 3 of the License, or\n"
"(at your option) any later version.\n\n"
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n\n"
"You should have received a copy of the GNU General Public License\n"
"along with this program.  If not, see <http://www.gnu.org/licenses/>.";

int /* entry point */
main (int argc, char *argv[]) {
    memset(&cc,0,sizeof(cc));
    atexit(handle_exit);

    if ( !(cc.curl_errmsg=malloc(CURL_ERROR_SIZE)) ) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    cc.curl_errmsg[0] = '\0';

    parse_opts(argc,argv);

    if (cc.gi.version_given) {
        curl_version_info_data *c = curl_version_info(CURLVERSION_NOW);
        cc_log("%s version %s with libcurl version %s  [%s].\n%s\n",
            CMDLINE_PARSER_PACKAGE, CMDLINE_PARSER_VERSION,
            c->version, OSNAME, copyr_notice);
        exit(EXIT_SUCCESS);
    }

    init_curl();

    if (cc.gi.supported_hosts_given) {
        list_hosts();
        exit(EXIT_SUCCESS);
    }

    if (cc.gi.youtube_user_given) {
        if (login_youtube() != 0)
            exit(EXIT_FAILURE);
    }

    if (!cc.gi.inputs_num) {
        const size_t size = 1024;
        char *ln = malloc(size);
        int l;

        if (!ln) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        while (fgets(ln,size,stdin)) {
            l = strlen(ln);
            if (l < 8)
                continue;
            if (ln[l-1] == '\n')
                ln[l-1] = '\0';
            if (strlen(ln))
                handle_host(ln);
        }
        free(ln);
    } else {
        int i;
        for (i=0; i<cc.gi.inputs_num; ++i) {
            handle_host(cc.gi.inputs[i]);
        }
    }

    if (cc.gi.subsequent_given)
        run_subseq();

    exit(EXIT_SUCCESS);
}
