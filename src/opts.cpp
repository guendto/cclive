/*
 * Copyright (C) 2009,2010 Toni Gundogdu.
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

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#include "except.h"
#include "macros.h"
#include "util.h"
#include "log.h"
#include "opts.h"

OptionsMgr::OptionsMgr()
    : opts(Options()), path(""), locale("undefined")
{ }

OptionsMgr::~OptionsMgr() {
    cmdline_parser_free(&opts);
}

void
OptionsMgr::init (int argc, char **argv) {

    const char *tmp = setlocale(LC_ALL, "");

    if (tmp) {
        locale = tmp;
#ifdef HAVE_NL_LANGINFO
        const char *tocode =
            nl_langinfo(CODESET);
        if (tocode)
            locale = tocode;
#endif
    }

    const char *no_config =
        getenv("CCLIVE_NO_CONFIG");

    const char *home = getenv("CCLIVE_HOME");
    if (!home)
        home = getenv("HOME");

    const std::string
#ifndef HOST_W32
    fname = "/.ccliverc";
#else
    fname = "\\ccliverc";
#endif

    bool no_conf = true;

    if (home && !no_config) {
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

const std::string&
OptionsMgr::getLocale() const {
    return locale;
}


