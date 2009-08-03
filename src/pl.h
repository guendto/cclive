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

#ifndef pl_h
#define pl_h

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>

#include "singleton.h"

class PerlMgr : public Singleton<PerlMgr> {
public:
    PerlMgr();
    PerlMgr(const PerlMgr&);
    PerlMgr& operator=(const PerlMgr&);
    virtual ~PerlMgr();
private:
    void _init        ();
    void _cleanup     ();
public:
    void parseTitle  (const std::string& html,
                      VideoProperties& props);
    void filterTitle (VideoProperties& props);
private:
    PerlInterpreter *pi;
};

#define perlmgr PerlMgr::getInstance()

#endif
