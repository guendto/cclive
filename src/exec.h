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

#ifndef exec_h
#define exec_h

#include "singleton.h"

class ExecMgr : public Singleton<ExecMgr> {
public:
    ExecMgr();
public:
    void verifyExecArgument();
    void append(const QuviVideo&); 
    void playQueue();
    void playStream(const QuviVideo&);
    void passStream(QuviVideo&);
private:
    void playPlus();
    void playSemi();
private:
    enum Mode { Plus, Semi };
private:
    typedef std::vector<QuviVideo> vquvi;
    std::tr1::shared_ptr<vquvi> queue;
    Mode mode;
};

#define execmgr ExecMgr::getInstance()

#endif


