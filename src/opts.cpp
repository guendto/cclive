/*
 * Copyright (C) 2009 Toni Gundogdu.
 *
 * This file is part of cclive.
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

#include "except.h"
#include "macros.h"
#include "util.h"
#include "log.h"
#include "opts.h"

OptionsMgr::OptionsMgr()
    : opts(Options()), path("")
{
}

OptionsMgr::~OptionsMgr() {
    cmdline_parser_free(&opts);
}

void
OptionsMgr::init(int argc, char **argv) {

    char *home = getenv("CCLIVE_HOME");
    if (!home)
        home = getenv("HOME");

    std::string fname = "/.ccliverc";
#ifdef HOST_W32
    fname = "\\ccliverc";
#endif

    bool no_conf = true;

    if (home) {
        path = std::string(home) + fname;

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
                    no_conf = false;
            }
            _FREE(pp);
        }
    }

    if (no_conf) {
        if (cmdline_parser(argc, argv, &opts) != 0)
            throw RuntimeException();
    }
}

const Options&
OptionsMgr::getOptions() const {
    return opts;
}

const std::string&
OptionsMgr::getPath() const {
    return path;
}
