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
    system(cmd.c_str());
}

void
ExecMgr::playSemi() {
    Options opts = optsmgr.getOptions();

    typedef std::vector<VideoProperties> vv;

    for (vv::iterator iter = queue->begin();
        iter != queue->end();
        ++iter)
    {
        std::string cmd = opts.exec_arg;

        Util::subStrReplace(cmd, "%i", (*iter).getFilename());
        Util::subStrReplace(cmd, ";", "");

        system(cmd.c_str());
    }
}

void
ExecMgr::playStream(const VideoProperties& props) {
    Options opts = optsmgr.getOptions();
    std::string cmd = opts.stream_exec_arg;
    Util::subStrReplace(cmd, "%i", props.getFilename());
    system(cmd.c_str());
}
