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
#ifdef WITH_SIGWINCH
#include <signal.h>
#endif
#include <curl/curl.h>

#include "cclive.h"

struct cclive_s cc;

extern void handle_sigwinch(int); /* progress.c */

static void /* parse cmdline opts and config (if exists) */
parse_opts (const int argc, char **argv) {
    int noconf=1;
    char *home=0;

    if ((home = getenv("HOME")) != 0) {
        char *path=0;
        asprintf(&path,"%s/.ccliverc",home);

        if (path && file_exists(path) != 0) {
            struct cmdline_parser_params *pp = cmdline_parser_params_create();
            pp->check_required      = 0;
            if (cmdline_parser_config_file(path,&cc.gi,pp) == 0) {
                pp->initialize      = 0;
                pp->override        = 1;
                pp->check_required  = 1;
                if (cmdline_parser_ext(argc,argv,&cc.gi,pp) == 0)
                    noconf = 0;
            }
            FREE(pp);
        }
        FREE(path);
    } else {
        fprintf(stderr,
            "warn: HOME environment variable not defined\n"
            "warn: config will not be parsed\n");
    }

    if (noconf) {
        if (cmdline_parser(argc,argv,&cc.gi) != 0)
            exit(EXIT_FAILURE);
    }
}

static void /* init curl handle which will be reused */
init_curl (void) {
    char *proxy = cc.gi.proxy_arg;

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

    if (cc.gi.no_proxy_given)
        proxy = "";

    curl_easy_setopt(cc.curl, CURLOPT_PROXY, proxy);
}

static void
exec_semi (void) {
    llst_node_t curr = cc.fnames;
    char *cmd=0;
    while (curr != 0) {
        cmd = strrepl(cc.gi.exec_arg,"%i",curr->str);
        if (cmd) {
            strrmch(cmd,';');
            system(cmd);
        }
        FREE(cmd);
        curr = curr->next;
    }
}

static void
exec_plus (void) {
    char *arg=0, *cmd=malloc(_POSIX_ARG_MAX);
    llst_node_t curr=cc.fnames;
    int exceeds=0;

    if (!cmd) {
        perror("malloc");
        return;
    }
    cmd[0] = '\0';

    arg = strrepl(cc.gi.exec_arg,"%i"," "); /* strip any %i */
    strlcat(cmd, arg, _POSIX_ARG_MAX);
    strrmch(cmd,'+');
    strlcat(cmd, " ", _POSIX_ARG_MAX);
    FREE(arg);

    while (curr != 0) {
        if (strlcat(cmd, curr->str, _POSIX_ARG_MAX) >= _POSIX_ARG_MAX)
            exceeds=1;
        if (strlcat(cmd, " ", _POSIX_ARG_MAX) >= _POSIX_ARG_MAX)
            exceeds=1;
        if (exceeds) {
            cc_log("warning: exceeded _POSIX_ARG_MAX (%d)\n",_POSIX_ARG_MAX);
            break;
        }
        curr = curr->next;
    }
    system(cmd);
    FREE(cmd);
}

static void /* function to be called at exit */
handle_exit (void) {
    cmdline_parser_free(&cc.gi);
    curl_easy_cleanup(cc.curl);
    llst_free(&cc.fnames);
    FREE(cc.curl_errmsg);
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
    int exec_mode;

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

    if (cc.gi.exec_given) {
        const int  l = strlen(cc.gi.exec_arg);
        const char c = cc.gi.exec_arg[l-1];
        if (c != ';' && c != '+') {
            cc_log("error: --exec expression must be terminated "
                "by either ';' or '+'\n");
            exit(EXIT_FAILURE);
        }
        exec_mode = c;
    }

    if (cc.gi.youtube_user_given) {
        if (login_youtube() != 0)
            exit(EXIT_FAILURE);
    }

#ifdef WITH_SIGWINCH
    signal(SIGWINCH, handle_sigwinch);
#endif

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
        FREE(ln);
    } else {
        int i;
        for (i=0; i<cc.gi.inputs_num; ++i) {
            handle_host(cc.gi.inputs[i]);
        }
    }

    if (cc.gi.exec_given) {
        if (exec_mode == ';')
            exec_semi();
        else
            exec_plus();
    }

    return(EXIT_SUCCESS);
}
