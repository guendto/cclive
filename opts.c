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

#include "cclive.h"

void /* parse cmdline opts and config (if exists) */
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
