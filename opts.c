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
#include <assert.h>

#include "cclive.h"

void /* parse cmdline opts and config (if exists) */
cc_parse_opts (int argc, char **argv) {
    int noconf;
    char *env;

    assert(argv != 0);

    noconf = 1;

    if ((env = getenv("HOME")) != 0) {
        char *c;
        asprintf(&c,"%s/.ccliverc",env);

        if (cc_file_exists(c) != 0) {
            int rc;

            cc.pp = cmdline_parser_params_create();
            cc.pp->check_required = 0;

            rc = cmdline_parser_config_file(c,&cc.gi,cc.pp);
            free(c);

            if (rc != 0)
                exit(EXIT_FAILURE);

            cc.pp->initialize      = 0;
            cc.pp->override        = 1;
            cc.pp->check_required  = 1;

            if (cmdline_parser_ext(argc,argv,&cc.gi,cc.pp) != 0)
                exit(EXIT_FAILURE);

            noconf = 0;
        }
        else
            free(c);
    } else {
        fprintf(stderr,
            "warn: HOME environment variable not defined\n"
            "warn: config will not be parsed\n");
    }

    if (noconf) {
        if (cmdline_parser(argc,argv,&cc.gi) != 0)
            exit(EXIT_FAILURE);
    }

    if (!cc.gi.proxy_given) {
        if ((cc.http_proxy_env = getenv("http_proxy")) != 0) {
            cc.gi.proxy_given = 1;
            cc.gi.proxy_arg   = cc.http_proxy_env;
        }
    }

    if (cc.gi.no_proxy_given)
        cc.gi.proxy_given = 0;
}
