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

#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iterator>
#include <tr1/memory>

#include "except.h"
#include "video.h"
#include "singleton.h"
#include "cmdline.h"
#include "opts.h"
#include "util.h"
#include "exec.h"

ExecMgr::ExecMgr()
    : queue(new propvect), mode(Plus)
{
}

void
ExecMgr::verifyExecArgument() {
    Options opts = optsmgr.getOptions();
    if (opts.exec_given) {
        const int l = strlen(opts.exec_arg);
        const char c = opts.exec_arg[l - 1];
        if (c != ';' && c != '+') {
            throw RuntimeException(
                "--exec expression must be terminated "
                "by either ';' or '+'");
        }
        mode = c == ';' ? Semi:Plus;
    }
}

void
ExecMgr::append(const VideoProperties& props) {
    queue->push_back(props);
}

void
ExecMgr::playQueue() {
    if (mode == Plus)
        playPlus();
    else
        playSemi();
}

void
ExecMgr::playPlus() {
    Options opts = optsmgr.getOptions();

    std::string cmd = opts.exec_arg;
    Util::subStrReplace(cmd, "%i", " ");
    Util::subStrReplace(cmd, "+",  "");
    cmd += " ";

    typedef std::vector<VideoProperties> vv;

    for (vv::iterator iter=queue->begin();
        iter != queue->end();
        ++iter)
    {
        cmd += " ";
        cmd += (*iter).getFilename();
    }
    int n = system(cmd.c_str());
    if (n != 0) {
        // TODO:
    }
}

void
ExecMgr::playSemi() {
    Options opts = optsmgr.getOptions();

    typedef std::vector<VideoProperties> vv;
    int n;

    for (vv::iterator iter = queue->begin();
        iter != queue->end();
        ++iter)
    {
        std::string cmd = opts.exec_arg;

        Util::subStrReplace(cmd, "%i", (*iter).getFilename());
        Util::subStrReplace(cmd, ";", "");

        n = system(cmd.c_str());
        if (n != 0) {
            // TODO:
        }
    }
}

void
ExecMgr::playStream(const VideoProperties& props) {
    Options opts = optsmgr.getOptions();
    std::string cmd = opts.stream_exec_arg;
    Util::subStrReplace(cmd, "%i", props.getFilename());
    int n = system(cmd.c_str());
    if (n != 0) {
        // TODO:
    }
}
