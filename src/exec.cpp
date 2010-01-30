/*
 * Copyright (C) 2009,2010 Toni Gundogdu.
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

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <string>

#include "except.h"
#include "quvi.h"
#include "opts.h"
#include "util.h"
#include "log.h"
#include "exec.h"

ExecMgr::ExecMgr()
    : queue(new vquvi), mode(Plus)
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
ExecMgr::append(const QuviVideo& qv) {
    queue->push_back(qv);
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
        logmgr.cout()
            << "invoke "
            << what
            << " ..."
            << std::flush;
    }

    const int n = system( cmd.c_str() );

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

#define try_while \
    try { while (1) {

#define end_try_while \
    } } catch (const QuviNoVideoLinkException&) { }

void
ExecMgr::playPlus() {

    const Options opts = optsmgr.getOptions();

    std::string cmd = opts.exec_arg;
    Util::subStrReplace(cmd, "%i", " ");
    Util::subStrReplace(cmd, "+",  "");
    cmd += " ";

    for (vquvi::iterator iter=queue->begin();
        iter != queue->end();
        ++iter)
    {
        (*iter).resetVideoLink();
        try_while
            cmd += " \"";
            cmd += (*iter).getFileName();
            cmd += "\"";
            (*iter).nextVideoLink();
        end_try_while
    }

    invokeCommand(cmd, "--exec");
}

void
ExecMgr::playSemi() {

    const Options opts = optsmgr.getOptions();

    for (vquvi::iterator iter = queue->begin();
        iter != queue->end();
        ++iter)
    {
        (*iter).resetVideoLink();
        try_while
            std::stringstream fname;
            fname << "\"" << (*iter).getFileName() << "\"";

            std::string cmd = opts.exec_arg;
            Util::subStrReplace(cmd, "%i", fname.str());
            Util::subStrReplace(cmd, ";", "");

            invokeCommand(cmd, "--exec");

            (*iter).nextVideoLink();
        end_try_while
    }
}

void
ExecMgr::passStream(QuviVideo& qv) {
    qv.resetVideoLink();
    try_while
        std::string cmd = 
            optsmgr.getOptions().stream_exec_arg;

        std::stringstream lnk;
        lnk << "\"" << qv.getFileUrl() << "\"";
 
        std::stringstream fname;
        fname << "\"" << qv.getFileName() << "\"";

        Util::subStrReplace(cmd, "%i", lnk.str());
        Util::subStrReplace(cmd, "%f", fname.str());

        invokeCommand(cmd, "--stream-exec");

        qv.nextVideoLink();
    end_try_while
}


