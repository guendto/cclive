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

#include <tr1/memory>
#include <iterator>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#include "except.h"
#include "video.h"
#include "opts.h"
#include "util.h"
#include "log.h"
#include "exec.h"

ExecMgr::ExecMgr()
    : queue(new propvect), mode(Plus)
{
}

void
ExecMgr::verifyExecArgument() {
    const Options opts = optsmgr.getOptions();
    if (opts.exec_given) {
        const int  l = strlen(opts.exec_arg);
        const char c = opts.exec_arg[l - 1];
        if (c != ';' && c != '+') {
            throw RuntimeException(CCLIVE_OPTARG,
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

static void
invokeCommand(const std::string& cmd, const char *what=0) {

    if (what) {
        logmgr.cout() << "invoke "
                      << what
                      << " ..."
                      << std::flush;
    }

    int n = system(cmd.c_str());

    std::stringstream tmp;

    switch (n) {
    case 0:
        if (what)
            logmgr.cout() << "done." << std::endl;
        break;
    case -1:
        tmp << "failed to execute: `" << cmd << "'";
        throw RuntimeException(CCLIVE_SYSTEM, tmp.str());
    default:
        tmp << "child exited with: " << (n >> 8);
        throw RuntimeException(CCLIVE_SYSTEM, tmp.str());
    }
}

typedef std::vector<VideoProperties> VIDV;

void
ExecMgr::playPlus() {
    const Options opts = optsmgr.getOptions();

    std::string cmd = opts.exec_arg;
    Util::subStrReplace(cmd, "%i", " ");
    Util::subStrReplace(cmd, "+",  "");
    cmd += " ";

    for (VIDV::const_iterator iter=queue->begin();
        iter != queue->end();
        ++iter)
    {
        cmd += " \"";
        cmd += (*iter).getFilename();
        cmd += "\"";
    }

    invokeCommand(cmd, "--exec");
}

void
ExecMgr::playSemi() {
    const Options opts = optsmgr.getOptions();

    for (VIDV::const_iterator iter = queue->begin();
        iter != queue->end();
        ++iter)
    {
        std::stringstream fname;
        fname << "\"" << (*iter).getFilename() << "\"";

        std::string cmd = opts.exec_arg;
        Util::subStrReplace(cmd, "%i", fname.str());
        Util::subStrReplace(cmd, ";", "");

        invokeCommand(cmd, "--exec");
    }
}

void
ExecMgr::passStream(const VideoProperties& props) {
    std::string cmd = 
        optsmgr.getOptions().stream_exec_arg;

    std::stringstream lnk;
    lnk << "\"" << props.getLink() << "\"";

    Util::subStrReplace(cmd, "%i", lnk.str());
    invokeCommand(cmd, "--stream-exec");
}

void
ExecMgr::playStream(const VideoProperties& props) {
    std::string cmd = optsmgr.getOptions().stream_exec_arg;

    std::stringstream fname;
    fname << "\"" << props.getFilename() << "\"";

    Util::subStrReplace(cmd, "%i", fname.str());
    invokeCommand(cmd);
}


