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

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

#include "error.h"
#include "except.h"
#include "cmdline.h"
#include "util.h"
#include "singleton.h"
#include "macros.h"
#include "log.h"
#include "opts.h"

OptionsMgr::OptionsMgr()
    : opts(Options())
{
}

OptionsMgr::~OptionsMgr() {
    cmdline_parser_free(&opts);
}

void
OptionsMgr::init(int argc, char **argv) {
    bool noconf = true;
    char *home  = NULL;

    if ((home = getenv("HOME")) != NULL) {
        std::string path = std::string(home) + "/.ccliverc";

        if (Util::fileExists(path) > 0) {
            cmdline_parser_params *pp =
                cmdline_parser_params_create();

            pp->check_required = 0;

            if (cmdline_parser_config_file(
                const_cast<char *>(path.c_str()), &opts, pp) == 0)
            {
                pp->initialize      = 0;
                pp->override        = 1;
                pp->check_required  = 1;
                if (cmdline_parser_ext(argc, argv, &opts, pp) == 0)
                    noconf = false;
            }
            _FREE(pp);
        }
    }
    else {
        logmgr.cerr() << "warn: HOME environment variable not defined\n"
            << "warn: config file will not be parsed"
            << std::endl;
    }

    if (noconf) {
        if (cmdline_parser(argc, argv, &opts) != 0)
            throw RuntimeException();
    }
}

const Options&
OptionsMgr::getOptions() const {
    return opts;
}
